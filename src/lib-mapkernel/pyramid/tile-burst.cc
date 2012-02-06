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
#include "pyramidal-dart.hh"
#include "pyramidal-region.hh"
#include "array.hh"
#include "macros.hh"
#include <algorithm>

#include INCLUDE_NON_INLINE("tile-burst.icc")

using namespace Map2d;
using namespace pyramid;

//------------------------------------------------------------------------------
CTile* CTile::createChildByCopy(const ProjectionMode & AProjectionMode)
{
  //std::cout<<"\n[start] CTile::copyLinkTile()"<<std::endl;
  uint ratiox = downRatioX();
  uint ratioy = downRatioY();

  CTile* downTile = new CTile(ratiox*width(), ratioy*height());
  downTile->setBottomRight( CPoint2D(xmax()*ratiox, ymax()*ratioy) );
  downTile->setIndex( 0, index(0) );
  downTile->setIndex( 1, index(1) );
  downTile->setIndex( 2, index(2)+1 );
  downTile->setPixelMark();
  linkTileUpDown(this, downTile);

  // L'image
  downTile->setImage(image());
  downTile->loadImage();

  // La géométrie
  if(AProjectionMode==DirectProjection)
    {
      downTile->copyKhalimsky(this);
      // Les brins
      downTile->copyDarts(this);
      // Les régions
      downTile->copyRegions(this);
      // Les pixels
      if(ratiox!=1 || ratioy!=1)
	downTile->traversePixels();
    }
  else if(AProjectionMode==DijkstraProjection)
    {
      if(depth()==0)
	{
	  //classif sur l'ensemble des tuiles filles
	  IM_Box box;
	  box.XTop = xmin()*ratiox;  box.YTop = ymin()*ratioy;
	  box.XBtm = xmax()*ratiox;  box.YBtm = ymax()*ratioy;
	  image()->setDepth(depth()+1);
	  image()->setCurrentBox(box);
	  FClassif = image()->kmeans(3);
	}

      FMatrixPixelRegion = createPixelRegionMatrix();
      FMatrixLignelDart  = createLignelDartMatrix();


      downTile->FClassif = FClassif;
      downTile->FMatrixPixelRegion = FMatrixPixelRegion;
      downTile->FMatrixLignelDart  = FMatrixLignelDart;

      downTile->setKhalimsky(new CKhalimsky(width()*ratiox, height()*ratioy));
      downTile->projectionDijkstraConstant();
      // Les brins
      //downTile->copyDarts(this);
      // Les régions
      //downTile->copyRegions(this);

      downTile->extractMap(this, SegmentationOff, FocusAll);
      assert(downTile->isOk());
      downTile->toSVG();
      if(FClassif)
	delete [] FClassif;

      delete FMatrixPixelRegion;
      delete FMatrixLignelDart;
    }


  //  assert( downTile->isOk() );
  //  assert( downTile->checkGeometry() );
  //  assert( downTile->checkDarts() );
  //  assert( downTile->checkDartLinks() );

  //std::cout<<"[end] CTile::copyLinkTile\n"<<std::endl;

  return downTile;
}
//------------------------------------------------------------------------------
void CTile::copyRegions(CTile* ATileUp)
{
  //this = ATileDown
  uint ratiox = upRatioX();
  uint ratioy = upRatioY();

  CPyramidalDart* upDart = NULL;
  CPyramidalRegion* upRegion = NULL;
  CPyramidalRegion* downRegion = NULL;

  // La région infinie
  CPyramidalRegion* downInfiniteRegion = addMapInfiniteRegion();
  CPyramidalRegion* upInfiniteRegion = static_cast<CInfinitePyramidalRegion*>(ATileUp->getInclusionTreeRoot());
  setInclusionTreeRoot(downInfiniteRegion);
  upInfiniteRegion->setDown(downInfiniteRegion);
  downInfiniteRegion->setUp(upInfiniteRegion);

  // Les régions
  for( CDynamicCoverageAll it( ATileUp ); it.cont();++it )
    {
      upDart = static_cast<CPyramidalDart*>( *it );
      upRegion = ATileUp->getRegion(upDart);

      if(!upRegion->existDown())
	{
	  downRegion = addMapRegion(upRegion);
	  ATileUp->linkRegionUpDown(upRegion, downRegion);
	}
      else
	downRegion = upRegion->down();

      ATileUp->getDartDown(upDart)->setRegion(downRegion);
    }

  // L'arbre d'inclusion
  for( CDynamicCoverageAllRegions it( ATileUp ); it.cont();++it )
    {
      upRegion =  static_cast<CPyramidalRegion*>(*it);
      downRegion = upRegion->down();
      downRegion->setRepresentativeDart(static_cast<CPyramidalDart*>(upRegion->getRepresentativeDart())->down());

      if(upRegion->existNextSameCC())
	downRegion->setNextSameCC(upRegion->getNextSameCC()->down());

      if(upRegion->existFather() )
	downRegion->setFather(upRegion->getFather()->down());

      if(upRegion->existBrother() )
	downRegion->setBrother(upRegion->getBrother()->down());

      if(upRegion->existSon())
	downRegion->setFirstSon(upRegion->getFirstSon()->down());

      CPoint2D upPixel = upRegion->firstPixel();
      downRegion->setFirstPixel(CPoint2D(upPixel.getX()*ratiox, upPixel.getY()*ratioy));
      downRegion->setNbPixels(0);
    }
}

