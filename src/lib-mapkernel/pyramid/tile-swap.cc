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

#include "tile.hh"
#include "tile.hh"
#include "pyramidal-dart.hh"
#include "pyramidal-region.hh"

#define UNUSED(x) (void)x

using namespace Map2d;
using namespace pyramid;

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

void CTile::saveProperties()
{
  //std::cout<<"[start] CTile::saveProperties"<<std::endl;
  assert(isOk());
  m_properties = new SProperties[1];

  m_properties->id          = id();
  m_properties->bottomRight = bottomRight();
  m_properties->indexI      = index(0);
  m_properties->indexJ      = index(1);
  m_properties->indexK      = index(2);
  m_properties->nbDarts     = getNbDarts();
  m_properties->nbRegions   = getNbRegions();

  if(existTileUp())
    m_properties->up = tileUp()->id();
  else if(m_first)
    m_properties->up = 0;
  else m_properties->up = m_oldProperties->up;

  if(existTileDown())
    m_properties->down = tileDown()->id();
  else if(m_first)
    m_properties->down = 0;
  else
    m_properties->down = m_oldProperties->down;

  //std::cout<<"[end] CTile::saveProperties"<<std::endl;
}

void CTile::loadProperties()
{
  setId(m_properties->id);
  setBottomRight(m_properties->bottomRight);
  setIndex(0, m_properties->indexI);
  setIndex(1, m_properties->indexJ);
  setIndex(2, m_properties->indexK);
  assert(width()!=0 && height()!=0);
}

void CTile::loadEmptyTopology()
{
  //std::cout<<"[start] CTile::loadEmptyTopology"<<std::endl;
  m_mapDarts.clear();
  CPyramidalDart* newDart = NULL;
  unsigned int nbDarts = m_properties->nbDarts;

  for (unsigned int i = 0; i < nbDarts; ++i)
    {
      newDart = static_cast<CPyramidalDart*>(addMapDart());
      m_mapDarts[m_dartFields[i].id] = newDart;
    }

  //std::cout<<"[end] CTile::loadEmptyTopology"<<std::endl;
}

void CTile::loadTopology()
{
  //std::cout<<"[start] CTile::loadTileTopology"<<std::endl;

  CPyramidalDart* newDart = NULL;
  std::map<TDartId, CPyramidalDart*>::iterator itd;
  std::map<TRegionId, CRegion*>::iterator itr;

  uint nbDarts = m_properties->nbDarts;

  for(uint i = 0; i < nbDarts; ++i, newDart=NULL)
    {
      itd = m_mapDarts.find(m_dartFields[i].id);
      assert(itd!=m_mapDarts.end());
      newDart = itd->second;
      newDart->setId(itd->first);

      itd = m_mapDarts.find(m_dartFields[i].beta1); //On cherche le beta1
      assert(itd->first!=0);
      assert(itd!=m_mapDarts.end());

      assert(itd->second!=NULL);
      newDart->setBeta1(itd->second); //le beta1 a déjà été crée

      itd->second->setBeta0(newDart);

      assert(newDart->getBeta1()!=NULL);

      setDoublet(newDart, m_dartFields[i].doublet);
      newDart->setFictive(m_dartFields[i].fictive);

      // Coutures beta2
      // Pour un brin du bord, on ne fait pas la couture mais on renseigne la
      // map des beta2. Ils seront cousus correctement lorsqu'on traitera le
      // brin de la région infinie correspondant

      // Bords gauche et haut
      CDoublet doublet = newDart->doublet();
      if( (doublet.getX() == 0 && doublet.getLinel() == YNEG) ||
	  (doublet.getY() == 0 && doublet.getLinel() == XPOS) )
	m_mapBeta2[m_dartFields[i].id] = m_dartFields[i].beta2;
      else
	{
	  itd = m_mapDarts.find(m_dartFields[i].beta2);
	  assert( itd!=m_mapDarts.end() );

	  newDart->setBeta2(itd->second);
	  itd->second->setBeta2(newDart);
	}

      itr = m_mapRegions.find(m_dartFields[i].region); //On cherche la région

      assert( itr!=m_mapRegions.end() );
      newDart->setRegion(itr->second);

      if( m_dartFields[i].up!=0 && existTileUp() )
	{
	  itd = tileUp()->m_mapDarts.find(m_dartFields[i].up);
	  assert( itd != tileUp()->m_mapDarts.end() );
	  linkDartUpDown(itd->second, newDart);
	}

      if( m_dartFields[i].down!=0 && existTileDown() )
	{
	  itd = tileDown()->m_mapDarts.find(m_dartFields[i].down);
	  assert( itd != tileDown()->m_mapDarts.end() );
	  tileDown()->linkDartUpDown(newDart,itd->second);
	}
    }
  //assert(checkDarts());
  //std::cout<<"[end] CTile::loadTopology"<<std::endl;
}

