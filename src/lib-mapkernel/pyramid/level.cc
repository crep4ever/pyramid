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
#include "level.hh"
#include "pyramidal-region.hh"
#include "pyramidal-dart.hh"
#include "tile.hh"
#include "chrono.hh"
#include <cassert>
#include INCLUDE_NON_INLINE("level.icc")
using namespace Map2d;
//******************************************************************************
CLevel::~CLevel()
{ 
  //std::cout<<"[destructeur] CLevel \n";
  unloadAllTiles();
  FHisto=NULL;
}

//******************************************************************************
// Construction
//******************************************************************************


void CLevel::createTopLevel()
{
  std::cout<<"[start] CLevel::createTopLevel"<<std::endl;
  assert(depth()==0);
  
  FTileCounter  = nbTilesWidth() * nbTilesHeight();
  uint width  = imageWidth();
  uint height = imageHeight();

  uint tileWidth=FTileWidth;
  uint tileHeight=FTileHeight;

  //  #pragma omp parallel for
  for(uint i=0; i<FTileCounter; ++i)
    {
      assert(FTileWidth!=0 && FTileHeight!=0);
      uint l,c,x,y;
      l = i / nbTilesWidth();
      c = i % nbTilesWidth();
      x = (c+1) * FTileWidth;
      y = (l+1) * FTileHeight;

      if(x>width)
	{
	  x = width;
	  tileWidth = width % FTileWidth;
	}

      if(y>height)
	{
	  y = height;
	  tileHeight = height % FTileHeight;
	}
      CTile* tile = new CTile(tileWidth, tileHeight);      
      tile->setBottomRight(CPoint2D(x, y));
      tile->setId(i+1);
      tile->setIndex( 0, xmin(tile)/FTileWidth  );
      tile->setIndex( 1, ymin(tile)/FTileHeight );
      tile->setIndex( 2, 0 );
      tile->setImage(FImage);
      tile->setMergeThreshold(FMergeThreshold);
      tile->setPixelMark();
      tile->createTopTile();
      tile->write();
      delete tile;     
    }
  //std::cout<<"[end] CLevel::createTopLevel"<<std::endl;
}

//------------------------------------------------------------------------------
void CLevel::extract(const ExtractMode & AExtractMode, 
		     const SegmentationMode & ASegmentationMode,
		     const ProjectionMode & AProjectionMode,
		     const FocusAttentionMode & AFocusAttentionMode,
		     const DetectFictiveBordersMode & ADetectFictiveBordersMode )
{
  assert(levelUp()!=NULL);
  assert(this!=NULL && FDepth>0); // il faut qu'il y ait au moins un niveau de fait
  std::cout<<"[start] CLevel::extract level "<< FDepth<< "\n";

  // Parcours des tuiles du niveau précédent
  uint nbWidth  = levelUp()->nbTilesWidth();
  uint nbHeight = levelUp()->nbTilesHeight();
  uint memory = 0; FMemory = 0;
  FTileCounter += levelUp()->FTileCounter;

  CChrono levelChrono;
  CChrono segmentationChrono;
  levelChrono.start();
  segmentationChrono.start();
  preprocessing(ASegmentationMode);
  segmentationChrono.stop();
  for(uint j=0; j<nbHeight; ++j)
    for(uint i=0; i<nbWidth; ++i)
      {
	CPoint2D pos(i,j);
	levelUp()->loadTile(pos); // Load tile up
	switch(AExtractMode)
	  {
	  case ConstantTileNumber:
	    memory = burstAndMergeTile(pos, ASegmentationMode, AProjectionMode, 
				       AFocusAttentionMode, ADetectFictiveBordersMode);
	    break;	  
	  case ConstantTileSize:
	    memory = extractTile(pos, ASegmentationMode, AProjectionMode, 
				 AFocusAttentionMode, ADetectFictiveBordersMode, 
				 segmentationChrono);
	    break;
	  default:
	    std::cout<<"CLevel::extract error extractMode not supported"<<std::endl;
	    assert(false);
	  }
	FMemory = (FMemory <= memory)? memory : FMemory;
	levelUp()->unloadTile(pos); // Swap tile up
      }
  unload();      
  if(FClassif) delete FClassif;
  if(FAssignment) delete FAssignment;
  FClassif=NULL;
  FHisto=NULL;
  FAssignment=NULL;
  segmentationChrono.display("segmentation du niveau");
  levelChrono.stop();
  levelChrono.display("extraction du niveau");
  print();

  FILE* fd = fopen("./qscripts/raffinement.dat","a");
  std::ostringstream res;
  res << "\n" << nbHeight*nbWidth << "  " << levelChrono.realTime() << "  " << FMemory/1000000.0 << "\n";
  fputs(res.str().c_str(),fd);
  fclose(fd);
  std::cout<<"[end] CPyramid::extract level \n";
}

