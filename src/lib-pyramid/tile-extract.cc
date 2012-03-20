/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "inline-macro.hh"
#include "tile.hh"
#include "array.hh"
#include "matrix.hh"
#include "pyramidal-dart.hh"
#include "pyramidal-region.hh"
#include <algorithm>
#include "heap.hh"
#include "dk-doublet.hh"
#include <limits>
#include "chrono.hh"
#include "orders.hh"
#include INCLUDE_NON_INLINE("tile-extract.icc")

using namespace Map2d;
using namespace pyramid;

void CTile::extractMapMainLoop( CDart* ALast,
				const SegmentationMode & ASegmentationMode,
				const FocusAttentionMode & AFocusAttentionMode)
{
  //std::cout<<" [start] CTile::extractMainLoop"<<std::endl;

  //Déclaration des variables
  CDart* last	  = ALast;
  CDart* up	  = NULL;
  CDart* nextLast = NULL;
  CDart* vertex   = NULL;  //brin désignant le sommet

  CPyramidalRegion* currentRegion = NULL;
  bool newRegion = false;
  CDoublet current;

  IM_Pixel pix;

  // Extraction de la carte.
  for (uint y=0; y<height()+1; ++y)
    {
      pix.y=y+ymin();
      for (uint x=0; x<width()+1; ++x)
	{
	  pix.x=x+xmin();
	  image()->getPixel( pix, index(2) );
	  //std::cout<<" traitement du pixel ("<<x<<","<<y<<")"<<std::endl;
	  // 1. On calcule les brins up à  partir du brin last.
	  up = computeUpFromLast(last);

	  //  On cherche la région d'appartenance du pixel courant. Quand on
	  //  ne la trouve pas, c'est que c'est la première fois qu'on la
	  //  rencontre et donc on la crée.
	  currentRegion = NULL;
	  newRegion = false;

	  bool sameLeft = samePixelActuLeft(pix, ASegmentationMode, AFocusAttentionMode);
	  bool sameUp   = samePixelActuUp  (pix, ASegmentationMode, AFocusAttentionMode);

	  if( !isPixelInTile(x, y, Relative) )
	    currentRegion = inclusionTreeRoot();
	  else if( sameLeft && sameUp)
	    {
	      //si les pixels last et up sont identiques
	      //et si les regions de last et up sont différentes
	      //alors ces regions doivent être fusionnées.
	      unionRegionRoot(getRegion(last), getRegion(up));
	      currentRegion = findRegionRoot(getRegion(last));
	    }
	  else if( sameLeft )
	    currentRegion = findRegionRoot(getRegion(last));
	  else if( sameUp )
	    currentRegion = findRegionRoot(getRegion(up));
	  else
	    {
	      // On crée la région.
	      currentRegion = addMapRegion();
	      newRegion = true;

	      // Hiérarchie des régions
	      CPoint2D up = coordinateInParent(x, y, Relative);
	      CPyramidalRegion* downRegion = currentRegion;
	      CPyramidalRegion* upRegion   = static_cast<CPyramidalRegion*>
		(m_matrixPixelRegion->getValue( up.x(), up.y() ));

	      downRegion->setUp(upRegion);
	      if(upRegion)
		downRegion->setLabel((CPyramidalRegion::Label)(upRegion->label()));

	      if(upRegion!=NULL && !upRegion->existDown())
		upRegion->setDown(downRegion);

	      downRegion->setFirstPixel(CPoint2D(x,y));
	      downRegion->setLabel((CPyramidalRegion::Label) upRegion->label());

	      // Et on la rajoute à  la fin de la liste.
	      // Rappel, la dernière cellule est désigné par le frère
	      // de m_inclusionTreeRoot, et le chainage s'effectue
	      // par les pointeurs fils et frère.
	      setFirstSon(inclusionTreeRoot()->brother(), currentRegion); // next
	      setBrother(currentRegion, inclusionTreeRoot()->brother()); // prev
	      setBrother(inclusionTreeRoot(), currentRegion);
	    }

	  // 2. On crée le doublet correspondant au pixel courant qui
	  // est rattaché à  la carte déja créé.
	  current  = CDoublet(x,y,XPOS);
	  nextLast = createSquareFace(last, up, current, currentRegion);

	  if( !currentRegion->isInfiniteRegion() )
	    currentRegion->addPixel( image()->getPixelGreyValue(pix) & 0xFF );

	  // Si on vient de créer la région, on doit
	  // initialiser son représentant et l'assigner à  la tuile.
	  if( newRegion )
	    currentRegion->setRepresentativeDart(beta2(last));

	  //3. On conserve un brin (réel) incident au sommet central.
	  vertex = last;

	  // 4. On teste d'abord les deux suppressions d'arêtes possibles.
	  if( sameLeft )
	    {
	      // Test si c'est le brin représentant le pointel est supprimé
	      if ( beta2(last) == vertex || last==vertex )
		vertex = beta02(vertex);
	      topoEdgeRemoval(last);
	    }
	  else
	    {
	      current.setLinel (YPOS);
	      //std::cout<<" set LCell = "<<current<<std::endl;
	      setLCell(current,true);
	    }
	  if( sameUp )
	    {
	      // Test si c'est le brin représentant le pointel est supprimé
	      if ( beta2(up) == vertex || up==vertex )
		{
		  vertex = beta02(vertex);
		  if ( beta2(up) == vertex || up==vertex )
		    vertex = NULL;
		}
	      topoEdgeRemoval(up);
	    }
	  else
	    {
	      current.setLinel(XPOS);
	      //std::cout<<" set LCell = "<<current<<std::endl;
	      setLCell(current,true);
	    }

	  // 5. Et enfin la suppression de sommet lorsque c'est possible sauf si on
	  // est sur le bord
	  if ( vertex!=NULL && beta1(vertex) != vertex )
	    {
	      // Si on n'est pas dans le cas d'une boucle
	      // Si le sommet est de degré 2 et qu'il
	      // ne s'agit pas d'un cas dégénéré
	      if( beta02(vertex) == beta21(vertex) &&
		  beta0(vertex) != beta2(vertex) &&
		  !isPixelOnBorder(x,y,Relative) )
		vertexRemoval(vertex);
	      else
		setPCell(current,true);
	    }
	  last = nextLast;
	}
    }
  destroyBorder(last);
  //std::cout<<" [end] CTile::extractMainLoop"<<std::endl;
}