void CTile::saveTopology()
{
  //std::cout<<"[start] CTile::saveTopology"<<std::endl;

  std::map<TDartId, TDartId>::iterator it2; //it pour les beta2 du bord
  CPyramidalDart* current = NULL;
  unsigned int count = 0;
  assert(getNbDarts()>0);
  assert(m_dartFields==NULL);
  m_dartFields = new SDart[getNbDarts()];

  for( CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      current = static_cast<CPyramidalDart*>(*it);

      m_dartFields[count].id    = current->id();
      m_dartFields[count].beta1 = current->getBeta1()->id();

      it2 = m_mapBeta2.find(current->id());

      if(it2 != m_mapBeta2.end())
	m_dartFields[count].beta2 = it2->second;
      else
	m_dartFields[count].beta2 = current->getBeta2()->id();

      m_dartFields[count].doublet = current->doublet();
      m_dartFields[count].fictive  = current->isFictive();
      m_dartFields[count].region  = getRegion(current)->id();

      if(current->existUp())
	m_dartFields[count].up = current->up()->id();
      else if(m_first)
	m_dartFields[count].up = 0;
      else
	m_dartFields[count].up = m_oldDarts[count].up;

      if(current->existDown())
	m_dartFields[count].down = current->down()->id();
      else if(m_first)
	m_dartFields[count].down = 0;
      else
	m_dartFields[count].down = m_oldDarts[count].down;

      ++count;
    }
  //std::cout<<" count = "<<count<<std::endl;
  //std::cout<<" nbDarts = "<<getNbDarts()<<std::endl;
  //std::cout<<" nbDartsinTabs = "<<getNbDartsInTabs()<<std::endl;
  assert(count == getNbDarts());

  //std::cout<<"[end] CTile::saveTopology"<<std::endl;
}

void CTile::loadEmptyTree()
{
  //std::cout<<"[start] CTile::loadEmptyTree"<<std::endl;

  m_mapRegions.clear();
  CPyramidalRegion* newRegion = NULL;
  unsigned int nbRegions = m_properties->nbRegions;

  for (unsigned int i = 0; i < nbRegions; ++i)
    {
      if(m_regionFields[i].infinite)
	{
	  newRegion = addMapInfiniteRegion();
	  setInclusionTreeRoot(newRegion);
	}
      else
	{
	  newRegion = addMapRegion();
	}

      m_mapRegions[m_regionFields[i].id] = newRegion;
    }
  //std::cout<<"[end] CTile::loadEmptyTree"<<std::endl;
}

void CTile::loadTree()
{
  //std::cout<<"[start] CTile::loadTree"<<std::endl;

  CPyramidalRegion* newRegion = NULL;
  std::map<TRegionId, CRegion*>::iterator itr;
  unsigned int nbRegions = m_properties->nbRegions;

  for (unsigned int i=0; i < nbRegions; ++i)
    {
      // La région a déjà été créée: on remplit juste ses champs
      newRegion = static_cast<CPyramidalRegion*>(m_mapRegions.find(m_regionFields[i].id)->second);
      assert(newRegion!=NULL);

      newRegion->setId(m_regionFields[i].id);
      newRegion->setFirstPixel(m_regionFields[i].firstPixel);
      newRegion->setNbPixels(m_regionFields[i].nbPixels);
      newRegion->setGreySum(m_regionFields[i].greySum);
      newRegion->setGreySquareSum(m_regionFields[i].greySquareSum);

      if(!newRegion->isInfiniteRegion())
	{
	  if( m_regionFields[i].up != 0 && existTileUp() )
	    {
	      itr = tileUp()->m_mapRegions.find( m_regionFields[i].up );
	      linkRegionUpDown(itr->second, newRegion);
	    }

	  if( m_regionFields[i].down != 0 && existTileDown() )
	    {
	      itr = tileDown()->m_mapRegions.find( m_regionFields[i].down );
	      tileDown()->linkRegionUpDown(newRegion, itr->second);
	    }
	}
      else
	{
	  if(existTileUp())
	    {
	      CPyramidalRegion* upInfiniteRegion =
		static_cast<CInfinitePyramidalRegion*>(tileUp()->getInclusionTreeRoot());

	      newRegion->setUp(upInfiniteRegion);
	      upInfiniteRegion->setDown(newRegion);

	    }
	  if(existTileDown())
	    {
	      CPyramidalRegion* downInfiniteRegion =
		static_cast<CInfinitePyramidalRegion*>(tileDown()->getInclusionTreeRoot());

	      downInfiniteRegion->setUp(newRegion);
	      newRegion->setDown(downInfiniteRegion);
	    }
	}

      newRegion->setGreyMin(m_regionFields[i].greyMin);
      newRegion->setGreyMax(m_regionFields[i].greyMax);
      newRegion->setLabel((CPyramidalRegion::Label)m_regionFields[i].label);

      if( m_regionFields[i].father != 0 )
	newRegion->setFather(m_mapRegions.find(m_regionFields[i].father)->second);

      if( m_regionFields[i].son != 0 )
	newRegion->setFirstSon(m_mapRegions.find(m_regionFields[i].son)->second);

      if( m_regionFields[i].brother != 0 )
	newRegion->setBrother(m_mapRegions.find(m_regionFields[i].brother)->second);

      if( m_regionFields[i].cc != 0 )
	newRegion->setNextSameCC(m_mapRegions.find(m_regionFields[i].cc)->second);

      assert( m_regionFields[i].representative != 0 );
      newRegion->setRepresentativeDart((m_mapDarts.find(m_regionFields[i].representative))->second);
    }
  //std::cout<<"[end] CTile::loadTree"<<std::endl;
}