//------------------------------------------------------------------------------
uint  CLevel::burstAndMergeTile(const CPoint2D & APos, 
				const SegmentationMode & ASegmentationMode,
				const ProjectionMode & AProjectionMode,
				const FocusAttentionMode & AFocusAttentionMode,
				const DetectFictiveBordersMode & ADetectFictiveBordersMode )
{
  //std::cout<<"[start] CLevel::burstAndMergeTile "<<APos<<" for level "<<FDepth<<"\n";
  uint memory = 0;
  uint i = APos.getX(); uint j = APos.getY();
  if(i>0) memory += loadTile(CPoint2D(i-1, j))->memory(); // Load tile left
  if(j>0) memory += loadTile(CPoint2D(i, j-1))->memory(); // Load tile top
  memory+=levelUp()->tile(APos)->memory();
  
  CTile* parent = levelUp()->tile(APos);
  parent->FAssignment = FAssignment;
  parent->FClassif = FClassif;
  
  CTile* newTile = parent->createChildByCopy(AProjectionMode);
  newTile->setId(++FTileCounter);
  newTile->setMergeThreshold(FMergeThreshold);

  // On éclate le bord de la tuile de façon à récupérer les coins et les beta2
  newTile->splitBorder();
  retrieveTileCorners(newTile);
  retrieveTileBeta2(newTile, ASegmentationMode, ADetectFictiveBordersMode); 
  
  //classif
  IM_Box box;
  box.XTop = xmin(newTile);  box.YTop = ymin(newTile);
  box.XBtm = xmax(newTile);  box.YBtm = ymax(newTile);
  newTile->FClassif = image()->kmeans(box, FDepth, 3);
  
  // Split de la tuile par burst/merge
  newTile->burstAndMerge(AFocusAttentionMode, ASegmentationMode);
  memory += newTile->memory();
  delete [] newTile->FClassif;
 
  // Simplification des sommets de degré 2
  newTile->simplifyMap();
  simplifyTileBorder(newTile);
 
  // Reconstruction de l'arbre d'inclusion des régions
  newTile->computeInclusionTree();

  levelUp()->writeTile(APos); // save tile up
  addTile(newTile);
  writeTile(newTile);

  if(i>0)	// Swap left tile
    {
      writeTile(  CPoint2D(i-1,j) );
      unloadTile( CPoint2D(i-1,j) );
    }
  if(j>0)	// Swap top tile
    {
      writeTile(  CPoint2D(i,j-1) );
      unloadTile( CPoint2D(i,j-1) );
    }
  unloadTile( CPoint2D(i,j) );   // Swap tile down
  return memory;
  //std::cout<<"[end] CLevel::burstAndMergeTile"<<std::endl;
}

