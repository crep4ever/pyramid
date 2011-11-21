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
#include "traversal-region-pixels.hh"
#include <pngwriter.h>
#include INCLUDE_NON_INLINE("tile.icc")

#define UNUSED(x) (void)x

using namespace Map2d;
//******************************************************************************

void CTile::createTopTile()
{
  createSingleRegion( width(), height() );
  splitSingleRegion ( width(), height() );
    
  CRegion* infRegion = getInclusionTreeRoot();
  CRegion* region = infRegion->getFirstSon();
  
  FMapRegions[infRegion->getId()] = infRegion;
  FMapRegions[region->getId()] = region;
  
  CPyramidalDart* inf  = static_cast<CPyramidalDart*>(infRegion->getRepresentativeDart());
  CPyramidalDart* dart = static_cast<CPyramidalDart*>(region->getRepresentativeDart());
  
  FMapDarts[inf->getId()]  = inf;
  FMapDarts[dart->getId()] = dart;

  //loadImage();
  traversePixelsSingleRegion();
}
//------------------------------------------------------------------------------
void CTile::createSingleRegion(uint AWidth, uint AHeight)
{
  CPyramidalDart* dart = NULL;
  CPyramidalDart* inf  = NULL;
  CDoublet doublet;

  FKhalimsky = new CKhalimsky(AWidth, AHeight);

  CPyramidalRegion* infRegion = addMapInfiniteRegion();
  setInclusionTreeRoot(infRegion);
  CPyramidalRegion* region  = addMapRegion();

  doublet.setDoublet(0,0,YPOS);
  inf = static_cast<CPyramidalDart*>(addMapDart(doublet, infRegion));
  setRegion(inf, infRegion);

  doublet.setDoublet(0,0,XPOS);
  dart = static_cast<CPyramidalDart*>(addMapDart(doublet, region));
  setRegion(dart, region);

  // Plongement de la région
  getKhalimsky()->setPCell(doublet, true);

  for(uint x=0; x<AWidth; ++x)
    {
      doublet.setDoublet(x,0,XPOS);
      getKhalimsky()->setLCell(doublet, true);
      doublet.setDoublet(x, AHeight, XPOS);
      getKhalimsky()->setLCell(doublet, true);
    }
  
  for(uint y=0; y<AHeight; ++y)
    {
      doublet.setDoublet(0,y,YPOS);
      getKhalimsky()->setLCell(doublet, true);
      doublet.setDoublet(AWidth, y, YPOS);
      getKhalimsky()->setLCell(doublet, true);
    }

  //Coutures des brins
  linkBeta0(dart, dart); //linkBeta1(dart, dart);
  linkBeta0(inf, inf);   //linkBeta1(inf, inf);
  linkBeta2(dart, inf);

  //todo: déporter ça vers le constructeur par défaut ?
  // Initialisation des régions
  infRegion->setRepresentativeDart(inf);
  infRegion->setNextSameCC(infRegion); //on boucle sur soi par défaut
  infRegion->setFirstSon(region);      //élément suivant
  infRegion->setBrother(region);       //élément précédent

  region->setRepresentativeDart(dart);
  region->setNextSameCC(region);
  region->setFirstSon(NULL);
  region->setBrother(infRegion);
  region->setFirstPixel(CPoint2D(0,0));

  // Calcul de l'arbre d'inclusion des régions
  relabelDarts();
  computeInclusionTree();
}
//------------------------------------------------------------------------------
void CTile::splitSingleRegion(uint AWidth, uint AHeight)
{
  CDart* dart = getInclusionTreeRoot()->getFirstSon()->getRepresentativeDart();

  CDoublet doublet(AWidth,0,YPOS);
  dart = insertVertexOnEdge(dart, doublet);

  doublet.setDoublet(AWidth, AHeight, XNEG);
  dart = insertVertexOnEdge(dart, doublet);

  doublet.setDoublet(0, AHeight, YNEG);
  dart = insertVertexOnEdge(dart, doublet);
}
//------------------------------------------------------------------------------
void CTile::traversePixelsSingleRegion()
{
  assert(getNbRegions()==2);
  FPixIndex=0;
  image()->read(boundingBox(), 0);
  uint8* data = image()->getData(boundingBox(), index(2));
  CRegion* region = getInclusionTreeRoot()->getFirstSon();

  if(image()->colorMode()==CImageTiff::Grey)
    {
      for(uint i = 0; i<width()*height(); ++i)
	region->addPixel( data[i] );
    }
  else if(image()->colorMode()==CImageTiff::RGB)
    {
      for(uint i = 0; i<width()*height(); i = i+3)
	region->addPixel( (data[i]+data[i+1]+data[i+2])/3 );
    }
  else
    std::cout<<"Error CTile::traversePixels image encoding not supported"<<std::endl;

  delete [] data;
}
//------------------------------------------------------------------------------
void CTile::traversePixels()
{
  image()->read(boundingBox(), index(2));
  IM_Pixel pix;
  for( CDynamicCoverageAllRegions it( this ); it.cont();++it )
    for( CTraversalRegionPixels it2(this, static_cast<CPyramidalRegion*>(*it)); it2.cont(); ++it2)
      {
	pix.x = (*it2).getX()+xmin(); pix.y = (*it2).getY()+ymin();
	addPixel(pix, *it);
      }
}
//------------------------------------------------------------------------------
int CTile::unionRegionRoot(CRegion* ARegion1, CRegion* ARegion2)
{
  // l'union de deux régions revient à unir leurs racines
  ARegion1=findRegionRoot(ARegion1);
  ARegion2=findRegionRoot(ARegion2);

  //si les deux régions racines sont égales on sort de la fonction
  if(ARegion1==ARegion2)
    return 0;
  
  // sinon on les fusionne en mettant la région d'identifiant le + petit
  // racine de l'arbre (cad la première pour notre ordre de parcours)
  CPyramidalRegion* region1 = static_cast<CPyramidalRegion*>(ARegion1);
  CPyramidalRegion* region2 = static_cast<CPyramidalRegion*>(ARegion2);
  if ( region1->getFirstPixel() < region2->getFirstPixel() )
    {
      region2->setNextSameCC(region1);
      region2->setRepresentativeDart( NULL );
      region1->mergeWith( region2 );//maj des paramètres des régions
    }
  else
    {
      region1->setNextSameCC(region2);
      region1->setRepresentativeDart( NULL );
      region2->mergeWith( region1 );//maj des paramètres des régions      
    }
  
  assert(ARegion1->getRepresentativeDart()!=NULL || 
	 ARegion2->getRepresentativeDart() != NULL);
 
  return 1;
}
//------------------------------------------------------------------------------
void CTile::simplifyMap()
{
  //std::cout<<"[start] CTile::simplifyTile"<<std::endl;   

  // TODO: voir si on peut supprimer la marque
  // à première vue, ça pose pas de problèmes mais en fait,
  // elle évite de re-supprimer des brins lors du vertexRemoval

  CTopologicalDart* dart = NULL;
  int toDelete = getNewMark();

  // On fusionne les arêtes reliées par des sommets de degré 2
  // et qui n'appartiennent pas au bord de la tuile

  for( CDynamicCoverageAll it( this ); it.cont(); ++it )
    {
      dart = static_cast<CTopologicalDart*>(*it);
      if(!isDartOnBorder(dart,1) && !isMarked(dart, toDelete) && beta21(dart) == beta02(dart))
	{
	  if( !isEdgeLoop(dart) )
	    vertexRemoval(dart, toDelete);
	  else
	    getKhalimsky()->setPCell(dart->doublet(), false);
	}
    }
  
  //On supprime tous les brins qui ont été marqués lors du vertex removal
  CDart* actu = NULL;
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
    
  //std::cout<<"[end] CTile::simplifyTile"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::deleteRegionList(std::deque<CRegion*>& AList)
{
  CRegion* currentRegion = NULL;
  std::deque<CRegion*>::iterator it;
  for(it=AList.begin(); it != AList.end(); )
    {  
      currentRegion = *it;
      ++it;
      //Suppression de la région
      currentRegion->setFirstSon(NULL);  
      currentRegion->setBrother(NULL);
      currentRegion->setNextSameCC(NULL);
      delRegion(currentRegion);
    }
}
//------------------------------------------------------------------------------
void CTile::relabelDarts()
{
  // 1. On parcours les brins de la carte pour effectuer une mise à
  //    jour de leur étiquettage
  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      // Si la région d'appartenance du brin pointe sur elle même,
      // il n'y a aucune mise à jour à faire.
      if (getRegion(*it)->getNextSameCC() != getRegion(*it))
	{
	  assert(getRegion(*it)->getNextSameCC() != NULL);
	  // dans les cartes tuilées, on utilise le first pixel pour la comparaison des régions
	  //assert(findRegionRoot(getRegion(*it))->getId() <= getRegion(*it)->getId());
	 
	  //Conservation des up/down
	  CPyramidalRegion* region = getRegion(*it);
	  if( region->existRegionUp() && region->getRegionUp()->getRegionDown()==region)
	    region->getRegionUp()->setRegionDown(static_cast<CPyramidalRegion*>(findRegionRoot(region)));

	  //on récupère la région avec laquelle il va falloir étiquetter
	  //la région courante
	  setRegion(*it, findRegionRoot(getRegion(*it)));
	}
    }

  // 2. On parcours maintenant les régions à partir de la première région
  //    contenue dans le fils de FInclusioTreeRoot.
  //    On supprime toutes celles qui sont inutiles et on met à jour
  //    la liste chainée des régions

  //vaiable de parcours de la chaine
  FInclusionTreeRoot->setNextSameCC(NULL);
  CRegion* currentRegion = FInclusionTreeRoot->getFirstSon();
  while (currentRegion != NULL)
    {
      assert(currentRegion->getNextSameCC()!=NULL);
      
      //si la région ne pointe pas sur elle même elle doit être supprimée
      //car elle ne correspond pas à une racine d'un arbre
      if (currentRegion->getNextSameCC() != currentRegion)
	{
	  CRegion* TempRegion = currentRegion; //contient la région à supprimer

	  //normalement la région infinie n'est pas parcourue
	  assert(TempRegion != FInclusionTreeRoot);
	  // et il y a au moins une région avant la région courante
	  // (au pire la région infinie).
	  assert(TempRegion->getBrother() != NULL);

	  //on met à jour la liste chainée des régions

	  // prev -> next
	  TempRegion->getBrother()->setFirstSon(TempRegion->getFirstSon());

	  //si on est pas en bout de chaine
	  if (TempRegion->getFirstSon() != NULL)
	    {
	      //prev <- next
	      TempRegion->getFirstSon()->setBrother(TempRegion->getBrother());
	    }

	  //on conserve la région suivante dans la variable currentRegion
	  currentRegion = TempRegion->getFirstSon();

	  //on détruit la région courante par appel au destructeur.
	  //Avant de la détruire on remet les champs de la région à NULL
	  //afin de ne pas supprimer l'arbre union
	  TempRegion->setFirstSon(NULL);
	  TempRegion->setBrother(NULL);
	  TempRegion->setNextSameCC(NULL);

	  assert( !static_cast<CPyramidalRegion*>(TempRegion)->existRegionUp() ||
		  !(static_cast<CPyramidalRegion*>(TempRegion)->getRegionUp()->getRegionDown()==TempRegion));
	 
	  delete TempRegion;
	  //on décrémente le nombre de régions de la carte
	  --FNbRegions;
	}
      //sinon on passe directement à la région suivante
      else
	{
	  currentRegion->setNextSameCC(NULL);
	  currentRegion = currentRegion->getFirstSon();
	}
    }
}
//------------------------------------------------------------------------------
void CTile::delRegion(CRegion* ARegion)
{
  assert( ARegion!=NULL );
  assert( !static_cast<CPyramidalRegion*>(ARegion)->existRegionUp() ||
	  !(static_cast<CPyramidalRegion*>(ARegion)->getRegionUp()->getRegionDown()==ARegion));
    
  setNbRegions(getNbRegions()-1);
    
  if( ARegion->existSon() )
    delRegion( ARegion->getFirstSon() );
    
  if( ARegion->existBrother() )
    delRegion( ARegion->getBrother() );
    
  if( ARegion->existNextSameCC() ) 
    delRegion( ARegion->getNextSameCC() );
    
  delete ARegion;
}
//------------------------------------------------------------------------------
void CTile::getRegionBoundingBox(CDart* ADart, std::vector<uint>& tab)
{ 
  CDoublet tmp; 
  tab.push_back(getDoublet(ADart).getX()); //xmin
  tab.push_back(getDoublet(ADart).getY()); //ymin
  tab.push_back(0);                      //xmax
  tab.push_back(0);                      //ymax
    
  // On récupère les dimensions de la région
  for( CDynamicCoverage1 it( this, ADart ) ; it.cont() ; ++it )
    {
      tmp = getDoublet(*it);
      uint x = tmp.getX();
      uint y = tmp.getY();
	
      if( x > tab[2] ) 
	tab[2] = x;
      else if( x < tab[0] ) 
	tab[0] = x;
	
      if( y > tab[3] ) 
	tab[3] = y;
      else if( y < tab[1] ) 
	tab[1] = y;
    }
}

