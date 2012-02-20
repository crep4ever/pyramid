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
#include "pyramid.hh"
#include "imageTiff.hh"
#include "tile.hh"
#include "region.hh"
#include "pyramidal-dart.hh"
#include "merge-grey.hh"

#include INCLUDE_NON_INLINE("pyramid.icc")

#define UNUSED(x) (void)x

using namespace Map2d;
using namespace pyramid;

//******************************************************************************
//Constructeurs
//******************************************************************************

CPyramid::CPyramid():
  m_image(NULL),
  m_tileWidth(512),
  m_tileHeight(512),
  m_nbLevels(5),
  m_extractMode( ConstantTileSize ),
  m_segmentationMode( Histology ),
  m_projectionMode( DirectProjection ),
  m_focusAttentionMode( FocusHisto ),
  m_detectFictiveBordersMode( DetectionOff ),
  m_tileTotal(0),
  m_tileCounter(0),
  m_levelCounter(0),
  m_maxMemory(0)
{ }

CPyramid::CPyramid(const std::string & AFilename):
  m_image(NULL),
  m_tileWidth(32),
  m_tileHeight(32),
  m_nbLevels(0),
  m_extractMode( ConstantTileNumber ),
  m_segmentationMode( Thresholds ),
  m_projectionMode( DirectProjection ),
  m_focusAttentionMode( FocusGrey ),
  m_detectFictiveBordersMode( DetectionOff ),
  m_tileTotal(0),
  m_tileCounter(0),
  m_levelCounter(0),
  m_maxMemory(0)
{
  init();
  importImTiff(AFilename);
}

CPyramid::~CPyramid()
{
  //std::cout<<"[destructeur] CPyramid \n";
  //for(uint i=0; i<nbLevels(); ++i)
  //  level(i)->unloadAllTiles();

  for(uint i=0; i<nbLevels(); ++i)
    delete level(i);
  m_levels.clear();

  if(m_image->isOpen) m_image->close();
  delete m_image; m_image=NULL;
}

//******************************************************************************
// Construction de la pyramide
//******************************************************************************

void CPyramid::build()
{
  //std::cout<<"[start] CPyramid::buildPyramid \n";
  print();
  m_image->open();

  for(uint k=0; k<nbLevels(); ++k)
    addLevel();

  level(0)->createTopLevel();
  for(uint k=1; k<nbLevels(); ++k)
    level(k)->extract((ExtractMode) extractMode(),
		      (SegmentationMode) segmentationMode(),
		      (ProjectionMode) projectionMode(),
		      (FocusAttentionMode) focusAttentionMode(),
		      (DetectFictiveBordersMode) detectFictiveBordersMode());

  m_image->unload();
  std::cout<<std::endl<<"success"<<std::endl;
  //std::cout<<"[end] CPyramid::buildPyramid \n";
}

void CPyramid::addLevel()
{
  //std::cout<<" [start] CPyramid::addLevel"<<std::endl;
  CLevel* lvl = new CLevel();
  CLevel* last = NULL;
  if(!levels().empty())
    last = level(levels().size()-1);

  lvl->setImage(image());
  lvl->setDepth(levels().size());
  lvl->setMergeThreshold( mergeThresholds()[lvl->depth()] );

  //hiérarchie des niveaux
  lvl->setLevelUp(last);
  if(last)
    last->setLevelDown(lvl);

  if( extractMode() == ConstantTileNumber )
    {
      //nombre de tuiles constant par niveau
      lvl->setTileWidth( tileWidth()*ratios(lvl->depth()).x() );
      lvl->setTileHeight( tileHeight()*ratios(lvl->depth()).y() );
    }
  else if( extractMode() == ConstantTileSize )
    {
      //taille de tuile constante
      lvl->setTileWidth(tileWidth());
      lvl->setTileHeight(tileHeight());
    }
  else
    {
      std::cout<<"CPyramid::addLevel Warning extract mode unsupported"<<std::endl;
    }
  m_levels.push_back(lvl);
  //std::cout<<" [end] CPyramid::addLevel"<<std::endl;
}

CImageTiff* CPyramid::importImTiff(const std::string & filename)
{
  CImageTiff* tif = new CImageTiff(filename);
  assert(tif!=NULL && tif->getNbPages() > 0);
  //tif = tif->tilePages();
  //tif->sortPages();
  //tif = tif->interpolatePagesWithMagick();
  //tif->sortPages();
  setImage(tif);
  return tif;
}

void CPyramid::linkTilesUpDown()
{
  for(uint k=1;k<nbLevels()-1; ++k)
    for(uint j=0;j<nbTilesHeight(k);++j)
      for(uint i=0;i<nbTilesWidth(k);++i)
	{
	  CTile* res =  level(k)->loadTile( CPoint2D(i,j) );
	  std::deque<CTile*>::iterator it2;

	  //si la tuile est chargée, on les relie, sinon, on fait rien
	  //TileUp
	  if(res->properties()->up != 0)
	    for(it2=level(k-1)->tiles().begin(); it2!=level(k-1)->tiles().end(); ++it2)
	      if((*it2)->id() == res->properties()->up)
		{
		  linkTileUpDown(*it2,res);
		  break;
		}

	  //TileDown
	  if(res->properties()->down != 0)
	    for(it2=level(k+1)->tiles().begin(); it2!=level(k+1)->tiles().end(); ++it2)
	      if((*it2)->id() == res->properties()->down)
		{
		  linkTileUpDown(res,*it2);
		  break;
		}
	  delete [] res->properties();
	  res->setProperties(0);
	}
}


