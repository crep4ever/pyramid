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
#include <algorithm>
#include "tile.hh"
#include "pyramidal-dart.hh"
#include "pyramidal-region.hh"
#include "coverage-region-pixels.hh"
#include "svg-export.hh"
using namespace Map2d;

//******************************************************************************
// Display information
//******************************************************************************

void CTile::print()
{
#ifdef DEBUG_PYRAMID
  std::cout<<"Tile :"<<id()<<std::endl;

  std::cout<<"   BottomRight corner: "<<bottomRight();
  std::cout<<"   Width: "<<width();
  std::cout<<"   Height: "<<height();

  std::cout<<"  Index: "<<index(0)<<"-"<<index(1)<<"-"<<index(2);

  if(existTileUp())
    std::cout<<"  Up: " <<tileUp()->id();
  else
    std::cout<<"  Up: NULL";

  if(existTileDown())
    std::cout<<"  Down: " <<tileDown()->id();
  else
    std::cout<<"  Down: NULL";

  std::cout<<std::endl;
#endif
}
//------------------------------------------------------------------------------
void CTile::printCorners()
{
  std::cout<<"\n[start] CTile: printCorners "<<std::endl;
  for ( uint i = 0; i < FCorners.size(); ++i )
    static_cast<CPyramidalDart*>(FCorners[i])->print();

  std::cout<<"[end] CTile: printCorners\n"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printMapDarts()
{
  std::cout<<"\n[start] CTile::printMapDarts  "<<std::endl;

  if(FMapDarts.empty())
    std::cout<<"WARNING: CTile::printMapDarts FMapDarts is empty"<<std::endl;
  else
    {
      std::map<TDartId, CPyramidalDart*>::iterator it;
      for ( it=FMapDarts.begin(); it!=FMapDarts.end(); ++it )
	std::cout<<" Key (dart id) = "<<it->first
		 <<" Value (dart*) = "<<it->second<<std::endl;
    }
  std::cout<<"[end] CTile: printDarts\n"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printMapRegions()
{
  std::cout<<"\n[start] CTile::printMapRegions  "<<std::endl;

  if(FMapRegions.empty())
    std::cout<<"WARNING: CTile::printMapRegions FMapRegions is empty"<<std::endl;
  else
    {
      std::map<TRegionId, CRegion*>::iterator it;
      for ( it=FMapRegions.begin(); it!=FMapRegions.end(); ++it )
      std::cout<<" Key (region id) = "<<(*it).first
	       <<" Value (region*) = "<<(*it).second<<std::endl;
    }
  std::cout<<"[end] CTile: printMapRegions\n"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printMapBeta2()
{
  std::cout<<"\n[start] CTile::printMapBeta2  "<<std::endl;

  if(FMapBeta2.empty())
    std::cout<<"WARNING: CTile::printMapBeta2 FMapBeta2 is empty"<<std::endl;
  else
    {
      std::map<TDartId, TDartId>::iterator it;
      for ( it=FMapBeta2.begin(); it!=FMapBeta2.end(); ++it )
	std::cout<<" Key (dart id in current tile) = "<<(*it).first
		 <<" Value (dart id in adjacent tile (beta2')) = "<<(*it).second<<std::endl;

    }
  std::cout<<"[end] CTile: printMapBeta2\n"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printPropertiesStruct(struct SProperties* AProperties)
{
  std::cout << "[start] CTile::printPropertiesStruct"<<std::endl;

  std::cout<<"Tuile n°"<<AProperties->id<<std::endl;
  std::cout<<"BottomRight    = "<<AProperties->bottomRight<<std::endl;
  std::cout<<"Index (i,j,k)  = "<<AProperties->indexI<<"-"<<AProperties->indexJ<<"-"<<AProperties->indexK<<std::endl;
  std::cout<<"up             = "<<AProperties->up<<std::endl;
  std::cout<<"down           = "<<AProperties->down<<std::endl;
  std::cout<<"nbDarts        = "<<AProperties->nbDarts<<std::endl;
  std::cout<<"nbRegions      = "<<AProperties->nbRegions<<std::endl;

  std::cout << "[end] CTile::printPropertiesStruct"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printTopoStruct(struct SDart* ADarts, uint ASize)
{
  std::cout << "[start] CTile::printTopoStruct"<<std::endl;

  for(uint i=0; i<ASize; ++i)
    {
      std::cout<<"Brin n°"<<ADarts[i].id<<std::endl;
      std::cout<<"Beta1  = "<<ADarts[i].beta1<<std::endl;
      std::cout<<"Beta2  = "<<ADarts[i].beta2<<std::endl;
      std::cout<<"doublet= "<<ADarts[i].doublet<<std::endl;
      std::cout<<"region = "<<ADarts[i].region<<std::endl;
      std::cout<<"up = "<<ADarts[i].up<<std::endl;
      std::cout<<"down = "<<ADarts[i].down<<"\n"<<std::endl;
    }

  std::cout << "[end] CTile::printTopoStruct"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printTreeStruct(struct SRegion* ARegions, uint ASize)
{
  std::cout << "[start] CTile::printTreeStruct"<<std::endl;

  for(uint i=0; i<ASize; ++i)
    {
      std::cout<<"Region n°"<<ARegions[i].id<<std::endl;
      std::cout<<"First pixel          = "<<ARegions[i].firstPixel<<std::endl;
      std::cout<<"Nombre de pixels     = "<<ARegions[i].nbPixels<<std::endl;
      std::cout<<"Somme nv gris        = "<<ARegions[i].greySum<<std::endl;
      std::cout<<"Somme carrés nv gris = "<<ARegions[i].greySquareSum<<std::endl;
      std::cout<<"up                   = "<<ARegions[i].up<<std::endl;
      std::cout<<"down                 = "<<ARegions[i].down<<std::endl;
      std::cout<<"gris min             = "<<ARegions[i].greyMin<<std::endl;
      std::cout<<"gris max             = "<<ARegions[i].greyMax<<std::endl;
      std::cout<<"father               = "<<ARegions[i].father<<std::endl;
      std::cout<<"brother              = "<<ARegions[i].brother<<std::endl;
      std::cout<<"cc                   = "<<ARegions[i].cc<<std::endl;
      std::cout<<"representative       = "<<ARegions[i].representative<<std::endl;
      std::cout<<"infinite             = "<<ARegions[i].infinite<<"\n"<<std::endl;
    }

  std::cout << "[end] CTile::printTreeStruct"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printChainRegionList()
{
  CRegion* rootRegion = getInclusionTreeRoot();
  CRegion* tmp = rootRegion;
  std::cout<<"\n***** Affichage de la liste chainée des régions *****"<<std::endl;
  std::cout<<"("<<tmp->getId();
  while(tmp->getFirstSon() != NULL )
    {
      std::cout<<", "<<tmp->getFirstSon()->getId();
      tmp = tmp->getFirstSon();
    }
  std::cout<<")"<<std::endl;

  tmp = rootRegion;
  while(tmp != NULL)
    {
      std::cout<<"\n region id: "<<tmp->getId()<<std::endl;
      std::cout<<" First Pixel: "<<static_cast<CPyramidalRegion*>(tmp)->getFirstPixel()<<std::endl;
      //std::cout<<" brin représentant: "<<getDoublet(tmp->getRepresentativeDart())<<std::endl;
      // std::cout<<" beta0 du brin représentant: "<<getDoublet(beta0(tmp->getRepresentativeDart()))<<std::endl;
      // std::cout<<" id self loop (getNextSameCC) : "<<tmp->getNextSameCC()->getId()<<std::endl;// !commenter si méthode utilisée après relabelDarts
      std::cout<<" id prev element (getBrother): "<<tmp->getBrother()->getId()<<std::endl;
      if( tmp->getFirstSon() != NULL )
	{
	  std::cout<<" id next element (getFirstSon): "<<tmp->getFirstSon()->getId()<<std::endl;
	  tmp = tmp->getFirstSon();
	}
      else
	{
	  std::cout<<"***** Fin de la liste chainée des régions *****\n"<<std::endl<<std::endl;
	  tmp = tmp->getFirstSon();
	}
    }
}
//------------------------------------------------------------------------------
void CTile::printRegionList(std::deque<CPyramidalRegion*>& AList)
{
  std::deque<CPyramidalRegion*>::iterator it;

  std::cout<<"\n*****Affichage d'une liste de régions*****"<<std::endl;
  int count = 0;
  for(it=AList.begin(); it != AList.end(); ++it)
    {
      std::cout<<"Region : "<<(*it)->getId();
      std::cout<<"   FirstPixel : ("<<(*it)->getFirstPixel()<<std::endl<<std::endl;
      ++count;
    }
  std::cout<<"Nombre de régions dans la liste : "<<count<<std::endl;
  std::cout<<"************************************"<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printInfosAllRegions()
{
  for( CDynamicCoverageAllRegions it( this ); it.cont();++it )
    static_cast<CPyramidalRegion*>(*it)->print();
}
//------------------------------------------------------------------------------
void CTile::printInfosAllDarts()
{
  for( CDynamicCoverageAll it( this ); it.cont();++it )
    static_cast<CPyramidalDart*>(*it)->print();
}
//------------------------------------------------------------------------------
void CTile::printInfosMap()
{
  uint nbD  = 0;
  uint nbV  = 0;
  uint nbE  = 0;
  uint nbF  = 0;
  uint nbCC = 0;
  std::string  sVoid = "-";

  getGlobalCharacteristics(&nbD, &nbV, &nbE, &nbF, &nbCC);

  std::cout<<std::endl<<" Carte pyramidale : "<<std::endl
	   <<"  Brins: "<<nbD
	   <<"   Sommets: "<<nbV
	   <<"   Arêtes: "<<nbE
	   <<"   Faces: "<<nbF
	   <<"   CC: "<<nbCC
	   <<"   Régions: "<<getNbRegions()<<std::endl;


  std::cout<<"   Level Up: ";
  /*
    if(existMapUp())
    std::cout<<getMapUp();
    else
    std::cout<<sVoid;
    std::cout<<"   Level Down: ";
    if(existMapDown())
    std::cout<<getMapDown();
    else
    std::cout<<sVoid;
  */
  std::cout<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printInfosMemory()
{
  std::cout<<"\t Carte:\t"<<getMemoryForMap()
	   <<"\t Arbre:\t"<<getMemoryForInclusionTree()
	   <<"\t Image:\t"<<getMemoryForImageTile()
	   <<"\t Khalimsky:\t"<<getMemoryForKhalimsky()<<"\n";
    //	   <<"   Total: "<<getMemoryForMap() + getMemoryForInclusionTree() + getMemoryForKhalimsky()<<std::endl;
}
//------------------------------------------------------------------------------
void CTile::printInclusionTree()
{
  for( CDynamicCoverageAllRegions it( this ); it.cont();++it )
    {
      CRegion* region =  *it;
      if(region->isInfiniteRegion())
	{
	  assert(region != NULL);
	  region->draw();
	}
    }
}
//------------------------------------------------------------------------------
void CTile::toSVG()
{
  CSvgExporter exporter;
  exporter.drawTile(this);
}


//******************************************************************************
// Controls
//******************************************************************************


bool CTile::checkGeometry()
{
  bool result = true;

  for( CDynamicCoverageAll it(this) ; it.cont() ; it++ )
    {
      if( !isLCell(getDoublet(*it)) )
	{
	  std::cerr<<"CTile::checkMapGeometry Le brin n'a pas de lignel : "<<
	    getDoublet(*it)<<std::endl;
	  result = false;
	}
      if( !isPCell(getDoublet(*it)) && !isEdgeLoop(*it) )
	{
	  std::cerr<<"CTile::checkMapGeometry Le brin n'a pas de pointel : "<<
	    getDoublet(*it)<<std::endl;
	  result = false;
	}
    }
  return result;
}
//------------------------------------------------------------------------------
bool CTile::checkSquareRegion(CDart* ADart) const
{
  bool result = true;
  CDoublet currentDoublet = getDoublet(ADart);
  CDoublet tmpDoublet;
  CDart* tmpDart = ADart;

  for( int i = 0 ; i < 4 ; i++ )
    {
      tmpDart = beta1( tmpDart );
      tmpDoublet = getDoublet(tmpDart);

      if(tmpDoublet != currentDoublet.getNextPointel().getNextLinel())
	{
	  result = false;
	  std::cerr<<"CTile::checkSquareRegion : erreur de géométrie"<<std::endl;
	}
      currentDoublet = tmpDoublet;
    }

  if( tmpDart != ADart )
    {
      result = false;
      std::cerr<<"CTile::checkSquareRegion : erreur de topologie"<<std::endl;
    }
  return result;
}
//------------------------------------------------------------------------------
bool CTile::checkDartLinks()
{
  //std::cout<<"  [start] CTile::checkDartLinks"<<std::endl;
  for( CDynamicCoverageAll it( this ); it.cont(); ++it )
    {
      CPyramidalDart* dart = static_cast<CPyramidalDart*>( *it );
      if ( dart->up() != NULL && dart->up()->down() != dart )
	//std::cout<<"checkDartLinks warning : no involution up/down "<<dart->doublet()<<std::endl;

      if(dart->existUp() && !dart->up()->existDown())
	std::cout<<"checkDartLinks error : wrong link up/down on dart "<<dart->doublet()<<std::endl;

    }
  return true;
  //std::cout<<"  [end] CTile::checkDartLinks"<<std::endl;
}
//------------------------------------------------------------------------------
bool CTile::checkRegionLinks()
{
  //std::cout<<"  [start] CTile::checkRegionLinks"<<std::endl;
  bool result = false;
  int cpt = 0;
  for( CDynamicCoverageAllRegions it( this ); it.cont(); ++it )
    {
    ++cpt;
    CPyramidalRegion* region = static_cast<CPyramidalRegion*>( *it );
    assert( region != NULL );

    if (region->getRegionUp() != NULL && region->getRegionUp()->getRegionDown() != region)
      {
	std::cerr<<"  CTile::checkRegionLinks : wrong link up/down on region "<<cpt<<" Details :\n"<<std::endl;
	region->print();
	result = false;
      }
    else
      {
	result = true;
      }
    }
  return result;
  //std::cout<<"  [end] CTile::checkRegionLinks"<<std::endl;
}
//------------------------------------------------------------------------------
bool CTile::checkDarts()
{
  return true;
  //std::cout<<"  [start] CTile::checkDarts"<<std::endl;
  bool found = false;
  CPyramidalDart* dart = NULL;
  CPyramidalDart* current = NULL;
  CPyramidalDart* search = NULL;

  for( CDynamicCoverageAll it( this ); it.cont(); ++it )
    {
      dart = static_cast<CPyramidalDart*>( *it );

      if(dart->id()==0)
	{
	  std::cout<<" CTile::checkDarts the following dart has a non-valid id:"<<std::endl;
	  dart->print();
	}

      for (int i=0;i<3;++i)
	{
	  search = static_cast<CPyramidalDart*>(beta(dart,i));
	  //todo: regarder ce qu'est devenu le getFirstDart
	  current = NULL;
	  //current = static_cast<CPyramidalDart*>(getFirstDart());
	  found = false;
	  while(current!=NULL && !found)
	    {
	      if(search==current)
		found = true;

	      //todo: regarder ce qu'est devenu le getNext
	      current = NULL;
	      //current = static_cast<CPyramidalDart*>(current->getNext());
	    }
	  if(!found)
	    {
	      std::cout<<" CTile::checkDarts the following dart does not belong to this map:"<<std::endl;
	      dart->print();
	      return false;
	    }
	}
      found = false;
    }
  return true;
  //std::cout<<"  [end] CTile::checkDarts"<<std::endl;
}
//------------------------------------------------------------------------------
bool CTile::checkRegions()
{
  //std::cout<<"  [start] CTile::checkNbRegions"<<std::endl;

  uint count = 0;
  for( CDynamicCoverageAllRegions it( this ); it.cont(); ++it )
    ++count;

  if(count!=getNbRegions())
    {
      std::cout<<" WARNING: checkRegions: "<<count<<" régions dans l'arbre contre "<<getNbRegions()<<" crées"<<std::endl;
      return false;
    }

  // check du nombre de pixels
  uint total = 0;
  uint local = 0;
  for( CDynamicCoverageAllRegions it( this ); it.cont(); ++it )
    {
      local=0;
      //for( CDynamicCoverageRegionPixels it2(this, (*it)); it2.cont(); ++it2)
      for( CCoverageRegionPixels it2( this, *it); it2.cont(); ++it2)
	{
	  ++total;
	  ++local;
	}
      if(local!=(*it)->getNbPixels() && !(*it)->isInfiniteRegion())
      {
	std::cout<<" WARNING: checkRegions: "<<local<<" pixels parcourus dans une région de "<<(*it)->getNbPixels()<<" pixels"<<std::endl;
	static_cast<CPyramidalRegion*>(*it)->print();
	return false;
      }
    }

  uint pix = width()*height();
  if(total-1!=pix)
    {
      std::cout<<" WARNING: checkRegions: "<<total-1<<" pixels parcourus contre une taille d'image = "<<pix<<std::endl;
      return false;
    }

  //std::cout<<"  [end] CTile::checkNbRegions"<<std::endl
  return true;
}

//------------------------------------------------------------------------------
bool CTile::checkNbChainRegions()
{
  //std::cout<<"  [start] CTile::checkNbRegions"<<std::endl;
  CRegion* current = getInclusionTreeRoot();

  uint count = 0;
  while(current!=NULL)
    {
      current = current->getFirstSon();
      ++count;
    }

  if(count!=getNbRegions())
    {
      std::cout<<" WARNING: checkNbChainRegions: "<<count<<" régions dans la liste contre "<<getNbRegions()<<" crées"<<std::endl;
      return false;
    }

  //std::cout<<"  [end] CTile::checkNbRegions"<<std::endl
  return true;
}

//------------------------------------------------------------------------------
bool CTile::isChainRegionListOk()
{
  //std::cout<<"\n[start] isChainRegionListOk"<<std::endl;
  bool result = true; bool result1=true; bool result2=true; bool result3=true; bool result4=true;

  CRegion* rootRegion = getInclusionTreeRoot();
  CRegion* tmp = rootRegion;
  TRegionId id = 0;
  CRegion* prev = NULL;
  CRegion* next = NULL;

  while(tmp != NULL && result)
    {
      id   = tmp->getId();
      next = tmp->getFirstSon();
      prev = tmp->getBrother();

      if( !(tmp->getRepresentativeDart() != NULL) )
	{
	  std::cerr<<"CTile::isChainRegionListOk() : le brin représentant de la région "<<id<<" est nul."<<std::endl;
	  return false;
	}

      if( !(id == tmp->getNextSameCC()->getId()) )
	{
	  std::cerr<<"CTile::isChainRegionListOk() : la région "<<id<<" ne boucle pas sur elle-même."<<std::endl;
	  return false;
	}

      if( next != NULL)
	if( !(id == next->getBrother()->getId()) )
	  {
	    std::cerr<<"CTile::isChainRegionListOk() : la région suivant la région "<<id<<" est incorrecte."<<std::endl;
	    return false;
	  }

      if( tmp != rootRegion)
	if( !(id == prev->getFirstSon()->getId()) )
	  {
	    std::cerr<<"CTile::isChainRegionListOk() : la région précédant la région "<<id<<" est incorrecte."<<std::endl;
	    return false;
	  }

      tmp = tmp->getFirstSon();
      result = result1 && result2 && result3 && result4;
    }

  //std::cout<<"\n[start] isChainRegionListOk"<<std::endl;
  return result;
}
//------------------------------------------------------------------------------
bool CTile::checkLignelDartMatrix(CVolume<CDart*>* AMatrix)
{

  uint count1 = 0;
  uint count2 = 0;
  uint count3 = 0;

  for( CDynamicCoverageAll it( this ); it.cont(); ++it )
    {
      CDoublet cur(getDoublet(*it));
      ++count3;
      CDoublet stop = getDoublet(beta1(*it));
      CDoublet next = cur;
      do{
	// Doublet suivant
	next.setNextPointel();
	int count=0;
	do
	  {
	    next.setNextLinel();
	    ++count;
	    assert(count<4);
	  }
	while( !isLCell( next ) );

	// Dessine le bord
	if( isPCell(next) || next == stop)
	  {
	    break;
	  }
	else
	  {
	    cur = next;
	    ++count3;
	  }
      }
      while ( 1 );
      ++count1;
    }


  for (uint y=0; y<height()+2; ++y)
    for(uint x=0; x<width()+2; ++x)
      for(uint z=0; z<5; ++z)
	{
	  CPyramidalDart* dart = static_cast<CPyramidalDart*>(AMatrix->getValue(x,y,z));
	  if( dart!=NULL)
	    {
	      ++count2;
	      //std::cout<<"x,y,z = "<<x<<","<<y<<","<<z<<std::endl;
	      //dart->print();
	    }
	}
  if(count2!=count3)
    {
      std::cout<<"nb brins = "<<count1<<std::endl;
      std::cout<<"nb lignels parcourus = "<<count3<<std::endl;
      std::cout<<"nb elt matrix = "<<count2<<std::endl;
    }

  return (count2==count3);
}
//------------------------------------------------------------------------------
bool CTile::checkHierarchy()
{
  //  if(!existTileUp())
    return true;

  CTile* upTile = tileUp();

  for( CDynamicCoverageAll it(upTile); it.cont(); ++it )
    {
      CPyramidalDart* upDart = static_cast<CPyramidalDart*>(*it);
      if( !upDart->existDown() )
	{
	  if( !upDart->getRegion()->isInfiniteRegion() )
	    {
	      std::cout << " CTile::checkHierarchy error: following dart has no down :"<<std::endl;
	      print();
	      upDart->print();
	      return false;
	    }
	}
      else
	{
	  CPyramidalDart* downDart = upDart->down();
	  if( downDart->up()!=upDart )
	    {
	      std::cout << " CTile::checkHierarchy error: first down has a wrong up :"<<std::endl;
	      downDart->print();
	      return false;
	    }
	}
    }
  return true;
}
//------------------------------------------------------------------------------
bool CTile::isOk()
{
  //std::cout<<"CTile::isOk() "<<std::endl;

  if(!isMapOk())
    {
      std::cout<<"CTile::isOk() error: topological map problem"<<std::endl;
      print();
      printInfosAllDarts();
      return false;
    }

  if(!checkGeometry())
    {
      std::cout<<"CTile::isOk() error: geometrical problem"<<std::endl;
      print();
      return false;
    }

  if(!checkDarts())
    {
      std::cout<<"CTile::isOk() error: cannot access some darts"<<std::endl;
      print();
      return false;
    }

  //Vérification que le nombre de régions accessibles correspond bien au nombre de
  // régions crées - régions supprimées
  // des problèmes peuvent venir -> d'un computeInclusionTree() en trop
  uint count = 0;
  for(CDynamicCoverageAllRegions it(this); it.cont(); ++it)
    ++count;
  if(count != getNbRegions())
    {
      std::cout<<"CTile::isOk() error: regions warning "<<std::endl;
      std::cout<<" traversed regions = "<<count<<std::endl;
      std::cout<<" number of regions = "<<getNbRegions()<<std::endl;
      return false;
    }

  //Pareil pour les brins
  count = 0;
  for(CDynamicCoverageAll it(this); it.cont(); ++it)
    ++count;
  if(count != getNbDarts())
    {
      std::cout<<"CTile::isOk() error: regions warning "<<std::endl;
      std::cout<<" traversed darts = "<<count<<std::endl;
      std::cout<<" number of darts = "<<getNbDarts()<<std::endl;
      return false;
    }


  // On vérifie que toutes les régions ont un représentant non null
  for(CDynamicCoverageAllRegions it(this); it.cont(); ++it)
    if((*it)->getRepresentativeDart()==NULL)
      {
	std::cout<<"Le brin représentant de la région "<<(*it)->getId()<<" est NULL"<<std::endl;
	return false;
      }

//  if(!checkHierarchy())
//    {
//      std::cout<<"CTile::isOk() error: hierarchical problem"<<std::endl;
//      print();
//      return false;
//    }
  return true;
}
//------------------------------------------------------------------------------
/*
bool CTile::isOk()
{
  //std::cout<<"  [start] CTile::isPyramidalMapOk()"<<std::endl;
  bool result = false;
  bool nbPixelsOk = true;

  // Vérification de la somme des pixels des régions avec la taille de l'image
  int nbPixels = 0;
  int count = 0;
  for( CDynamicCoverageAllRegions it( this ); it.cont(); ++it )
    {
      //std::cout<<"CTile::isPyramidalMapOk : checking region "<<(*it)->getId()<<std::endl;
      assert( static_cast<CPyramidalRegion*>(*it)->isPyramidalRegionOk() );
      nbPixels += (*it)->getNbPixels();
      ++count;
    }
  (nbPixels == getSizeX()*getSizeY())?nbPixelsOk=true:nbPixelsOk=false;

  if( !nbPixelsOk )
    {
      std::cerr<<"CTile::isPyramidalMapOk : wrong number of pixels"<<std::endl;
      std::cout<<"there are "<<nbPixels<<" pixels in "<<count<<" regions."<<std::endl;
      std::cout<<"the image size is "<<getSizeX()<<"*"<<getSizeY()<<"="<<getSizeX()*getSizeY()<<" pixels"<<std::endl;
    }

  result = nbPixelsOk;
  //std::cout<<"  [end] CTile::isPyramidalMapOk()"<<std::endl;
  return result;
}
*/


//******************************************************************************
// Memory
//******************************************************************************


//------------------------------------------------------------------------------
unsigned long int CTile::memory() const
{
  return getMemoryForTile() + getMemoryForMap() +
    getMemoryForInclusionTree() + getMemoryForKhalimsky() +
    getMemoryForImageTile();
}
//------------------------------------------------------------------------------
unsigned long int CTile::getMemoryForImageTile() const
{
  return ymax(Relative)*xmax(Relative)*image()->colorMode();
}
//------------------------------------------------------------------------------
unsigned long int CTile::getMemoryForTile() const
{
  //std::cout<<"size of CTile : "<<sizeof(CTile)<<std::endl;

  unsigned long int result = sizeof(CTile);

  //std::cout<<" size map darts = "<<FMapDarts.size()<<std::endl;
  //std::cout<<" size TDartId = "<<sizeof(TDartId)<<std::endl;
  //std::cout<<" size CPyramidalDart* = "<<sizeof(CPyramidalDart*)<<std::endl;

  result += sizeof(std::map<TDartId, CPyramidalDart*>::value_type)*FMapDarts.size();
  result += sizeof(std::map<TRegionId, CRegion*>::value_type)*FMapRegions.size();
  result += sizeof(std::map<TDartId, TDartId>::value_type)*FMapBeta2.size();
  result += sizeof(std::vector<CDart*>::value_type)*FCorners.size();

  return result;
}

//------------------------------------------------------------------------------
/*
bool CTile::checkComputeLine()
{
  //Vérification du tracé entre 2 pointels

  CTile* tile = NULL;
  CSvgExporter svg;
  CDoublet pointel1, pointel2;

  tile = new CTile(10,10);
  tile->setId(1);
  pointel1.setDoublet(0,4,XPOS); pointel2.setDoublet(2,6,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(2);
  pointel1.setDoublet(0,4,XPOS); pointel2.setDoublet(2,4,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(3);
  pointel1.setDoublet(0,6,XPOS); pointel2.setDoublet(2,4,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(4);
  pointel1.setDoublet(2,4,XPOS); pointel2.setDoublet(2,6,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(5);
  pointel1.setDoublet(2,4,XPOS); pointel2.setDoublet(2,4,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(6);
  pointel1.setDoublet(2,6,XPOS); pointel2.setDoublet(2,4,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(7);
  pointel1.setDoublet(2,4,XPOS); pointel2.setDoublet(0,6,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(8);
  pointel1.setDoublet(2,4,XPOS); pointel2.setDoublet(0,6,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  tile = new CTile(10,10);
  tile->setId(9);
  pointel1.setDoublet(2,6,XPOS); pointel2.setDoublet(0,4,XPOS);
  tile->computeLine(pointel1,pointel2,false,true);
  svg.drawTileGeometry(tile);
  delete tile;

  return true;
}
*/

//------------------------------------------------------------------------------
void CTile::removeDanglingEdges()
{
  int toDelete = getNewMark();
  for(CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      if(!isMarked(*it,toDelete) )
	{
	  if(beta1(*it) == beta2(*it))
	    {
	      //on éteint le plongement
	      CDoublet init = getDoublet(*it);
	      CDoublet next(init);
	      setLCell(init, false);
	      do
		{
		  init = next;
		  next.setNextPointel();
		  assert(next.getNextPointel()==init);
		  int count = 0;
		  do
		    {
		      next = next.getNextLinel();
		      ++count;
		      assert(count<5);
		    }
		  while( !isLCell(next) && (next.getNextPointel()!=init) );
		  setPCell(next,false);
		  setLCell(next,false);
		  if( next.getNextPointel() == init)
		    break;
		}
	      while(1);

	      //on fait les coutures et on marque les brins à supprimer
	      linkBeta1(beta0(*it),beta1(beta1(*it)));
	      setMark(*it, toDelete);
	      setMark(beta1(*it), toDelete);
	    }
	  else if ( beta0(*it) == beta2(*it) )
	    {
	      //on éteint le plongement
	      CDoublet init = getDoublet(beta0(*it));
	      CDoublet next(init);
	      //setPCell(init, false);
	      setLCell(init, false);
	      do
		{
		  init = next;
		  next.setNextPointel();
		  assert(next.getNextPointel()==init);
		  int count = 0;
		  do
		    {
		      next = next.getNextLinel();
		      ++count;
		      assert(count<5);
		    }
		  while( !isLCell(next) && (next.getNextPointel()!=init) );
		  setPCell(next,false);
		  setLCell(next,false);
		  if( next.getNextPointel() == init)
		    break;
		}
	      while(1);

	      //on fait les coutures et on marque les brins à supprimer
	      linkBeta1(beta0(beta0(*it)), beta1(*it));
	      setMark(*it, toDelete);
	      setMark(beta2(*it), toDelete);
	    }
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
}
//-----------------------------------------------------------------------------
void CTile::removeLoops()
{
  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      CPyramidalDart* dart = static_cast<CPyramidalDart*>(*it);
      if(beta1(dart)==dart)
	{
	  dart->setFictive(true);
	  static_cast<CPyramidalDart*>(beta2(dart))->setFictive(true);
	}
    }
}