//------------------------------------------------------------------------------
void CTile::copyDarts(CTile* ATileUp)
{
  uint ratiox = upRatioX();
  uint ratioy = upRatioY();

  CPyramidalDart* upDart = NULL;
  CPyramidalDart* downDart = NULL;

  // Les brins
  for( CDynamicCoverageAll it( ATileUp ); it.cont(); ++it )
    {
      upDart = static_cast<CPyramidalDart*>( *it );
      downDart = static_cast<CPyramidalDart*>(addMapDart());

      CDoublet up = upDart->doublet();
      CDoublet down( (up.getX())*ratiox, (up.getY())*ratioy, up.getLinel());
      setDoublet(downDart, down);
      ATileUp->linkDartUpDown(upDart, downDart);
    }

  // Les coutures
  for( CDynamicCoverageAll it( ATileUp ); it.cont(); ++it )
    {
      upDart = static_cast<CPyramidalDart*>( *it );
      downDart = ATileUp->getDartDown(upDart);
      downDart->setBeta0(upDart->getBeta0()->down());
      downDart->setBeta1(upDart->getBeta1()->down());
      downDart->setBeta2(upDart->getBeta2()->down());
    }
}
//------------------------------------------------------------------------------
void CTile::copyKhalimsky(CTile* ATileUp)
{
  uint ratiox = upRatioX();
  uint ratioy = upRatioY();

  // Si la taille de la tuile est la même, on duplique
  if( ratiox==1 && ratioy==1 )
    {
      CKhalimsky* downKhalimsky = new CKhalimsky(*(ATileUp->getKhalimsky()));
      setKhalimsky(downKhalimsky);
    }
  else   // sinon, on dilate
    {
      CKhalimsky* khalimsky = new CKhalimsky(width(), height());
      CDoublet temp; CDoublet down;

      uint width = ATileUp->getKhalimsky()->getSizeX();
      uint height = ATileUp->getKhalimsky()->getSizeY();


      for(uint y=0; y<height; ++y)
	{
	  temp.setY(y);
	  for(uint x=0; x<width; ++x)
	    {
	      temp.setX(x);
	      if( ATileUp->isPCell(temp) )
		{
		  down.setX(x*ratiox); down.setY(y*ratioy);
		  khalimsky->setPCell(down, true);
		}
	      temp.setLinel(XPOS);
	      if( ATileUp->isLCell(temp) )
		{
		  down.setDoublet(x*ratiox, y*ratioy, XPOS);
		  khalimsky->setLCell(down, true);
		  for(uint i=1; i<ratiox; ++i)
		    {
		      down.incX();
		      khalimsky->setLCell(down, true);
		    }
		}
	      temp.setLinel(YPOS);
	      if( ATileUp->isLCell(temp) )
		{
		  down.setDoublet(x*ratiox, y*ratioy, YPOS);
		  khalimsky->setLCell(down, true);
		  for(uint i=1; i<ratioy; ++i)
		    {
		      down.incY();
		      khalimsky->setLCell(down, true);
		    }
		}
	    }
	}
      setKhalimsky(khalimsky);
    }
}
//------------------------------------------------------------------------------
void CTile::burstAndMerge(const FocusAttentionMode & AFocusAttentionMode,
			  const SegmentationMode & ASegmentationMode)
{
  //std::cout<<" start split"<<std::endl;
  CDart*    currentDart   = NULL;
  CRegion*  currentRegion = NULL;
  CPyramidalRegion*  tempRegion    = NULL;

  std::deque<CPyramidalRegion*> initialRegions; // liste de toutes les régions du niveau avant le split
  std::deque<CPyramidalRegion*> newRegions;     // liste actualisée des régions du niveau
  std::deque<CPyramidalRegion*> allRegions;     // liste finale des régions
  std::deque<CDart*> newDarts;         // liste des brins correspondant aux arêtes insérées
  std::stack<CRegion*> toDelete;       // liste des régions qui ont été splittées/refusionnées à supprimer

  std::deque<CPyramidalRegion*>::iterator it;
  std::deque<CPyramidalRegion*>::iterator it2;

  //On récupère toutes les régions du niveau
  for( CDynamicCoverageAllRegions it( this ); it.cont();++it )
    {
      initialRegions.push_back(static_cast<CPyramidalRegion*>(*it));
      splitAllEdgesRegion(*it);
    }

  createChainRegionList(initialRegions);

  for(it=initialRegions.begin(); it!=initialRegions.end(); ++it)
    {
      currentRegion = *it;
      currentDart = currentRegion->getRepresentativeDart();

      if( isRegionToSplit(currentRegion, AFocusAttentionMode) )
	{
	  newDarts.clear();
	  splitRegion(currentDart, newDarts);

	  if(!newDarts.empty())
	    {
	      createNewRegions(currentRegion, newDarts, newRegions);

	      it2= newRegions.begin();
	      tempRegion = *it2;
	      for( ++it2 ; it2!=newRegions.end(); ++it2 )
		{
		  if( (*it2)->firstPixel() <  tempRegion->firstPixel() )
		    tempRegion = *it2;
		}
	      updateRegionList(currentRegion, newRegions);
	      static_cast<CPyramidalRegion*>(currentRegion)->up()
		->setDown(tempRegion);

	      //On indique que la région a été traitée et qu'il faudra la supprimer
	      toDelete.push(currentRegion);

	      // On merge
	      merge(newDarts, ASegmentationMode);
	    }
	}
    }
  relabelDarts();

  //Reconstruction avec ordre total de la chaine
  //CRegion* region = getInclusionTreeRoot()->getFirstSon();
  CRegion* region = getInclusionTreeRoot()->getFirstSon();
  while( region != NULL )
    {
      region->setFather( NULL );
      allRegions.push_back(static_cast<CPyramidalRegion*>(region));
      region = region->getFirstSon();
    }

  std::sort<std::deque<CPyramidalRegion*>::iterator>( allRegions.begin(),
						      allRegions.end(),
						      CLessRegionFirstPixel() );
  allRegions.insert(allRegions.begin(), getInclusionTreeRoot());
  createChainRegionList(allRegions);

  //Suppression des régions qui ont été traitées
  while( !toDelete.empty() )
    {
      region = toDelete.top();
      toDelete.pop();

      region->setFirstSon(NULL);
      region->setBrother(NULL);
      region->setNextSameCC(NULL);
      delRegion(region);
    }
  relabelDarts();

  //std::cout<<" end split"<<std::endl;
}


