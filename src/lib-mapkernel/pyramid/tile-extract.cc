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

//******************************************************************************
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

//******************************************************************************
CDart* CTile::makeTileBorder()
{
  //std::cout<<" [start] CTile::makeBorder"<<std::endl;
  CDart *first, *last, *tmp;
  uint x,y;

  CDoublet doublet(0, 0, YPOS);
  first = addMapDart(doublet, getInclusionTreeRoot());

  // Ajout du brin de la diagonale
  doublet.setY(1);
  tmp = addMapDart(doublet, getInclusionTreeRoot());
  linkBeta1(first, tmp);
  last = tmp;

  // On crée le bord supérieur de l'image
  x = width();
  y = 0;

  // Initialisation du doublet
  doublet.setY(y);
  doublet.setLinel(XNEG);

  // Parcours du bord supérieur de "droite à gauche"
  // et ajout des brins créés à la carte.
  while(x>0)
    {
      doublet.setX(x);
      tmp = addMapDart(doublet, getInclusionTreeRoot());
      assert(tmp!=NULL);
      linkBeta1(last, tmp);
      last=tmp;
      --x;
    }
  linkBeta1(last,first); // on ferme le bord

  //std::cout<<" [end] CTile::makeBorder"<<std::endl;
  return first;
}

//------------------------------------------------------------------------------
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

  CRegion* currentRegion = NULL;
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
	    currentRegion = getInclusionTreeRoot();
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
	      CPyramidalRegion* downRegion = static_cast<CPyramidalRegion*>(currentRegion);
	      CPyramidalRegion* upRegion   = static_cast<CPyramidalRegion*>
		(FMatrixPixelRegion->getValue( up.getX(), up.getY() ));

	      downRegion->setUp(upRegion);
	      if(upRegion)
		downRegion->setLabel((CPyramidalRegion::Label)(upRegion->label()));

	      if(upRegion!=NULL && !upRegion->existDown())
		upRegion->setDown(downRegion);

	      downRegion->setFirstPixel(CPoint2D(x,y));
	      downRegion->setLabel((CPyramidalRegion::Label) upRegion->label());
	      //On initialise le pointeur des fusions (FNextSameCC)
	      //Par defaut, la region pointe sur elle même.
	      currentRegion->setNextSameCC(currentRegion);

	      // Et on la rajoute à  la fin de la liste.
	      // Rappel, la dernière cellule est désigné par le frère
	      // de FInclusionTreeRoot, et le chainage s'effectue
	      // par les pointeurs fils et frère.
	      getInclusionTreeRoot()->getBrother()->setFirstSon(currentRegion); // next
	      currentRegion->setBrother(getInclusionTreeRoot()->getBrother()); // prev
	      getInclusionTreeRoot()->setBrother(currentRegion);
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
		vertexRemovalWithUnionFindRegions(vertex);
	      else
		setPCell(current,true);
	    }
	  last = nextLast;
	}
    }
  destroyBorder(last);
  //std::cout<<" [end] CTile::extractMainLoop"<<std::endl;
}

//------------------------------------------------------------------------------
void CTile::extractMap( CTile* ATileUp,
			const SegmentationMode & ASegmentationMode,
			const FocusAttentionMode & AFocusAttentionMode)
{
  //std::cout<<" [start] CTile::extractMap"<<std::endl;
  assert(isOk());
  assert(getNbRegions()==0);

  setInclusionTreeRoot( addMapInfiniteRegion() );
  assert(getInclusionTreeRoot()!=NULL);
  getInclusionTreeRoot()->setBrother   ( getInclusionTreeRoot() );
  getInclusionTreeRoot()->setNextSameCC( getInclusionTreeRoot() );

  // 1. On crée le bord supérieur.
  CDart* last = makeTileBorder();
  setRepresentativeDart(getInclusionTreeRoot(), last);

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
//------------------------------------------------------------------------------
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
  FMatrixPixelRegion = createPixelRegionMatrix();
  FMatrixLignelDart  = createLignelDartMatrix();

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
      FClassif = image()->kmeans(2);
      break;

    case SegmentationOff:
      if(depth()==0)
	//classif sur l'ensemble des tuiles filles
	FClassif = image()->kmeans(3);
      break;

    case Histology:
      if(depth()==0)
	assert(FClassif);
//      	{
//      	  FClassif = image()->kmeans(box, depth()+1, 3);
//      	  image()->moveClasses(FClassif, size);
//      	  image()->mergeClasses(2,3, FClassif, size);
//      	}

//      if(depth()==1)
//	{
//	  data = regionsData();
//	  FClassif = image()->kmeansMitosis(data, size, 3);
//	  	  if(FClassif)
//	  	    {
//	  	      std::cout<<"merge and move mitoses classes"<<std::endl;
//	  	      image()->moveClasses(FClassif, size/3, 1);
//	  	      image()->mergeClasses(3,4, FClassif, size/3);
//	  	      image()->mergeClasses(2,3, FClassif, size/3);
//	  	    }
//	}
      //classif sur les régions d'intérêt des tuiles filles
      //      if(depth()==2)
      //	assert(FAssignment);
      //      data = regionsData();
      //      FClassif = image()->kmeansRegions(data, size, 3);
      //      if(FClassif)
      //	{
      //	  image()->moveClasses(FClassif, size);
      //	  image()->mergeClasses(2,3, FClassif, size);
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
	downTile->setMergeThreshold(FMergeThreshold);
	downTile->setPixelMark();
	downTile->FClassif = FClassif;
	downTile->FAssignment = FAssignment;
	downTile->FMatrixPixelRegion = FMatrixPixelRegion;
	downTile->FMatrixLignelDart  = FMatrixLignelDart;

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
    delete [] FClassif;

  delete FMatrixPixelRegion;
  delete FMatrixLignelDart;
  //std::cout<<"[end] CTile::extract\n"<<std::endl;
}
//-----------------------------------------------------------------------------
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
  CRegion* upRegion = FMatrixPixelRegion->getValue( up.x(), up.y() );
  if(!tileUp()->isRegionToSplit(upRegion, AFocusAttentionMode))
    return true; //focus of attention

  // 3. Refinement: apply ASegmentationMode to refine the region
  image()->getPixel( APixel2, index(2) );
  return oracle(APixel1, APixel2, ASegmentationMode);

  //std::cout<<"[end] same regions"<<std::endl;
}