//------------------------------------------------------------------------------
uint CLevel::extractTile(const CPoint2D & APos,
			 const SegmentationMode & ASegmentationMode,
			 const ProjectionMode & AProjectionMode,
			 const FocusAttentionMode & AFocusAttentionMode,
			 const DetectFictiveBordersMode & ADetectFictiveBordersMode,
			 CChrono & AChrono)
{
  //std::cout<<"\r [start] CLevel::extractTile "<<APos<<" for level "<<FDepth << "\n";
  //std::cout<<"this depth = "<<depth()<<" up depth() = "<<ALevelUp->depth()<<" \n";
  CTile* parent = levelUp()->tile(APos);
  parent->FAssignment = FAssignment;
  parent->FClassif = FClassif;
  std::deque<CTile*> children; 
  
  parent->extractChildren(children, ASegmentationMode, AProjectionMode, AFocusAttentionMode, AChrono);

  uint memory = 0;
  std::deque<CTile*> process = children;
  for(uint k=0; k<children.size(); ++k)
    {
      uint childrenMemory = 0;
      CTile* tile = children[k];
      assert(tile->isOk());
      tile->setId(++FTileCounter);
      uint i = tile->index(0);  uint j = tile->index(1);
      if(i>0) childrenMemory += loadTile( CPoint2D(i-1,j) )->memory(); // Load tile left
      if(j>0) childrenMemory += loadTile( CPoint2D(i,j-1) )->memory(); // Load tile top
      
      //pas besoin de splitter le bord car on ne supprime pas les sommets de
      //degré 2 du bord pendant l'extraction
      retrieveTileCorners(tile);
      retrieveTileBeta2(tile, ASegmentationMode, ADetectFictiveBordersMode);
      childrenMemory += tile->memory();
      tile->simplifyMap();
      simplifyTileBorder(tile);
      memory= (memory<childrenMemory)? childrenMemory : memory;

      levelUp()->writeTile(APos);
      addTile(tile);
      writeTile(tile);
      unloadTile(CPoint2D(i,j));
      if(i>0)	// Swap left tile
	{
	  writeTile(  CPoint2D(i-1,j) );
	  unloadTile( CPoint2D(i-1,j) );
	}
      if(j>0)	// Swap top tile
	{
	  writeTile(  CPoint2D(i,j-1) );
	  unloadTile( CPoint2D(i,j-1) );
	}
    }
  return memory + parent->memory();
  //std::cout<<"[end] CLevel::extractTile"<<std::endl;
}

//------------------------------------------------------------------------------
CTile* CLevel::tileTop(CTile* ATile)
{
  assert(ATile!=NULL);
  CTile* current = NULL;

  if( ymin(ATile) == 0 )
    return NULL; // Pas de tuile up
    
  for(it=FTiles.begin(); it!=FTiles.end(); ++it)
    {
      current = *it;
      if(xmin(current) == xmin(ATile) && ymax(current) == ymin(ATile))
	return current;
    }
  return NULL;
}
//------------------------------------------------------------------------------
CTile* CLevel::tileLeft(CTile* ATile)
{
  assert(ATile!=NULL);
  CTile* current = NULL;

  if( xmin(ATile) == 0 )
    return NULL; // Pas de tuile left
    
  for(it=FTiles.begin(); it!=FTiles.end(); ++it)
    {
      current = *it;
      if(ymin(current) == ymin(ATile) && xmax(current) == xmin(ATile))
	return current;
    }
  return NULL;
}
//------------------------------------------------------------------------------
void CLevel::delTile(CTile* ATile)
{
  //on s'assure de bien supprimer une tuile appartenant à ce niveau
  assert(ATile->index(2)==FDepth);
  //std::cout<<" [start] CLevel::delTile ("<<ATile->index(0)<<","<<ATile->index(1)<<","<<ATile->index(2)<<")"<<std::endl;
    
  // les brins et régions des tuiles up et down ne peuvent plus pointer sur this
  if(ATile->existTileUp())
    {
      CTile* up = ATile->tileUp();
      //il faut parcourir uniquement les brins/régions de up qui sont plongés dans this
      for( CDynamicCoverageAllRegions it( ATile ); it.cont(); ++it )
	{
	  CPyramidalRegion* upRegion = static_cast<CPyramidalRegion*>(*it)->getRegionUp();
	  if(upRegion!=NULL  && upRegion->getRegionDown()==(*it))
	    upRegion->setRegionDown(NULL);
	}

      for( CDynamicCoverageAll it( ATile ); it.cont(); ++it )
	{
	  CPyramidalDart* upDart = static_cast<CPyramidalDart*>(*it)->getDartUp();
	  if(upDart!=NULL && upDart->getDartDown()==(*it))
	    upDart->setDartDown(NULL);
	}
      //puisqu'on est en top-down, il n'y a qu'un seul parent
      up->setTileDown(NULL);
    }
    
  if(ATile->existTileDown())
    {
      //Il faut que toutes les tuiles filles qui avaient pour parent ATile
      assert(levelDown());
      std::deque<CTile*> copy = levelDown()->tiles();
      for(uint i=0; i<copy.size(); ++i)
	if( copy[i]->tileUp()==ATile )
	  {
	    //Régions
	    for( CDynamicCoverageAllRegions it2( *it ); it2.cont(); ++it2 )
	      static_cast<CPyramidalRegion*>(*it2)->setRegionUp(NULL);
	    //Brins
	    for( CDynamicCoverageAll it2( *it ); it2.cont(); ++it2 )
	      static_cast<CPyramidalDart*>(*it2)->setDartUp(NULL);
	    //Tuile
	    copy[i]->setTileUp(NULL);
	  }
    }
  delete ATile;
}
  