//******************************************************************************
// Opérations de split
//******************************************************************************

void CTile::splitEdge( CDart* ADart )
{
  //std::cout<<"[start] splitEdge"<<std::endl;
  assert( ADart != NULL );

  CDart*   dart = ADart;
  CDoublet doublet = getDoublet( dart );

  if( isEdgeLoop( dart ) )
    FKhalimsky->setPCell( doublet, true );

  // On suit le plongement de l'arête. Tant que le prochain pointel n'est pas
  // actif (i.e. il n'y a pas de sommet), on poursuit la division.
  // Si le beta2 était représentant, on s'assure de sa position après le split

  CDart* rep = (beta2(dart));
  CRegion* regionBeta2 = getRegion(rep);

  if( isRepresentativeDart(rep) )
    {
      CDoublet min = getDoublet(rep);
      uint minY = min.getY();
      CDoublet tmp;

      while( !isPCell( doublet.getNextPointel() ) )
	{
	  dart = insertVertexOnEdge( dart );
	  doublet = getDoublet(dart);
	  tmp = getDoublet(beta21(dart));
	  uint tmpY = tmp.getY();

	  if( (tmpY < minY) ||
	      (tmpY == minY && tmp.getX() < min.getX()) )
	    {
	      min = tmp;
	      rep = beta21(dart);
	    }
	}
      regionBeta2->setRepresentativeDart(rep);
    }
  // Sinon, division normale
  else
    {
      while( !isPCell( doublet.getNextPointel() ) )
	{
	  dart = insertVertexOnEdge( dart );
	  doublet = getDoublet(dart);
	}
    }
  //std::cout<<"[end] splitEdge"<<std::endl;
}
//------------------------------------------------------------------------------
CDart* CTile::insertEdge( CDart* ADart )
{
  //std::cout<<"[start] insertEdge"<<std::endl;
  assert( ADart != NULL );

  CDart* b  = ADart;
  CDart* b0 = beta0(ADart);
  CDoublet t  = getDoublet(b);

  t.setPrevLinel();
  assert(!isLCell(t));
  CDoublet t1 = t.getNextPointel();

  // On allume le pointel/lignel de l'arête pendante que l'on insère.
  FKhalimsky->setLCell(t1,true);
  FKhalimsky->setPCell(t1,true);

  // Création des nouveaux brins de l'arête.
  CDart* j1 = addMapDart( t, getRegion(b) );
  CDart* j  = addMapDart( t1,getRegion(b) );

  // Couture de l'arête elle même.
  linkBeta1( j1 , j  );
  linkBeta2( j , j1  );

  // Couture de l'arête dans la face.
  linkBeta1( b0, j1 );
  linkBeta1( j, b );
  return beta0( b );
}
//------------------------------------------------------------------------------
CDart* CTile::insertVertexOnEdge( CDart* ADart, const CDoublet& ADoublet1, const CDoublet& ADoublet2 )
{
  //std::cout<<"[start] insertVertexOnEdge"<<std::endl;
  assert( ADart != NULL );

  CPyramidalDart* dart = static_cast<CPyramidalDart*>(ADart);
  CPyramidalDart* upDart = dart->up();

  CDart* i   = ADart;
  CDart* i1  = NULL;

  CDart* i2  = NULL;
  CDart* i21 = NULL;

  CDoublet t1 = ADoublet1;
  CDoublet t2 = ADoublet2;

  // Mise en place des nouveaux brins.
  i1  = beta1(i);
  i2  = beta2(i);
  i21 = beta1(i2);

  CPyramidalDart* j  = static_cast<CPyramidalDart*>(addMapDart( t1, getRegion(i) ));
  CPyramidalDart* j2 = static_cast<CPyramidalDart*>(addMapDart( t2, getRegion(i2) ));

  // Conservation de la relation causale des brins
  if( dart->existUp() )
    {
      j ->setUp(upDart);
      j2->setUp(upDart->getBeta2());
    }

  linkBeta1(i , j ); linkBeta1(j , i1 );
  linkBeta1(i2, j2); linkBeta1(j2, i21);
  linkBeta2(i , j2); linkBeta2(j , i2);

  FKhalimsky->setPCell(getDoublet(j2),true);

  //std::cout<<"[end] insertVertexOnEdge"<<std::endl;
  return beta1(i);
}
//------------------------------------------------------------------------------
void CTile::splitAllEdges()
{
  //std::cout<<"[start] splitAllEdges"<<std::endl;

  int mTreated = getNewMark();
  std::stack< CDart* > toTreat;

  for( CDynamicCoverageAll it(this) ; it.cont() ; it++ )
    if( !isMarked( *it, mTreated ) )
      {
	// Marque l'orbite arête pour ne pas empiler plusieurs fois la
	// même arête.
	markOrbit( *it, ORBIT_EDGE, mTreated );
	// Empile toutes les arêtes.
	toTreat.push(*it);
      }

  while( !toTreat.empty() )
    {
      unmarkOrbit( toTreat.top(), ORBIT_EDGE, mTreated );
      // Division de l'arête en cours.
      splitEdge( toTreat.top() );
      toTreat.pop();
    }

  freeMark(mTreated);
  //std::cout<<"[end] splitAllEdges"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::splitAllEdgesRegion(CRegion* ARegion)
{
  //std::cout<<"[start] splitAllEdgesRegion"<<std::endl;

  CPyramidalRegion* current = static_cast<CPyramidalRegion*>(ARegion);
  std::stack<CDart*> dStack; //La pile des brins correspondant aux arêtes de la région

  // On récupère l'ensemble des brins appartenant aux bords externe/internes de la région
  // en utilisant les relations d'inclusion du niveau supérieur
  // bord externe
  for( CDynamicCoverage1 it( this, current->getRepresentativeDart() ) ; it.cont() ; ++it )
    dStack.push(*it);

  // bords internes
  current = current->up();

  if( current->existSon())
    {
      CPyramidalRegion* son = current->getFirstSon();
      CDart* dart = beta2(son->down()->getRepresentativeDart());
      for(CDynamicCoverage1 it(this, dart); it.cont(); ++it)
	dStack.push(*it);

      current = son;

      while( current->existBrother() )
	{
	  CPyramidalRegion* brother = current->getBrother();
	  CDart* dart2 = beta2(brother->down()->getRepresentativeDart());
	  for(CDynamicCoverage1 it(this, dart2); it.cont(); ++it )
	    dStack.push(*it);

	  current = brother;
	}
    }

  // Division de l'arête
  while(!dStack.empty())
    {
      splitEdge( dStack.top() );
      dStack.pop();
    }
  //std::cout<<"[end] splitAllEdgesRegion"<<std::endl;
}



//------------------------------------------------------------------------------
void CTile::splitRegion( CDart* ADart, std::deque<CDart*>& toLabel )
{
  //std::cout<<"[start] CTile::splitRegion"<<std::endl;

  assert( ADart != NULL );
  assert( isRepresentativeDart(ADart) );
  assert( !getRegion(ADart)->isInfiniteRegion() );
  assert(toLabel.empty());

  std::vector<uint> tab;
  getRegionBoundingBox(ADart, tab);

  CArray* array = new CArray(tab[0], tab[1], tab[2]-tab[0]+1, tab[3]-tab[1]+1);
  std::stack< CDart* > toTreat;

  // Initialisation du tableau de sommets :
  // On remplit le tableau avec tous les brins du bord de la face à diviser.

  // bord externe
  for(CDynamicCoverage1 it(this, ADart); it.cont(); ++it)
    {
      array->setDart(*it);
      toTreat.push(*it);
    }

  // bords internes
  CPyramidalRegion* region = static_cast<CPyramidalRegion*>(getRegion(ADart))->up();
  CPyramidalRegion* firstSon = NULL;
  CPyramidalRegion* brother = NULL;
  assert( region != NULL );

  if(region->existSon())
    {
      firstSon = region->getFirstSon();
      CDart* dart = beta2(firstSon->down()->getRepresentativeDart());
      for( CDynamicCoverage1 it( this, dart ); it.cont() ; ++it )
	{
	  array->setDart(*it);
	  toTreat.push(*it);
	}
      region = firstSon;
      while( region->existBrother() )
	{
	  brother = region->getBrother();
	  CDart* dart2 = beta2(brother->down()->getRepresentativeDart());
	  for( CDynamicCoverage1 it( this, dart2 ) ; it.cont() ; ++it )
	    {
	      array->setDart(*it);
	      toTreat.push(*it);
	    }
	  region = brother;
	}
    }

  // Traitement des brins désignant les arêtes à insérer :
  // On cherche à déterminer l'existence d'une arête perpendiculaire au brin
  // actuellement traité.
  CDart* currentDart = NULL;
  CDart* newDart = NULL;
  CDoublet currentDoublet;
  CDoublet testDoublet;

  assert(!toTreat.empty());
  while(!toTreat.empty())
    {
      currentDart = toTreat.top();
      toTreat.pop();
      currentDoublet = getDoublet( currentDart );
      //assert(isLCell(currentDoublet));

      // Calcul du doublet correspondant à l'arête que l'on va essayer d'ajouter
      testDoublet = currentDoublet.getPrevLinel();

      if(!isLCell(testDoublet))
	{
	  // Insertion de l'arête incidente au sommet traité
	  newDart = insertEdge( currentDart );

	  // Ajout des couples brins/doublet dans le tableau
	  array->setDart(newDart);
	  array->setDart(beta2(newDart));

	  // Ajout des brins de l'arête nouvellement insérée dans la pile
	  // des brins à traiter.
	  assert( newDart != NULL );
	  assert( beta2(newDart) != NULL );

	  toTreat.push( newDart );
	  toTreat.push( beta2(newDart) );

	  // On ajoute un des brins de l'arête à la pile des brins à traiter
	  // lorsque nous allons étiqueter les faces.
	  toLabel.push_front( newDart );
	}
      else if ( beta2(currentDart) == beta0(currentDart) )
	{
	  CDart* tmp = array->getDart( testDoublet.getNextPointel() );
	  assert(tmp != NULL);

	  linkBeta1( beta2(currentDart), beta1(tmp) );
	  linkBeta1( tmp, currentDart );
	}
    }

  std::deque<CDart*>::iterator i;
  for( i=toLabel.begin(); i != toLabel.end(); ++i )
    {
      CDart* b1 = *i;
      CDart* b2 = beta2(b1);

      if( !(getRegion(b2)->getFather() == getRegion(b1)) )
	{
	  CDoublet d1 = getDoublet(b1);
	  CDoublet d2 = d1.getNextPointel();

	  if( b1!=NULL )
	    {
	      CDoublet d3 = d1.getPrevLinel().getNextPointel();
	      CDoublet d5 = d2.getNextLinel();
	      CDart* b3 = array->getDart(d3);
	      CDart* b5 = array->getDart(d5);

	      linkBeta1(b3, b1);
	      linkBeta1(b1, b5);
	    }

	  if( b2!=NULL )
	    {
	      CDoublet d4 = d1.getNextLinel();
	      CDoublet d6 = d2.getPrevLinel().getNextPointel();
	      CDart* b4 = array->getDart(d4);
	      CDart* b6 = array->getDart(d6);
	      linkBeta1(b2, b4);
	      linkBeta1(b6, b2);
	    }
	}
    }

  delete array;

  // Vérification de la topologie de la carte produite
  //assert(checkTopology()); //assert très couteux

  //std::cout<<"[end] CTile::splitRegion"<<std::endl;
}


//------------------------------------------------------------------------------
void CTile::createNewRegions(CRegion* ARegion,
			     std::deque< CDart* >& ADartList,
			     std::deque< CPyramidalRegion* >& ARegionList)
{
  //std::cout<<"[start] CTile::createNewRegions"<<std::endl;
  assert(! ADartList.empty() );
  ARegionList.clear();

  std::deque<CDart*>::iterator it;

  // Reconstruction du marquage des faces
  CPyramidalRegion* region = NULL;
  CPyramidalRegion* tmp = static_cast<CPyramidalRegion*>(ARegion);
  CDart* currentDart = NULL;

  IM_Pixel pix;
  for(it=ADartList.begin(); it != ADartList.end(); ++it)
    {
      currentDart = *it;
      do
	{
	  if( getRegion(currentDart)==tmp )
	    {
	      assert( checkSquareRegion( currentDart ) );
	      region = addMapRegion();
	      region->setUp(tmp->up());
	      for( int i = 0 ; i < 4 ; ++i )
		{
		  setRegion( currentDart, region );
		  currentDart = beta1( currentDart );
		  if( getDoublet(currentDart).getLinel() == XPOS )
		    {
		      region->setRepresentativeDart(currentDart);
		      //On met à jour les infos de la région depuis les coordonnées du brin représentant
		      pix.x = getDoublet(currentDart).getX()+xmin();
		      pix.y = getDoublet(currentDart).getY()+ymin();
		      addPixel(pix, region);
		      region->setFirstPixel( CPoint2D(pix.x-xmin(), pix.y-ymin()) );
		      ARegionList.push_back(region);
		    }
		}
	    }
	  currentDart=beta2(currentDart);
	}
      while(currentDart != *it);
    }
  //std::cout<<"[end] CTile::createNewRegions"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::createChainRegionList(std::deque<CPyramidalRegion*>& AList)
{
  //std::cout<<"[start] CTile::createChainRegionList"<<std::endl;

  // On crée la liste chainée des régions :
  // - 1er élément :   région infinie
  // - getFirstSon() : région suivante
  // - getBrother() :  région précédente
  // - le brother du 1er élément est la dernière région
  // - le son du dernier élément est NULL

  CRegion* prev =  NULL;
  std::deque<CPyramidalRegion*>::iterator it = AList.begin();
  prev = *it;
  prev->setNextSameCC(prev);
  ++it;

  for( ; it != AList.end(); ++it)
    {
      //Initialisation de toutes les régions en les faisant pointer sur elles-même
      (*it)->setNextSameCC(*it);

      //On chaine la liste
      (*it)->setBrother(prev);
      prev->setFirstSon(*it);
      prev = *it;
    }

  //Par défaut; l'élément précédent du 1er élément est le dernier de la liste
  (*AList.begin())->setBrother(prev);
  prev->setFirstSon(NULL);

  //std::cout<<"[end] CTile::createChainRegionList"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::updateRegionList(CRegion* ARegion,
			     std::deque<CPyramidalRegion*>& ANewRegionList)
{
  //std::cout<<"[start] CTile::updateRegionList"<<std::endl;
  createChainRegionList(ANewRegionList);

  CRegion* begin = *ANewRegionList.begin();
  CRegion* end   = begin->getBrother();

  ARegion->getBrother()->setFirstSon(begin);
  begin->setBrother( ARegion->getBrother() );
  end->setFirstSon( ARegion->getFirstSon() );

  if( end->getFirstSon() != NULL )
    end->getFirstSon()->setBrother(end);
  else
    FInclusionTreeRoot->setBrother(end);

  //std::cout<<"[end] CTile::updateRegionList"<<std::endl;
}


//******************************************************************************
//Opérations de merge
//******************************************************************************


void CTile::merge(std::deque<CDart*>& toMerge,
		  const SegmentationMode & ASegmentationMode)
{
  //std::cout<<"\n[start] CTile::merge"<<std::endl;

  CDart* dart       = NULL;
  CRegion* parent1  = NULL;
  CRegion* parent2  = NULL;
  int toDelete = getNewMark();
  std::deque<CDart*>::iterator it;

  // Les fusions d'après le critère et entre régions issues du même parent
  for( it=toMerge.begin(); it!=toMerge.end(); ++it)
    {
      dart = *it;
      if( !isMarked(dart, toDelete ) && getDartUp(dart)==NULL )
	{
	  assert( getDartUp(beta2(dart))==NULL );

	  parent1  = findRegionRoot(getRegion(dart));
	  parent2  = findRegionRoot(getRegion(beta2(dart)));

	  if( isRegionToMerge(parent1, parent2, ASegmentationMode) )
	    {
	      unionRegionRoot(parent1, parent2);
	      mergeEdgeRemoval( dart, toDelete );
	      //edgeRemoval(dart, toDelete);
	    }
	}
    }

  // Les fusions de multi-adjacence
  for( it=toMerge.begin(); it!=toMerge.end(); ++it)
    {
      dart = *it;
      if( !isMarked(dart, toDelete) )
 	{
 	  parent1  = findRegionRoot(getRegion(dart));
 	  parent2  = findRegionRoot(getRegion(beta2(dart)));

 	  if( parent1 == parent2 )
	    {
	      mergeEdgeRemoval( dart, toDelete );
	      //edgeRemoval(dart, toDelete);
	    }
 	}
    }

  // La suppression des brins
  CDart * actu = NULL;
  for( it=toMerge.begin(); it!=toMerge.end(); )
    {
      actu = *it;
      ++it;

      if ( isMarked(beta2(actu), toDelete) )
	{
	  assert(!isRepresentativeDart(beta2(actu)));
	  delMapDart(beta2(actu));
	}

      if ( isMarked(actu, toDelete) )
	{
	  assert(!isRepresentativeDart(actu));
	  delMapDart(actu);
	}
    }

  freeMark(toDelete);

  //Controle des modifications
  assert( isMapOk() );

  //std::cout<<"[end] CTile::merge\n"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::merge()
{
  //std::cout<<"\n[start] CTile::merge"<<std::endl;

  CDart* dart       = NULL;
  CRegion* parent1  = NULL;
  CRegion* parent2  = NULL;
  int toDelete = getNewMark();


  // Les fusions d'après le critère
  for( CDynamicCoverageAll it( this ); it.cont(); ++it )
    {
      dart = *it;
      if( !isMarked(dart, toDelete ) && getDartUp(dart)==NULL )
	{
	  assert( getDartUp(beta2(dart))==NULL );

	  parent1  = findRegionRoot(getRegion(dart));
	  parent2  = findRegionRoot(getRegion(beta2(dart)));

	  if( isRegionToMerge(parent1, parent2) )
	    {
	      unionRegionRoot(parent1, parent2);
	      mergeEdgeRemoval( dart, toDelete );
	      //edgeRemoval(dart);
	    }
	}
    }

  // Les fusions de multi-adjacence
  for( CDynamicCoverageAll it( this ); it.cont(); ++it )
    {
      dart = *it;
      if( !isMarked(dart, toDelete) )
 	{
 	  parent1  = findRegionRoot(getRegion(dart));
 	  parent2  = findRegionRoot(getRegion(beta2(dart)));

 	  if( parent1 == parent2 )
	    {
 	      mergeEdgeRemoval( dart, toDelete );
	      //edgeRemoval(dart);
	    }
 	}
    }

  // La suppression des brins
  CDart * actu = NULL;
  for( CDynamicCoverageAll it( this ); it.cont();  )
    {
      actu = *it;
      ++it;
      if ( isMarked(actu, toDelete) )
	{
	  assert(!isRepresentativeDart(actu));
	  delMapDart(actu);
	}
    }

  freeMark(toDelete);

  //Controle des modifications
  //assert( isMapOk() );

  //std::cout<<"[end] CTile::merge\n"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::mergeEdgeRemoval( CDart* ADart, int AMarkNumber )
{
  //std::cout<<"[start] CTile::mergeEdgeRemoval"<<std::endl;
  assert( ADart!=NULL     );
  assert( !isFree2(ADart) );
  assert( !isFree1(ADart) );
  assert( !isFree0(ADart) );

  // Extinction du lignel
  // Spécifique à la méthode du burst:
  // comme on a fait une région par pixel,
  // toutes les arêtes ont un plongement d'un seul linel
  FKhalimsky->setLCell(getDoublet(ADart),false);

  // Extinction des pointels
  if( beta0(ADart)==beta2(ADart) )
    FKhalimsky->setPCell(getDoublet(ADart),false);

  if( beta1(ADart)==beta2(ADart) )
    FKhalimsky->setPCell(getDoublet(beta1(ADart)),false);

  // La suppression effective.
  linkBeta1( beta20 (ADart), beta1  (ADart) );
  linkBeta1( beta0  (ADart), beta21 (ADart) );

  assert( findRegionRoot(getRegion(beta2(ADart)))->getRepresentativeDart() != beta2(ADart) );
  setMark(beta2(ADart), AMarkNumber);
  assert( findRegionRoot(getRegion(ADart))->getRepresentativeDart() != ADart );
  setMark(ADart, AMarkNumber);

  //std::cout<<"[end] CTile::mergeEdgeRemoval"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::extractVertexRemoval( CDart* ADart )
{
  CPyramidalDart* dart = static_cast<CPyramidalDart*>(ADart);
  CPyramidalDart* tempDart  = static_cast<CPyramidalDart*>(beta21(ADart));

  // On s'assure de conserver les up/down
  if(dart->existUp())
    {
      CPyramidalDart* upDart = dart->up();
      upDart->setDown( dart->getBeta0() );
    }

  if(tempDart->existUp())
    {
      CPyramidalDart* upDart = tempDart->up();
      upDart->setDown(tempDart->getBeta0());
    }

  vertexRemoval( ADart );
}