void CTile::extractMap( CTile* ATileUp,
			const SegmentationMode & ASegmentationMode,
			const FocusAttentionMode & AFocusAttentionMode)
{
  //std::cout<<" [start] CTile::extractMap"<<std::endl;
  assert(isOk());
  assert(getNbRegions()==0);

  setInclusionTreeRoot( addMapInfiniteRegion() );
  assert(inclusionTreeRoot());
  setBrother( inclusionTreeRoot(), inclusionTreeRoot() );
  setNextSameCC( inclusionTreeRoot(), inclusionTreeRoot() );

  // 1. On crée le bord supérieur.
  CDart* last = makeBorder(width(), height());
  setRepresentativeDart(inclusionTreeRoot(), static_cast<CPyramidalDart*>(last));

  // 2. On parcours l'image en faisant les fusions nécessaires.
  extractMapMainLoop( last, ASegmentationMode, AFocusAttentionMode );

  //std::cout<<" regions avant relabel = "<<getNbRegions()<<std::endl;

  //up/down pour les bords gauche/haut de la région infinie
  linkInfiniteUpDown();


  // 3. On relabélise les brins des régions fusionnées avec d'autres régions.
  relabelDarts();

  //std::cout<<" regions après relabel = "<<getNbRegions()<<std::endl;
  //printChainRegionList();
  // 4. On calcule l'arbre d'inclusion des régions.
  computeInclusionTree();

  //removeDanglingEdges();
  //todo: ça appelle le relabel de topological map au lieu de pyramidal ..
  //removeSmallRegions(1);
  //simplifyMap();

  assert(isOk());
  assert(checkHierarchy());
  //std::cout<<" [end] CTile::extractMap"<<std::endl;
}