//******************************************************************************
// Traitements sur le bord des tuiles
//******************************************************************************


void CLevel::simplifyTileTopBorder(CTile* ATile)
{
  CTile* currentTile = ATile;
  CDart* rep = currentTile->getInclusionTreeRoot()->getRepresentativeDart();

  if( existTileTop(currentTile) )
    {
      //std::cout<<"simplify Top Border"<<std::endl;
      CTile* topTile  = tileTop(ATile);
      CDart* topRep = topTile->getInclusionTreeRoot()->getRepresentativeDart();
      
      // On se positionne comme il faut
      CDart* up  = topRep;
      CDart* down = beta0(rep);
      
      while(topTile->getDoublet(up).getLinel()!=XPOS)
	up = beta1(up);

      assert(topTile->getDoublet( up ).getLinel() == XPOS);
      assert(currentTile->getDoublet(down).getLinel() == XNEG);
      
      // On avance d'un cran
      up = beta1(up); 
      
      // Tant qu'on n'arrive pas au bout du bord,
      // si les deux pointels sont de degré 2, on les supprime
      // sinon, on avance ensemble
      while( topTile->getDoublet(up).getLinel() == XPOS )
	{
	  up = beta1(up);
	  down = beta0(down);
	  
	  if( currentTile->isDegreeTwoPointel(currentTile->getDoublet(beta1(down))) &&
	      topTile->isDegreeTwoPointel(topTile->getDoublet(beta0(up))) )
	    {
	      currentTile->vertexRemoval(beta1(down));
	      topTile->vertexRemoval(beta0(up));
	    }
	}
    }
  else
    {
      CDart* down = beta0(rep);
      while( currentTile->getDoublet(down->getBeta0()).getLinel() == XNEG )
	{
	  down = beta0(down);
	  if( currentTile->isDegreeTwoPointel(currentTile->getDoublet(beta1(down))) )
	    currentTile->vertexRemoval(beta1(down));
	}
    }
}

//------------------------------------------------------------------------------
void CLevel::simplifyTileLeftBorder(CTile* ATile)
{
  CTile* currentTile = ATile;
  CDart* rep = currentTile->getInclusionTreeRoot()->getRepresentativeDart();

  if( existTileLeft(currentTile) )
    {
      //std::cout<<"simplify Left Border"<<std::endl;
      CTile* leftTile  = tileLeft(ATile);
      CDart* leftRep  = leftTile->getInclusionTreeRoot()->getRepresentativeDart();

      // On se positionne comme il faut
      CDart* left  = leftRep;
      CDart* right = rep;
      
      while(leftTile->getDoublet(left).getLinel()!=YNEG)
	{
	  assert(beta0(left)!=left);
	  left = beta0(left);
	}
      
      assert(leftTile->getDoublet( left ).getLinel() == YNEG);
      assert(currentTile->getDoublet(right).getLinel() == YPOS);
      
      // On avance d'un cran
      right = beta1(right); 

      // Tant qu'on n'arrive pas au bout du bord,
      // si les deux pointels sont de degré 2, on les supprime
      // sinon, on avance en parrallèle
      while( currentTile->getDoublet(right).getLinel() == YPOS )
	{
	  right = beta1(right);
	  left = beta0(left);
	  
	  if( currentTile->isDegreeTwoPointel(currentTile->getDoublet(beta0(right))) &&
	      leftTile->isDegreeTwoPointel(leftTile->getDoublet(beta1(left))) )
	    {
	      leftTile->vertexRemoval(beta1(left));
	      currentTile->vertexRemoval(beta0(right));
	    }
	}
    }
  else
    {
      CDart* dart = beta1(rep);
      while( currentTile->getDoublet(dart).getLinel() == YPOS )
	{
	  dart = beta1(dart);
	  if( currentTile->isDegreeTwoPointel(currentTile->getDoublet(beta0(dart))) )
	    currentTile->vertexRemoval(beta0(dart));
	}
    }
}

