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
#include "tile.hh"
#include "tile.hh"
#include "pyramidal-dart.hh"
#include "pyramidal-region.hh"

#define UNUSED(x) (void)x

using namespace Map2d;
//******************************************************************************
bool CTile::readInt(const std::string& fileName, std::vector<uint>& dest)
{
  std::ifstream istr (fileName.c_str(), std::ios::in | std::ios::trunc );
  uint value;
  bool result = false;
  istr.open(fileName.c_str());

  if ( istr.fail() )
    std::cout<<"CTile::readInt Unable to open file "<<fileName.c_str()<<std::endl;

  while (istr >> value)
    dest.push_back(value);

  if (istr.eof())
    result = true;
  else
    std::cout<<"CTile::readInt Warning "<<fileName.c_str()<<std::endl;

  istr.close();
  return result;
}
//------------------------------------------------------------------------------
void CTile::saveProperties()
{
  //std::cout<<"[start] CTile::saveProperties"<<std::endl;
  assert(isOk());
  FProperties = new SProperties[1];

  FProperties->id          = id();
  FProperties->bottomRight = bottomRight();
  FProperties->indexI      = index(0);
  FProperties->indexJ      = index(1);
  FProperties->indexK      = index(2);
  FProperties->nbDarts     = getNbDarts();
  FProperties->nbRegions   = getNbRegions();

  if(existTileUp())
    FProperties->up = tileUp()->id();
  else if(FFirst)
    FProperties->up = 0;
  else FProperties->up = FOldProperties->up;

  if(existTileDown())
    FProperties->down = tileDown()->id();
  else if(FFirst)
    FProperties->down = 0;
  else
    FProperties->down = FOldProperties->down;

  //std::cout<<"[end] CTile::saveProperties"<<std::endl;
}

//------------------------------------------------------------------------------
void CTile::loadProperties()
{
  setId(FProperties->id);
  setBottomRight(FProperties->bottomRight);
  setIndex(0, FProperties->indexI);
  setIndex(1, FProperties->indexJ);
  setIndex(2, FProperties->indexK);
  assert(width()!=0 && height()!=0);
}

//------------------------------------------------------------------------------
void CTile::loadEmptyTopology()
{
  //std::cout<<"[start] CTile::loadEmptyTopology"<<std::endl;
  FMapDarts.clear();
  CPyramidalDart* newDart = NULL;
  unsigned int nbDarts = FProperties->nbDarts;

  for (unsigned int i = 0; i < nbDarts; ++i)
    {
      newDart = static_cast<CPyramidalDart*>(addMapDart());
      FMapDarts[FDartFields[i].id] = newDart;
    }

  //std::cout<<"[end] CTile::loadEmptyTopology"<<std::endl;
}