void CTile::extractChildren(std::deque<CTile*>& AChildren,
			    const SegmentationMode & ASegmentationMode,
			    const ProjectionMode & AProjectionMode,
			    const FocusAttentionMode & AFocusAttentionMode,
			    CChrono & AChrono)
{
  //std::cout<<"\n[start] CTile::extractChildren of parent "
  //	   <<index(0)<<","<<index(1)<<","<<index(2)<<std::endl;

  assert(isOk()); assert(AChildren.empty());
  uint ratiox = downRatioX();  uint ratioy = downRatioY();

  // les structures de la tuile mère
  loadImage();
  m_matrixPixelRegion = createPixelRegionMatrix();
  m_matrixLignelDart  = createLignelDartMatrix();

  //Box correspondant à l'ensemble des tuiles filles
  IM_Box box;
  box.XTop = xmin()*ratiox;  box.YTop = ymin()*ratioy;
  box.XBtm = xmax()*ratiox;  box.YBtm = ymax()*ratioy;

  image()->setDepth(depth()+1);
  image()->setCurrentBox(box);


  //uint8* data;
  //uint size = width()*ratiox*height()*ratioy*3;

  AChrono.start();
  switch( ASegmentationMode )
    {
    case Classif:
      //classif sur l'ensemble des tuiles filles
      m_classif = image()->kmeans(2);
      break;

    case SegmentationOff:
      if(depth()==0)
	//classif sur l'ensemble des tuiles filles
	m_classif = image()->kmeans(3);
      break;

    case Histology:
      if(depth()==0)
	assert(m_classif);
//      	{
//      	  m_classif = image()->kmeans(box, depth()+1, 3);
//      	  image()->moveClasses(m_classif, size);
//      	  image()->mergeClasses(2,3, m_classif, size);
//      	}

//      if(depth()==1)
//	{
//	  data = regionsData();
//	  m_classif = image()->kmeansMitosis(data, size, 3);
//	  	  if(m_classif)
//	  	    {
//	  	      std::cout<<"merge and move mitoses classes"<<std::endl;
//	  	      image()->moveClasses(m_classif, size/3, 1);
//	  	      image()->mergeClasses(3,4, m_classif, size/3);
//	  	      image()->mergeClasses(2,3, m_classif, size/3);
//	  	    }
//	}
      //classif sur les régions d'intérêt des tuiles filles
      //      if(depth()==2)
      //	assert(m_assignment);
      //      data = regionsData();
      //      m_classif = image()->kmeansRegions(data, size, 3);
      //      if(m_classif)
      //	{
      //	  image()->moveClasses(m_classif, size);
      //	  image()->mergeClasses(2,3, m_classif, size);
      //	}
      break;

    case GreyValue:
      break;
    case Thresholds:
      break;
    default:
      break;
    }
  AChrono.stop();

  //construction des tuiles filles
  for(uint y=0; y<ratioy; ++y)
    for(uint x=0; x<ratiox; ++x)
      {
      	CTile* downTile = new CTile(width(), height());
	downTile->setImage(image());
	downTile->setMergeThreshold(m_mergeThreshold);
	downTile->setPixelMark();
	downTile->m_classif = m_classif;
	downTile->m_assignment = m_assignment;
	downTile->m_matrixPixelRegion = m_matrixPixelRegion;
	downTile->m_matrixLignelDart  = m_matrixLignelDart;

	//coordonnées
	downTile->setBottomRight
	  ( CPoint2D(xmax()*ratiox - (ratiox-1-x)*width() , ymax()*ratioy - (ratioy-1-y)*height()) );
	downTile->setIndex( 0, downTile->xmin()/width() );
	downTile->setIndex( 1, downTile->ymin()/height() );
	downTile->setIndex( 2, index(2)+1);
	linkTileUpDown(this, downTile);

	//extraction
	downTile->projection(AProjectionMode);
	downTile->extractMap(this, ASegmentationMode, AFocusAttentionMode);

	assert(downTile->isOk());

	//if(ASegmentationMode == Histology)

	if(ASegmentationMode == SegmentationOff)
	  {
	    //downTile->removeDanglingEdges();
	    downTile->removeLoops(); //evite le bug avec la projection dijkstra
	  }

	downTile->assignClassifLabelsToRegions(AFocusAttentionMode);

	if(ASegmentationMode == Histology && downTile->depth()==4)
	  downTile->saveMitosis();


	AChildren.push_back(downTile);
      }
  assert(AChildren.size()==ratiox*ratioy);

  if(ASegmentationMode == Classif)
    delete [] m_classif;

  delete m_matrixPixelRegion;
  delete m_matrixLignelDart;
  //std::cout<<"[end] CTile::extract\n"<<std::endl;
}