//------------------------------------------------------------------------------
void CLevel::simplifyTileBorder(CTile* ATile)
{
  //std::cout<<"[start] CLevel::simplifyTileBorder"<<std::endl;   
  CTile* currentTile = ATile;
  CDart* rep = currentTile->getInclusionTreeRoot()->getRepresentativeDart();

  // Traitement de la tuile de dessus (topTile)
  simplifyTileTopBorder(currentTile);

  // Traitement de la tuile de gauche (leftTile)
  simplifyTileLeftBorder(currentTile);
  
  // Simplification des bords bas et droite de l'image
  if( xmax(currentTile) == imageWidth() )
    {
      CDart* dart = rep;
      
      while(currentTile->getDoublet(dart).getLinel()!=YNEG)
	dart = beta0(dart);

      while( currentTile->getDoublet(beta0(dart)).getLinel() == YNEG )
	{
	  dart = beta0(dart);
	  if( currentTile->isDegreeTwoPointel(currentTile->getDoublet(beta1(dart))) )
	    currentTile->vertexRemoval(beta1(dart));
	}
    }
  
  if( ymax(currentTile) == imageHeight() )
    {
      CDart* dart = rep;
      
      while(currentTile->getDoublet(dart).getLinel()!=XPOS)
	dart = beta1(dart);
      
      dart = beta1(dart);
      
      while( currentTile->getDoublet(dart).getLinel() == XPOS )
	{
	  dart = beta1(dart);
	  if( currentTile->isDegreeTwoPointel(currentTile->getDoublet(beta0(dart))) )
	    currentTile->vertexRemoval(beta0(dart));
	}
    }
  //std::cout<<"[end] CLevel::simplifyTileBorder"<<std::endl;   
}

//------------------------------------------------------------------------------
void CLevel::retrieveTileCorners(CTile* ATile) const
{
  ATile->FCorners.clear();
  //std::cout<<"[start] CLevel::retrieveCorners"<<std::endl;
  CDart* left  = NULL;
  CDart* up    = NULL;
  CDart* right = NULL;
  CDart* down  = NULL;
  
  uint x = ATile->width();
  uint y = ATile->height();
 
  assert(ATile->getInclusionTreeRoot()!=NULL);
  CDart* current = ATile->getInclusionTreeRoot()->getRepresentativeDart();
  
  CDoublet dLeft  (0,0,YPOS); //doublet de référence
  CDoublet dUp    (x,0,XNEG); //doublet de départ
  CDoublet dRight (x,0,XNEG); //un cran après celui qu'on cherche
  CDoublet dDown  (0,y,XPOS); //doublet de référence
  
  left = current;
  assert(left!=NULL);
  assert( ATile->getDoublet(left) == dLeft );
  
  up = ATile->beta0(left);
  assert(up!=NULL);
  current = ATile->beta1(current);    
  
  for(CDynamicCoverage1 it(ATile, current); it.cont(); ++it)
    if( ATile->getDoublet(*it) == dDown )
      {
	down = *it;
	break;
      }
  assert(down!=NULL);

  for(CDynamicCoverage1 it(ATile, current); it.cont(); ++it)
    {
      if( ATile->getDoublet(*it) == dRight )
	{
	  right = (*it)->getBeta0();
	  break;
	}
    }
  assert(right!=NULL);
  
  ATile->addCorner(left);
  ATile->addCorner(up);
  ATile->addCorner(right);
  ATile->addCorner(down);
  //ATile->printCorners();
  //std::cout<<"[end] CLevel::retrieveCorners"<<std::endl;
}