//------------------------------------------------------------------------------
void CTile::loadTopology()
{
  //std::cout<<"[start] CTile::loadTileTopology"<<std::endl;

  CPyramidalDart* newDart = NULL;
  std::map<TDartId, CPyramidalDart*>::iterator itd;
  std::map<TRegionId, CRegion*>::iterator itr;

  uint nbDarts = FProperties->nbDarts;

  for(uint i = 0; i < nbDarts; ++i, newDart=NULL)
    {
      itd = FMapDarts.find(FDartFields[i].id);
      assert(itd!=FMapDarts.end());
      newDart = itd->second;
      newDart->setId(itd->first);

      itd = FMapDarts.find(FDartFields[i].beta1); //On cherche le beta1
      assert(itd->first!=0);
      assert(itd!=FMapDarts.end());

      assert(itd->second!=NULL);
      newDart->setBeta1(itd->second); //le beta1 a déjà été crée

      itd->second->setBeta0(newDart);

      assert(newDart->getBeta1()!=NULL);

      setDoublet(newDart, FDartFields[i].doublet);
      newDart->setFictive(FDartFields[i].fictive);

      // Coutures beta2
      // Pour un brin du bord, on ne fait pas la couture mais on renseigne la
      // map des beta2. Ils seront cousus correctement lorsqu'on traitera le
      // brin de la région infinie correspondant

      // Bords gauche et haut
      CDoublet doublet = newDart->doublet();
      if( (doublet.getX() == 0 && doublet.getLinel() == YNEG) ||
	  (doublet.getY() == 0 && doublet.getLinel() == XPOS) )
	FMapBeta2[FDartFields[i].id] = FDartFields[i].beta2;
      else
	{
	  itd = FMapDarts.find(FDartFields[i].beta2);
	  assert( itd!=FMapDarts.end() );

	  newDart->setBeta2(itd->second);
	  itd->second->setBeta2(newDart);
	}

      itr = FMapRegions.find(FDartFields[i].region); //On cherche la région

      assert( itr!=FMapRegions.end() );
      newDart->setRegion(itr->second);

      if( FDartFields[i].up!=0 && existTileUp() )
	{
	  itd = tileUp()->FMapDarts.find(FDartFields[i].up);
	  assert( itd != tileUp()->FMapDarts.end() );
	  linkDartUpDown(itd->second, newDart);
	}

      if( FDartFields[i].down!=0 && existTileDown() )
	{
	  itd = tileDown()->FMapDarts.find(FDartFields[i].down);
	  assert( itd != tileDown()->FMapDarts.end() );
	  tileDown()->linkDartUpDown(newDart,itd->second);
	}
    }

  //assert(checkDarts());

  //std::cout<<"[end] CTile::loadTopology"<<std::endl;
}

//------------------------------------------------------------------------------
void CTile::saveTopology()
{
  //std::cout<<"[start] CTile::saveTopology"<<std::endl;

  std::map<TDartId, TDartId>::iterator it2; //it pour les beta2 du bord
  CPyramidalDart* current = NULL;
  unsigned int count = 0;
  assert(getNbDarts()>0);
  assert(FDartFields==NULL);
  FDartFields = new SDart[getNbDarts()];

  for( CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      current = static_cast<CPyramidalDart*>(*it);

      FDartFields[count].id    = current->getId();
      FDartFields[count].beta1 = current->getBeta1()->getId();

      it2 = FMapBeta2.find(current->getId());

      if(it2 != FMapBeta2.end())
	FDartFields[count].beta2 = it2->second;
      else
	FDartFields[count].beta2 = current->getBeta2()->getId();

      FDartFields[count].doublet = current->doublet();
      FDartFields[count].fictive  = current->isFictive();
      FDartFields[count].region  = getRegion(current)->getId();

      if(current->existDartUp())
	FDartFields[count].up = current->getDartUp()->getId();
      else if(FFirst)
	FDartFields[count].up = 0;
      else
	FDartFields[count].up = FOldDarts[count].up;

      if(current->existDartDown())
	FDartFields[count].down = current->getDartDown()->getId();
      else if(FFirst)
	FDartFields[count].down = 0;
      else
	FDartFields[count].down = FOldDarts[count].down;

      ++count;
    }
  //std::cout<<" count = "<<count<<std::endl;
  //std::cout<<" nbDarts = "<<getNbDarts()<<std::endl;
  //std::cout<<" nbDartsinTabs = "<<getNbDartsInTabs()<<std::endl;
  assert(count == getNbDarts());

  //std::cout<<"[end] CTile::saveTopology"<<std::endl;
}


//------------------------------------------------------------------------------
void CTile::loadEmptyTree()
{
  //std::cout<<"[start] CTile::loadEmptyTree"<<std::endl;

  FMapRegions.clear();
  CPyramidalRegion* newRegion = NULL;
  unsigned int nbRegions = FProperties->nbRegions;

  for (unsigned int i = 0; i < nbRegions; ++i)
    {
      if(FRegionFields[i].infinite)
	{
	  newRegion = addMapInfiniteRegion();
	  setInclusionTreeRoot(newRegion);
	}
      else
	{
	  newRegion = addMapRegion();
	}

      FMapRegions[FRegionFields[i].id] = newRegion;
    }
  //std::cout<<"[end] CTile::loadEmptyTree"<<std::endl;
}