//------------------------------------------------------------------------------
void CTile::exportRegionToPng(CPyramidalRegion* ARegion, const std::string & ADirname)
{
  //std::cout<<" [start] CTile::exportRegionToPng "<<std::endl;
  ostringstream stream;
  stream << "mkdir -p "<<ADirname<<"/";
  UNUSED(system(stream.str().c_str()));
  
  ostringstream oss;
  oss << ADirname << "/region_" 
      << index(0) << "-"<< index(1) << "-" << index(2) 
      << "_" << FCount++ << ".png";
  
  std::vector<uint> tab;
  getRegionBoundingBox(ARegion->getRepresentativeDart(), tab);
  uint width =  tab[2] - tab[0] + 1;
  uint height = tab[3] - tab[1] + 1;
  
  IM_Pixel pix;
  pngwriter png(width, height, 0, oss.str().c_str());
  for(uint j = 0 ; j < height; ++j)
    {
      pix.y=j + ymin();
      for(uint i = 0; i < width; ++i)
	{
	  pix.x = i + xmin();
	  image()->getPixel(pix, depth());
	  png.plot(i, height-j-1, 
		   pix.value[0]/255.0, pix.value[1]/255.0, pix.value[2]/255.0);
	}
    }
  png.close();
  //std::cout<<" [end] CTile::exportRegionToPng "<<std::endl;
}