//------------------------------------------------------------------------------
void CLevel::retrieveTileBeta2(CTile* ATile,
			       const SegmentationMode & ASegmentationMode,
			       const DetectFictiveBordersMode & ADetectFictiveBordersMode) 
{
  //std::cout<<"*** [start] CLevel::retrieveBeta2 ***"<<std::endl;
  //std::cout<<"detect fictive borders = "<<ADetectFictiveBordersMode<<std::endl;
  assert(ATile!=NULL);
  ATile->FMapBeta2.clear();
  assert(ATile->FMapBeta2.empty());

  CTile* currentTile = ATile;
  CTile* leftTile = NULL;
  CTile* topTile = NULL;

  // Traitement de la tuile de gauche
  if( existTileLeft(currentTile) )
    {
      leftTile = tileLeft(currentTile);
      
      CDart* left  = NULL;
      CDart* right = NULL;
      CDart* nextleft  = NULL;
      CDart* nextright = NULL;
      
      // left est le brin de droite de leftTile
      // right est le brin de gauche de currentTile
      // left et right sont des brins de la région infinie
      retrieveTileCorners(leftTile);
      left  = leftTile->FCorners[2];
      right = currentTile->FCorners[0];
      
      while(leftTile->getDoublet(left).getLinel() == YNEG)
	{
	  nextleft  = beta0(left);
	  nextright = beta1(right);
	  ATile->FMapBeta2[static_cast<CPyramidalDart*>(beta2(right))->getId()] = 
	    static_cast<CPyramidalDart*>(left->getBeta2())->getId();

	  if(ADetectFictiveBordersMode == DetectionOn && depth()>0)	  
	    {
	      CPyramidalDart* dart1 = static_cast<CPyramidalDart*>(beta2(left));
	      CPyramidalDart* dart2 = static_cast<CPyramidalDart*>(beta2(right));
	      CPyramidalRegion* region1 = static_cast<CPyramidalRegion*>(dart1->getRegion());
	      CPyramidalRegion* region2 = static_cast<CPyramidalRegion*>(dart2->getRegion());
	      
	      if(region1->label() == region2->label())
		//if(currentTile->isRegionToMerge(region1, region2, ASegmentationMode))
		{
		  dart1->setFictive(true);
		  dart2->setFictive(true);
		}
	    }

	  left = nextleft;
	  right = nextright;
	}
    }

  // Traitement de la tuile du haut  
  if( existTileTop(currentTile) )
    {
      topTile = tileTop(currentTile);    
      
      CDart* up  = NULL;
      CDart* down = NULL;
      CDart* nextup  = NULL;
      CDart* nextdown = NULL;
      
      // down est le brin up de currentTile
      // up est le brin down de topTile
      // down et up sont des brins de la région infinie
      retrieveTileCorners(topTile);
      up   = topTile->FCorners[3];
      down = currentTile->FCorners[1];
      
      while(topTile->getDoublet(up).getLinel() == XPOS)
	{
	  nextup = up->getBeta1();
	  nextdown = down->getBeta0();
	  ATile->FMapBeta2[static_cast<CPyramidalDart*>(down->getBeta2())->getId()] =  
	    static_cast<CPyramidalDart*>(up->getBeta2())->getId();

	  if(ADetectFictiveBordersMode == DetectionOn && depth()>0)	  
	    {
	      CPyramidalDart* dart1 = static_cast<CPyramidalDart*>(beta2(up));
	      CPyramidalDart* dart2 = static_cast<CPyramidalDart*>(beta2(down));
	      CPyramidalRegion* region1 = static_cast<CPyramidalRegion*>(dart1->getRegion());
	      CPyramidalRegion* region2 = static_cast<CPyramidalRegion*>(dart2->getRegion());
		      
	      if(region1->label() == region2->label())
		//if(currentTile->isRegionToMerge(region1, region2, ASegmentationMode))
		{
		  dart1->setFictive(true);
		  dart2->setFictive(true);
		}
	      else
		{
		  dart1->setFictive(false);
		  dart2->setFictive(false);
		}
	    }

	  up = nextup;
	  down = nextdown;
	}
    }
  //std::cout<<"*** [end] CLevel::retrieveBeta2 ***"<<std::endl;
}

//******************************************************************************
// Opérations de contrôle et vérification
//******************************************************************************

uint CLevel::nbDarts()
{
  uint total = 0;
  CTile* current = NULL;
  for(uint i=0; i<nbTilesWidth(); ++i)
    for(uint j=0; j<nbTilesHeight(); ++j)
      {
      	current = tile(CPoint2D(i,j));
      	for(CDynamicCoverageAll it(current); it.cont(); ++it)
      	  ++total;
	
      	unloadTile(CPoint2D(i,j));
      }
  return total;
}