//------------------------------------------------------------------------------
void CTile::loadTree()
{
  //std::cout<<"[start] CTile::loadTree"<<std::endl;

  CPyramidalRegion* newRegion = NULL;
  std::map<TRegionId, CRegion*>::iterator itr;
  unsigned int nbRegions = FProperties->nbRegions;

  for (unsigned int i=0; i < nbRegions; ++i)
    {
      // La région a déjà été créée: on remplit juste ses champs
      newRegion = static_cast<CPyramidalRegion*>(FMapRegions.find(FRegionFields[i].id)->second);
      assert(newRegion!=NULL);

      newRegion->setId(FRegionFields[i].id);
      newRegion->setFirstPixel(FRegionFields[i].firstPixel);
      newRegion->setNbPixels(FRegionFields[i].nbPixels);
      newRegion->setGreySum(FRegionFields[i].greySum);
      newRegion->setGreySquareSum(FRegionFields[i].greySquareSum);

      if(!newRegion->isInfiniteRegion())
	{
	  if( FRegionFields[i].up != 0 && existTileUp() )
	    {
	      itr = tileUp()->FMapRegions.find( FRegionFields[i].up );
	      linkRegionUpDown(itr->second, newRegion);
	    }

	  if( FRegionFields[i].down != 0 && existTileDown() )
	    {
	      itr = tileDown()->FMapRegions.find( FRegionFields[i].down );
	      tileDown()->linkRegionUpDown(newRegion, itr->second);
	    }
	}
      else
	{
	  if(existTileUp())
	    {
	      CPyramidalRegion* upInfiniteRegion =
		static_cast<CInfinitePyramidalRegion*>(tileUp()->getInclusionTreeRoot());

	      newRegion->setRegionUp(upInfiniteRegion);
	      upInfiniteRegion->setRegionDown(newRegion);

	    }
	  if(existTileDown())
	    {
	      CPyramidalRegion* downInfiniteRegion =
		static_cast<CInfinitePyramidalRegion*>(tileDown()->getInclusionTreeRoot());

	      downInfiniteRegion->setRegionUp(newRegion);
	      newRegion->setRegionDown(downInfiniteRegion);
	    }
	}

      newRegion->setGreyMin(FRegionFields[i].greyMin);
      newRegion->setGreyMax(FRegionFields[i].greyMax);
      newRegion->setLabel((CPyramidalRegion::Label)FRegionFields[i].label);

      if( FRegionFields[i].father != 0 )
	newRegion->setFather(FMapRegions.find(FRegionFields[i].father)->second);

      if( FRegionFields[i].son != 0 )
	newRegion->setFirstSon(FMapRegions.find(FRegionFields[i].son)->second);

      if( FRegionFields[i].brother != 0 )
	newRegion->setBrother(FMapRegions.find(FRegionFields[i].brother)->second);

      if( FRegionFields[i].cc != 0 )
	newRegion->setNextSameCC(FMapRegions.find(FRegionFields[i].cc)->second);

      assert( FRegionFields[i].representative != 0 );
      newRegion->setRepresentativeDart((FMapDarts.find(FRegionFields[i].representative))->second);
    }
  //std::cout<<"[end] CTile::loadTree"<<std::endl;
}