//------------------------------------------------------------------------------
void CTile::saveMitosis()
{
  //std::cout<<" [start] CTile::saveMitosis "<<std::endl;
  //image()->regularization(boundingBox(), depth(), 10, 2, 0.1, 1);
  FCount = 0;
  for( CDynamicCoverageAllRegions it( this ); it.cont(); ++it )
    {
      CPyramidalRegion* region = static_cast<CPyramidalRegion*>(*it);
      if(region->label()==CPyramidalRegion::Mitosis)
	{
	  int sum=0, count=0;
	  IM_Pixel pix;
	  for( CTraversalRegionPixels it2(this, region); it2.cont(); ++it2)
	    {	
	      pix.x = (*it2).x() + xmin();
	      pix.y = (*it2).y() + ymin();
	      image()->getPixel( pix, depth() );
	      sum += 2*pix.value[0] - pix.value[1] - pix.value[2];
	      ++count;
	    }
	  std::vector<uint> tab;
	  getRegionBoundingBox(region->getRepresentativeDart(), tab);
	  uint width =  tab[2] - tab[0] + 1;
	  uint height = tab[3] - tab[1] + 1;
	  assert(count>0 && width > 0 && height > 0);

	  if( width < 3 || height < 3 || (sum/count) < 35 )
	    {
	      //std::cout << "remove mitosis" << std::endl;
	      region->setLabel(CPyramidalRegion::InSitu);
	    }
	  else
	    exportRegionToPng(region, "output/mitosis");
	}
    }
  //std::cout<<" [end] CTile::saveMitosis "<<std::endl;
}