//------------------------------------------------------------------------------
uint CLevel::nbFictiveDarts()
{
  uint total = 0;
  CTile* current = NULL;
  for(uint i=0; i<nbTilesWidth(); ++i)
    for(uint j=0; j<nbTilesHeight(); ++j)
      {
      	current = tile(CPoint2D(i,j));
      	for(CDynamicCoverageAll it(current); it.cont(); ++it)
	  if(static_cast<CPyramidalDart*>(*it)->isFictive())
	    ++total;
	
      	unloadTile(CPoint2D(i,j));
      }
  return total;
}

//------------------------------------------------------------------------------
uint CLevel::nbRegions()
{
  uint total = 0;
  CTile* current = NULL;
  for(uint i=0; i<nbTilesWidth(); ++i)
    for(uint j=0; j<nbTilesHeight(); ++j)
      {
      	current = tile(CPoint2D(i,j));
      	for(CDynamicCoverageAllRegions it(current); it.cont(); ++it)
      	  ++total;
	
      	unloadTile(CPoint2D(i,j));
      }
  return total;
}

//------------------------------------------------------------------------------
void CLevel::preprocessing(const SegmentationMode & ASegmentationMode)
{
  //std::cout<<"[start] CLevel::preprocessing level "<<FDepth<<" \n";
  IM_Box box;
  box.XTop = 0;  box.YTop = 0;
  box.XBtm = imageWidth();  box.YBtm = imageHeight();
  //uint size = imageWidth() * imageHeight() * 3;
  uint size = imageWidth() * imageHeight();

  //  if(ASegmentationMode==SegmentationOff && depth()==1)
  //    {
  //      FClassif = image()->kmeans(box, depth(), 3);
  //      image()->moveClasses(FClassif, size);
  //      image()->mergeClasses(2,3, FClassif, size);
  //    }

  if(ASegmentationMode==Histology)
    {    
      switch( depth() )
	{
	case 1:
	  std::cout<<" CLevel::preprocessing level 1 \n"<<std::endl;
	  FClassif = image()->kmeans(box, depth(), 3);
      	  //image()->moveClasses(FClassif, size);
      	  image()->mergeClasses(1,0, FClassif, size);
	  break;
	  
	case 2:
	  std::cout<<" CLevel::preprocessing level 2 \n"<<std::endl;
	  histogram();
	  FAssignment = new CImg<char>;
	  FAssignment->assign(256,256,256,1);
	  image()->setDepth(depth());
	  image()->kmeansHistogram(FHisto, FAssignment);
	  break;
	  
	case 3:
	  std::cout<<" CLevel::preprocessing level 3 \n"<<std::endl;
	  histogram();
	  FAssignment = new CImg<char>;
	  FAssignment->assign(256,256,256,1);
	  image()->setDepth(depth());
	  image()->kmeansHistogram(FHisto, FAssignment);
	  break;
	  
	case 4:
	  std::cout<<" CLevel::preprocessing level 4 \n"<<std::endl;
	  histogram();
	  FAssignment = new CImg<char>;
	  FAssignment->assign(256,256,256,1);
	  image()->setDepth(depth());
	  image()->kmeansHistogram(FHisto, FAssignment);
	  break;
	  
	default:
	  break;
	}
    }
  std::cout<<"[end] CLevel::preprocessing \n";
}


//------------------------------------------------------------------------------
void CLevel::printHistogram()
{
  std::cout<<"[start] CLevel::printHistogram" << std::endl;
  std::cout<<" number of data points = " << FHisto->sum() << std::endl;
  cimg_forXYZ(*FHisto,r,g,b)
    {
      int nb = (*FHisto)(r,g,b);
      if(nb>0)
	std::cout<<"pixel (" << r << "," << g << "," << b << ") counted " << nb << " times" << std::endl;      
    }
  std::cout<<"[end] CLevel::printHistogram" << std::endl;
}

//------------------------------------------------------------------------------
void CLevel::print()
{
#ifdef DEBUG_PYRAMID
  int sys = 0;
  std::cout<<"mémoire ram max = " << FMemory << std::endl;
  std::cout<<"mémoire disque = " ;
  sys = system("du -h ./output"); 
  std::cout << std::endl;
  std::cout<<"nombre de brins = " << nbDarts() << std::endl;
  std::cout<<"nombre de brins fictifs = " << nbFictiveDarts() << std::endl;
  std::cout<<"nombre de régions = " << nbRegions() << std::endl;
#endif
}