//------------------------------------------------------------------------------
void CTile::saveTree()
{
  //std::cout<<"[start] CTile::saveTree"<<std::endl;

  CPyramidalRegion* current = NULL;
  unsigned int count = 0;
  assert(getNbRegions()>0);
  FRegionFields = new SRegion[getNbRegions()];

  for( CDynamicCoverageAllRegions it(this); it.cont(); ++it)
    {
      if(!(*it)->isInfiniteRegion())
	{
	  current = static_cast<CPyramidalRegion*>(*it);

	  if(current->existRegionUp())
	    FRegionFields[count].up = current->getRegionUp()->getId();
	  else if (FFirst)
	    FRegionFields[count].up = 0;
	  else FRegionFields[count].up = FOldRegions[count].up;


	  if(current->existRegionDown())
	    FRegionFields[count].down = current->getRegionDown()->getId();
	  else if(FFirst)
	    FRegionFields[count].down = 0;
	  else
	    FRegionFields[count].down = FOldRegions[count].down;

	  FRegionFields[count].id            = current->getId();
	  FRegionFields[count].firstPixel    = current->getFirstPixel();
	  FRegionFields[count].nbPixels      = current->getNbPixels();
	  FRegionFields[count].greySum       = current->getGreySum();
	  FRegionFields[count].greySquareSum = current->getGreySquareSum();
	  FRegionFields[count].greyMin       = current->getGreyMin();
	  FRegionFields[count].greyMax       = current->getGreyMax();
	  FRegionFields[count].label         = current->label();
	  FRegionFields[count].infinite      = false;
	}
      else
	{
	  current = static_cast<CInfinitePyramidalRegion*>(*it);
	  FRegionFields[count].id = 1;
	  FRegionFields[count].up = 1;
	  FRegionFields[count].down = 1;
	  FRegionFields[count].infinite = true;
	  FRegionFields[count].firstPixel    = CPoint2D(0,0);
	  FRegionFields[count].nbPixels      = 0;
	  FRegionFields[count].greySum       = 0;
	  FRegionFields[count].greySquareSum = 0;
	  FRegionFields[count].greyMin       = 0;
	  FRegionFields[count].greyMax       = 0;
	  FRegionFields[count].label         = CPyramidalRegion::Invalid;
	}


      if(current->existFather())
	FRegionFields[count].father = current->getFather()->getId();
      else
	FRegionFields[count].father = 0;

      if(current->existSon())
	FRegionFields[count].son = current->getFirstSon()->getId();
      else
	FRegionFields[count].son = 0;

      if(current->existBrother())
	FRegionFields[count].brother = current->getBrother()->getId();
      else
	FRegionFields[count].brother = 0;

      if(current->existNextSameCC())
	FRegionFields[count].cc = current->getNextSameCC()->getId();
      else
	FRegionFields[count].cc = 0;

      //static_cast<CPyramidalDart*>(current->getRepresentativeDart())->printInfos();
      assert( static_cast<CPyramidalDart*>(current->getRepresentativeDart())->getId()!=0 );
      FRegionFields[count].representative = static_cast<CPyramidalDart*>(current->getRepresentativeDart())->getId();

      ++count;
    }

  //std::cout<<" count = "<<count<<std::endl;
  //std::cout<<" nb regions = "<<getNbRegions()<<std::endl;
  assert(count == getNbRegions());
  //std::cout<<"[end] CTile::saveTree"<<std::endl;
}