void CTile::saveTree()
{
  //std::cout<<"[start] CTile::saveTree"<<std::endl;

  CPyramidalRegion* current = NULL;
  unsigned int count = 0;
  assert(getNbRegions()>0);
  m_regionFields = new SRegion[getNbRegions()];

  for( CDynamicCoverageAllRegions it(this); it.cont(); ++it)
    {
      if(!(*it)->isInfiniteRegion())
	{
	  current = static_cast<CPyramidalRegion*>(*it);

	  if(current->existUp())
	    m_regionFields[count].up = current->up()->id();
	  else if (m_first)
	    m_regionFields[count].up = 0;
	  else m_regionFields[count].up = m_oldRegions[count].up;


	  if(current->existDown())
	    m_regionFields[count].down = current->down()->id();
	  else if(m_first)
	    m_regionFields[count].down = 0;
	  else
	    m_regionFields[count].down = m_oldRegions[count].down;

	  m_regionFields[count].id            = current->id();
	  m_regionFields[count].firstPixel    = current->firstPixel();
	  m_regionFields[count].nbPixels      = current->getNbPixels();
	  m_regionFields[count].greySum       = current->getGreySum();
	  m_regionFields[count].greySquareSum = current->getGreySquareSum();
	  m_regionFields[count].greyMin       = current->getGreyMin();
	  m_regionFields[count].greyMax       = current->getGreyMax();
	  m_regionFields[count].label         = current->label();
	  m_regionFields[count].infinite      = false;
	}
      else
	{
	  current = static_cast<CInfinitePyramidalRegion*>(*it);
	  m_regionFields[count].id = 1;
	  m_regionFields[count].up = 1;
	  m_regionFields[count].down = 1;
	  m_regionFields[count].infinite = true;
	  m_regionFields[count].firstPixel    = CPoint2D(0,0);
	  m_regionFields[count].nbPixels      = 0;
	  m_regionFields[count].greySum       = 0;
	  m_regionFields[count].greySquareSum = 0;
	  m_regionFields[count].greyMin       = 0;
	  m_regionFields[count].greyMax       = 0;
	  m_regionFields[count].label         = CPyramidalRegion::Invalid;
	}


      if(current->existFather())
	m_regionFields[count].father = current->getFather()->id();
      else
	m_regionFields[count].father = 0;

      if(current->existSon())
	m_regionFields[count].son = current->getFirstSon()->id();
      else
	m_regionFields[count].son = 0;

      if(current->existBrother())
	m_regionFields[count].brother = current->getBrother()->id();
      else
	m_regionFields[count].brother = 0;

      if(current->existNextSameCC())
	m_regionFields[count].cc = current->getNextSameCC()->id();
      else
	m_regionFields[count].cc = 0;

      //static_cast<CPyramidalDart*>(current->getRepresentativeDart())->printInfos();
      assert( static_cast<CPyramidalDart*>(current->getRepresentativeDart())->id()!=0 );
      m_regionFields[count].representative = static_cast<CPyramidalDart*>(current->getRepresentativeDart())->id();

      ++count;
    }

  //std::cout<<" count = "<<count<<std::endl;
  //std::cout<<" nb regions = "<<getNbRegions()<<std::endl;
  assert(count == getNbRegions());
  //std::cout<<"[end] CTile::saveTree"<<std::endl;
}