//******************************************************************************
// Méthodes système
//******************************************************************************


void CPyramid::open(const std::string & APath)
{
  std::ostringstream command;
  UNUSED(system("rm -rf output//* ;"));
  command<<"cp -R "<<APath<<" .//output ;";
  UNUSED(system(command.str().c_str()));

  importImTiff(".//output//image.tif");
  loadAllTiles();

  //std::cout<<" taille de la pyramide ouverte = "<<m_tiles.size()<<" tuiles."<<std::endl;
}

void CPyramid::copyImageInWorkingDirectory(const std::string & AImagePath, const std::string & AImageName)
{
  // AImageName est bien le nom original de l'image
  // juste que le open ne le connait pas...
  std::ostringstream command;
  command<<"cp "<<AImagePath<<" .//output//image.tif ;";

  UNUSED(system(command.str().c_str()));
}

void CPyramid::save(const std::string & APath)
{
  std::ostringstream command;
  command<<"cp .//output//* "<<APath<<" ;";
  UNUSED(system(command.str().c_str()));
}

void CPyramid::clear()
{
  UNUSED(system("rm -rf output//* ;"));
}

void CPyramid::init()
{
  clear();
  mkdir("output", 00755);
}

void CPyramid::exportData()
{
  UNUSED(system("rm -rf .//result//* ; "));
  UNUSED(system("mv .//output// .//result// ;"));
}


//******************************************************************************
// Opérations de contrôle et vérification
//******************************************************************************


unsigned long int CPyramid::getMemoryForPyramid() const
{
  //std::cout<<"size of CPyramid : "<<sizeof(CPyramid)<<std::endl;
  return sizeof(CPyramid);
}

void CPyramid::printInfosMemory() const
{
  /*
    std::cout<<" ************************************************************* \n";
    std::cout<<"\n Memory: \n"<<" Pyramide: "<<getMemoryForPyramid()<<"\n";

    // On parcours toutes les tuiles chargées en mémoire
    //std::vector<CTile*>::iterator it;
    std::deque<CTile*>::iterator it;
    uint total = getMemoryForPyramid();
    for( it = m_tiles.begin(); it != m_tiles.end(); ++it )
    {
    (*it)->printInfosMemory();
    total = total + (*it)->getMemoryForTile() +
    (*it)->getMemoryForMap() +
    (*it)->getMemoryForInclusionTree() +
    (*it)->getMemoryForKhalimsky();
    }

    std::cout<<"\n Total : "<<total;
    std::cout<<"\n ************************************************************* \n";
  */
}

uint CPyramid::getMemoryForLocalPyramid() const
{
  /*
  // On parcours toutes les tuiles chargées en mémoire
  std::deque<CTile*>::iterator it;
  uint total = getMemoryForPyramid();

  for( it = m_tiles.begin(); it != m_tiles.end(); ++it )
  {
  total = total + (*it)->getMemoryForTile() +
  (*it)->getMemoryForMap() +
  (*it)->getMemoryForInclusionTree() +
  (*it)->getMemoryForKhalimsky();
  }

  //std::cout<<" Nb de tuiles en mémoire: "<<m_tiles.size()<<" Memory: "<<total<<"\n";
  return total;
  */
  return 0;
}

uint CPyramid::nbDarts()
{
  unloadAllTiles();
  uint total = 0;
  for(it=m_levels.begin();it!=m_levels.end();++it)
    total+=(*it)->nbDarts();
  return total;
}

uint CPyramid::nbRegions()
{
  unloadAllTiles();
  uint total = 0;
  for(it=m_levels.begin();it!=m_levels.end();++it)
    total+=(*it)->nbRegions();
  return total;
}

void CPyramid::totalMemoryRequired() const
{
  /*
  //std::cout<<" ************************************************************* \n";
  uint total = 0;
  uint count = 0;
  CTile* tmpTile = NULL;

  for(uint k=0; k<getNbLevels(); ++k)
  for(uint i=0; i<getNbTilesWidth(k); ++i)
  for(uint j=0; j<getNbTilesHeight(k); ++j)
  {
  ++count;
  tmpTile = loadTile(i,j,k);
  assert(tmpTile!=NULL);
  total = total + (tmpTile)->getMemoryForTile() +
  (tmpTile)->getMemoryForMap() +
  (tmpTile)->getMemoryForInclusionTree() +
  (tmpTile)->getMemoryForKhalimsky();
  unloadTile(i,j,k);
  }
  std::cout<<" Nb de tuiles dans la pyramide: "<<count<<" Memory: "<<total<<"\n";
  //std::cout<<" ************************************************************* \n";
  */
}

/*
  bool CPyramid::isPyramidOk(){

  //cout<<"  [start] CPyramid::isPyramidOk()"<<std::endl;
  bool result = true;
  //cout<<"  [end] CPyramid::isPyramidOk()"<<std::endl;
  return result;

  }
*/

void CPyramid::print()
{
#ifdef DEBUG_PYRAMID
  std::cout<<" **** Pyramide infos ****"<<std::endl;
  std::cout<<" m_tileWidth = "      <<m_tileWidth<<std::endl;
  std::cout<<" m_tileHeight = "     <<m_tileHeight<<std::endl;
  std::cout<<" segmentation = "    <<m_segmentationMode<<std::endl;
  std::cout<<" projection = "      <<m_projectionMode<<std::endl;
  std::cout<<" focus = "           <<m_focusAttentionMode<<std::endl;
  std::cout<<" construction = "    <<m_extractMode<<std::endl;
  std::cout<<" fictive borders = " <<m_detectFictiveBordersMode<<std::endl;
#endif
}