//-----------------------------------------------------------------------------
CDart* CTile::createSquareFace(CDart* ALast, CDart* AUp, CDoublet& ADoublet,
			       CRegion* ARegion)
{
  assert(FMatrixLignelDart);
  CDoublet doublet(ADoublet);

  CPoint2D up = coordinateInParent(doublet.getX(), doublet.getY(), Relative);
  CDoublet upDoublet(up.getX(), up.getY(), doublet.getLinel());

  CPyramidalDart* first  = static_cast<CPyramidalDart*>(addMapDart(ADoublet, ARegion));
  CPyramidalDart* prev   = first;
  CPyramidalDart* actu   = NULL;
  CPyramidalDart* upDart = static_cast<CPyramidalDart*>(FMatrixLignelDart->getDart(upDoublet));

  first->setUp(upDart);
  if(upDart!=NULL )
    upDart->setDown(first);

  for(uint i=0; i<3; ++i)
    {
      doublet.setNextPointel();
      doublet.setNextLinel();
      up = coordinateInParent(doublet.getX(), doublet.getY(), Relative);
      upDoublet.setDoublet(up.getX(), up.getY(), doublet.getLinel());
      actu = static_cast<CPyramidalDart*>(addMapDart(doublet, ARegion));
      upDart = static_cast<CPyramidalDart*>(FMatrixLignelDart->getDart(upDoublet));

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
//-----------------------------------------------------------------------------
void CTile::linkInfiniteUpDown()
{
  if(!existTileUp())
    return;
  assert(FMatrixLignelDart);

  for( CDynamicCoverage1 it( this, getInclusionTreeRoot()->getRepresentativeDart() ); it.cont(); ++it)
    {
      CPyramidalDart* downDart = static_cast<CPyramidalDart*>(*it);
      //TODO ??
      CDoublet down = downDart->doublet();
      CDoublet up = CDoublet(down.getX()/upRatioX(), down.getY()/upRatioY());
      CPyramidalDart* upDart = 	static_cast<CPyramidalDart*>(FMatrixLignelDart->getDart(up));

      downDart->setUp(upDart);
      if(upDart!=NULL)
	upDart->setDown(downDart);
    }
}

//******************************************************************************
unsigned int CTile::
mergeRegions(bool (CRegion::*AMethod)(CRegion*,int)const, int AThreshold)
{
#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   CChrono c;
   c.start();
#endif

   unsigned int nbMerged = 0;

   // 1) On remet les régions en list
   transformInclusionTreeInOrderedList();

   // 2) On ordonne les arêtes par différence de couleur entre les 2 régions
   //    adjacentes
   std::deque< CTopologicalDart* > edgeQueue;
   for (CDynamicCoverageAll it(this); it.cont(); ++it)
   {
      assert( !isMarkedDeleted(*it) );
      if ( *it<beta2(*it) && // On traite seulement un brin par arête
            (getRegion(*it)->*AMethod)(getRegion(beta2(*it)),AThreshold) )
      {
         edgeQueue.push_front( static_cast<CTopologicalDart*>(*it) );
      }
   }
   std::sort< std::deque< CTopologicalDart* >::iterator >
         ( edgeQueue.begin(), edgeQueue.end(), orderEdgesInterval() );

   // 3) On traite chaque arête dans l'ordre croissant pour commencer à
   //    fusionner les régions les plus proches.
   CTopologicalDart* currentDart = NULL;
   CRegion* r1 = NULL;
   CRegion* r2 = NULL;
   while ( !edgeQueue.empty() )
   {
      currentDart = edgeQueue.front();
      edgeQueue.pop_front();
      assert( !isMarkedDeleted(currentDart) );

      r1 = findRegionRoot(getRegion(currentDart));
      r2 = findRegionRoot(getRegion(beta2(currentDart)));
      if ( r1==r2 || (r1->*AMethod)(r2,AThreshold) )
      {
         if (r1!=r2)
         {
            unionRegionRoot(r1,r2);
            ++nbMerged;
         }
         edgeRemoval(currentDart);
      }
   }

   // 4) Il faut reparcourir la carte pour le cas ou il reste des arêtes
   // interne (i.e. incidente 2 fois à la même région)
   for (CDynamicCoverageAll it(this); it.cont(); ++it)
   {
      assert( !isMarkedDeleted(*it) );
      if ( *it<beta2(*it) ) // On traite seulement un brin par arête
      {
         if ( findRegionRoot(getRegion(*it))==
              findRegionRoot(getRegion(beta2(*it))) )
         {
            edgeRemoval(*it);
         }
      }
   }

   // 5) On simplifie la carte et on recalcule l'arbre d'inclusion
   relabelDarts();
   // simplifyMap();

#ifdef DISPLAY_TIME_EXTRACT_IMAGE
   c.stop();
   c.display("Fusion des régions");
#endif

   computeInclusionTree();

   assert(isMapOk());

   return nbMerged;
}