void CTile::write()
{
  //std::cout<<"[start] CTile::write "<<" ("<<index(0)<<","<<index(1)<<","<<index(2)<<")"<<std::endl;
  assert(isOk());
  setFilename(index(0), index(1), index(2));

  unsigned int header[3];
  m_first = !fileExist(m_filename.c_str());

  // La tuile existait déjà, on récupère les infos du fichier
  // dans les structures "old"
  if ( !m_first )
    {
      FILE * file = NULL;
      file = fopen(m_filename.c_str(), "rb");
      assert(file!=NULL);

      UNUSED(fread(static_cast<void*>(header), sizeof(header), 1, file));

      m_oldProperties = new SProperties[1];
      UNUSED(fread( static_cast<void*>(m_oldProperties), sizeof(SProperties), 1, file ));

      m_oldDarts = new SDart[header[1]/sizeof(SDart)];
      unsigned int offset = sizeof(header)+sizeof(SProperties)+header[0];
      fseek(file, offset, SEEK_SET);
      UNUSED(fread( static_cast<void*>(m_oldDarts), header[1], 1, file ));

      m_oldRegions = new SRegion[header[2]/sizeof(SRegion)];
      UNUSED(fread( static_cast<void*>(m_oldRegions), header[2], 1, file ));

      fclose(file);
    }

  m_matrix = khalimsky()->getMatrix();
  header[0] = getKhalimskySizeX()*getKhalimskySizeY();
  header[1] = getNbDarts()*sizeof(SDart);
  header[2] = getNbRegions()*sizeof(SRegion);
  saveProperties();
  saveTopology();
  saveTree();

  if (!m_first)
    {
      delete [] m_oldDarts;      m_oldDarts=NULL;
      delete [] m_oldRegions;    m_oldRegions=NULL;
      delete [] m_oldProperties; m_oldProperties=NULL;
    }

  UNUSED(remove ( m_filename.c_str() ));
  FILE * pFile;
  pFile = fopen ( m_filename.c_str(), "wb" );
  assert(pFile!=NULL);

  UNUSED(fwrite( static_cast<void*>(header), sizeof(header), 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(m_properties), sizeof(SProperties), 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(m_matrix), header[0], 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(m_dartFields), header[1], 1, pFile ));
  UNUSED(fwrite( static_cast<void*>(m_regionFields), header[2], 1, pFile ));
  fclose (pFile);

  delete [] m_properties;   m_properties=NULL;
  delete [] m_dartFields;   m_dartFields=NULL;
  delete [] m_regionFields; m_regionFields=NULL;

  //std::cout<<"[end] CTile::write"<<std::endl;
}

void CTile::load(unsigned int Ai, unsigned int Aj, unsigned int Ak)
{
  //std::cout<<"[start] CTile::load "<<" ("<<Ai<<","<<Aj<<","<<Ak<<")"<<std::endl;
  setFilename(Ai,Aj,Ak);

  unsigned int header[3];
  FILE * file;
  file = fopen ( m_filename.c_str(), "rb" );
  if (file==NULL) {std::cout<<"CTile::load() : unable to open file "<<m_filename<<std::endl; assert(false);}

  // Lecture du header qui permet de récupérer les tailles
  UNUSED(fread( static_cast<void*>(header), sizeof(header), 1, file ));

  // Lecture des propriétés
  m_properties = new SProperties[1];
  UNUSED(fread( static_cast<void*>(m_properties), sizeof(SProperties), 1, file ));

  loadProperties();
  //printPropertiesStruct(m_properties);

  // Lecture de la matrice
  CKhalimsky* khalimsky = new CKhalimsky(width(), height());
  m_matrix = khalimsky->getMatrix();
  //on s'assure que la taille de la matrice est équivalente à celle lue sur le disque
  assert( (width()+1)*(height()+1)==header[0] );
  UNUSED(fread( static_cast<void*>(m_matrix), header[0], 1, file ));

  khalimsky->setMatrix(m_matrix);
  setKhalimsky(khalimsky);

  // Lecture des brins
  m_dartFields = new SDart[m_properties->nbDarts];
  UNUSED(fread( static_cast<void*>(m_dartFields), header[1], 1, file ));

  loadEmptyTopology();

  // Lecture des régions
  m_regionFields = new SRegion[m_properties->nbRegions];
  UNUSED(fread( static_cast<void*>(m_regionFields), header[2], 1, file ));

  loadEmptyTree();

  fclose(file);

  loadTopology();
  loadTree();

  delete [] m_dartFields;   m_dartFields = NULL;
  delete [] m_regionFields; m_regionFields = NULL;
  delete [] m_properties;   m_properties = NULL;

  assert(isOk());

  //std::cout<<"[end] CTile::load"<<std::endl;
}