bool CTile::sameRegions(IM_Pixel & APixel1, IM_Pixel & APixel2,
			const CDoublet & ADoublet,
			const SegmentationMode & ASegmentationMode,
			const FocusAttentionMode & AFocusAttentionMode)
{
  //std::cout<<"[start] same regions"<<std::endl;

  // 1. Causality: if the border exists in previous level, it has been projected
  //    during projection() and must be preserved
  if(isLCell(ADoublet))
    return false;

  // 2. Focus of attention: if the parent region is not a region of interest,
  //    its projection is not refined
  CPoint2D up = coordinateInParent(APixel1.x, APixel1.y, Absolute);
  CRegion* upRegion = m_matrixPixelRegion->getValue( up.x(), up.y() );
  if(!tileUp()->isRegionToSplit(upRegion, AFocusAttentionMode))
    return true; //focus of attention

  // 3. Refinement: apply ASegmentationMode to refine the region
  image()->getPixel( APixel2, index(2) );
  return oracle(APixel1, APixel2, ASegmentationMode);

  //std::cout<<"[end] same regions"<<std::endl;
}

CDart* CTile::createSquareFace(CDart* ALast, CDart* AUp, CDoublet& ADoublet,
			       CRegion* ARegion)
{
  assert(m_matrixLignelDart);
  CDoublet doublet(ADoublet);

  CPoint2D up = coordinateInParent(doublet.getX(), doublet.getY(), Relative);
  CDoublet upDoublet(up.x(), up.y(), doublet.getLinel());

  CPyramidalDart* first  = static_cast<CPyramidalDart*>(addMapDart(ADoublet, ARegion));
  CPyramidalDart* prev   = first;
  CPyramidalDart* actu   = NULL;
  CPyramidalDart* upDart = static_cast<CPyramidalDart*>(m_matrixLignelDart->getDart(upDoublet));

  first->setUp(upDart);
  if(upDart!=NULL )
    upDart->setDown(first);

  for(uint i=0; i<3; ++i)
    {
      doublet.setNextPointel();
      doublet.setNextLinel();
      up = coordinateInParent(doublet.getX(), doublet.getY(), Relative);
      upDoublet.setDoublet(up.x(), up.y(), doublet.getLinel());
      actu = static_cast<CPyramidalDart*>(addMapDart(doublet, ARegion));
      upDart = static_cast<CPyramidalDart*>(m_matrixLignelDart->getDart(upDoublet));

      actu->setUp(upDart);
      if(upDart!=NULL)
	upDart->setDown(actu);

      linkBeta1(prev, actu);
      prev = actu;
    }
  linkBeta1(prev, first);
  linkBeta2(prev,  ALast);
  linkBeta2(first, AUp);

  return beta1(first); //retourne le brin last de la face carré
}

void CTile::linkInfiniteUpDown()
{
  if(!existTileUp())
    return;
  assert(m_matrixLignelDart);

  for( CDynamicCoverage1 it( this, getInclusionTreeRoot()->getRepresentativeDart() ); it.cont(); ++it)
    {
      CPyramidalDart* downDart = static_cast<CPyramidalDart*>(*it);
      //TODO ??
      CDoublet down = downDart->doublet();
      CDoublet up = CDoublet(down.getX()/upRatioX(), down.getY()/upRatioY());
      CPyramidalDart* upDart = 	static_cast<CPyramidalDart*>(m_matrixLignelDart->getDart(up));

      downDart->setUp(upDart);
      if(upDart!=NULL)
	upDart->setDown(downDart);
    }
}