//------------------------------------------------------------------------------
void CTile::write()
{
  //std::cout<<"[start] CTile::write "<<" ("<<index(0)<<","<<index(1)<<","<<index(2)<<")"<<std::endl;
  assert(isOk());
  setFilename(index(0), index(1), index(2));

  unsigned int header[3];
  FFirst = !fileExist(FFilename.c_str());

  // La tuile existait déjà, on récupère les infos du fichier
  // dans les structures "old"
  if ( !FFirst )
    {
      FILE * file = NULL;
      file = fopen(FFilename.c_str(), "rb");
      assert(file!=NULL);

      UNUSED(fread(static_cast<void*>(header), sizeof(header), 1, file));

      FOldProperties = new SProperties[1];
      UNUSED(fread( static_cast<void*>(FOldProperties), sizeof(SProperties), 1, file ));

      FOldDarts = new SDart[header[1]/sizeof(SDart)];
      unsigned int offset = sizeof(header)+sizeof(SProperties)+header[0];
      fseek(file, offset, SEEK_SET);
      UNUSED(fread( static_cast<void*>(FOldDarts), header[1], 1, file ));

      FOldRegions = new SRegion[header[2]/sizeof(SRegion)];
      UNUSED(fread( static_cast<void*>(FOldRegions), header[2], 1, file ));

      fclose(file);
    }

  FMatrix = getKhalimsky()->getMatrix();
  header[0] = getKhalimskySizeX()*getKhalimskySizeY();
  header[1] = getNbDarts()*sizeof(SDart);
  header[2] = getNbRegions()*sizeof(SRegion);
  saveProperties();
  saveTopology();
  saveTree();

  if (!FFirst)
    {
      delete [] FOldDarts;      FOldDarts=NULL;
      delete [] FOldRegions;    FOldRegions=NULL;
      delete [] FOldProperties; FOldProperties=NULL;
    }

  UNUSED(remove ( FFilename.c_str() ));
  FILE * pFile;
  pFile = fopen ( FFilename.c_str(), "wb" );
  assert(pFile!=NULL);

  UNUSED(fwrite( static_cast<void*>(header), sizeof(header), 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(FProperties), sizeof(SProperties), 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(FMatrix), header[0], 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(FDartFields), header[1], 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(FRegionFields), header[2], 1, pFile ));
  fclose (pFile);

  delete [] FProperties;   FProperties=NULL;
  delete [] FDartFields;   FDartFields=NULL;
  delete [] FRegionFields; FRegionFields=NULL;

  //std::cout<<"[end] CTile::write"<<std::endl;
}


//------------------------------------------------------------------------------
void CTile::load(unsigned int Ai, unsigned int Aj, unsigned int Ak)
{
  //std::cout<<"[start] CTile::load "<<" ("<<Ai<<","<<Aj<<","<<Ak<<")"<<std::endl;
  setFilename(Ai,Aj,Ak);

  unsigned int header[3];
  FILE * file;
  file = fopen ( FFilename.c_str(), "rb" );
  if (file==NULL) {std::cout<<"CTile::load() : unable to open file "<<FFilename<<std::endl; assert(false);}

  // Lecture du header qui permet de récupérer les tailles
  UNUSED(fread( static_cast<void*>(header), sizeof(header), 1, file ));

  // Lecture des propriétés
  FProperties = new SProperties[1];
  UNUSED(fread( static_cast<void*>(FProperties), sizeof(SProperties), 1, file ));

  loadProperties();
  //printPropertiesStruct(FProperties);

  // Lecture de la matrice
  CKhalimsky* khalimsky = new CKhalimsky(width(), height());
  FMatrix = khalimsky->getMatrix();
  //on s'assure que la taille de la matrice est équivalente à celle lue sur le disque
  assert( (width()+1)*(height()+1)==header[0] );
  UNUSED(fread( static_cast<void*>(FMatrix), header[0], 1, file ));

  khalimsky->setMatrix(FMatrix);
  setKhalimsky(khalimsky);

  // Lecture des brins
  FDartFields = new SDart[FProperties->nbDarts];
  UNUSED(fread( static_cast<void*>(FDartFields), header[1], 1, file ));

  loadEmptyTopology();

  // Lecture des régions
  FRegionFields = new SRegion[FProperties->nbRegions];
  UNUSED(fread( static_cast<void*>(FRegionFields), header[2], 1, file ));

  loadEmptyTree();

  fclose(file);

  loadTopology();
  loadTree();

  delete [] FDartFields;   FDartFields = NULL;
  delete [] FRegionFields; FRegionFields = NULL;
  delete [] FProperties;   FProperties = NULL;

  assert(isOk());

  //std::cout<<"[end] CTile::load"<<std::endl;
}
