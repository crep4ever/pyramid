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
#include "svg-export.hh"
#include "svg-creator.hh"
#include <deque>
#include "pyramid.hh"
#include "tile.hh"
#include "level.hh"
#include "region.hh"
#include "pyramidal-dart.hh"
#include "traversal-region-pixels.hh"
#include "vertex.hh"
using namespace Map2d;
//******************************************************************************
void CSvgExporter::drawPyramid(CPyramid* APyramid, bool APerspective, bool ATopology, bool AGeometry)
{
  std::ostringstream filename;
  filename<<".//output//pyramid.svg";
  std::ofstream ofs( filename.str().c_str() );  
  CSvgCreator svg( ofs );
  svg.svgBegin( (APyramid->imageWidth(APyramid->nbLevels()-1)+50)*(APyramid->nbLevels()), 
		(APyramid->imageHeight(APyramid->nbLevels()-1)+50)*2 );
  if(APerspective)
    svg.groupBegin("transform=\"translate(300,0)\"");
  
  for( uint i=1; i<APyramid->nbLevels(); ++i)
    {
      std::ostringstream translate;
      translate<<"transform=\"translate(0,"
	       <<i*(APyramid->imageHeight(i)+30)
	       <<")\" ";
      
      svg.groupBegin(translate.str());
      
      if(APerspective)
	svg.groupBegin("transform=\"skewX(-30)\"");
    
      if(ATopology)
	addSvgPyramidTopology(APyramid, svg, i);

      if(AGeometry)
	addSvgPyramidGeometry(APyramid, svg, i);
      
      svg.groupEnd();
 
      if(APerspective)
	svg.groupEnd();
    }

  if(APerspective)
    svg.groupEnd();

  svg.svgEnd();
  ofs.close();
}

//------------------------------------------------------------------------------
void CSvgExporter::drawPyramidLevels(CPyramid* APyramid, bool ATopology, bool AGeometry)
{
  for( uint i=1; i<APyramid->nbLevels(); ++i)
    {
      std::ostringstream filename;
      std::string sep = "-";
      filename<<".//output//pyramid"<<sep<<"level"<<sep<<i<<".svg";
    
      std::ofstream ofs( filename.str().c_str() );
      
      CSvgCreator svg( ofs );
      svg.svgBegin( APyramid->imageHeight(0), APyramid->imageWidth(0) );
      
      if(ATopology)
	addSvgPyramidTopology(APyramid, svg, i);

      if(AGeometry)
	addSvgPyramidGeometry(APyramid, svg, i);

      svg.svgEnd();
      ofs.close();
    }
}

//------------------------------------------------------------------------------
void CSvgExporter::drawPyramidTopology(CPyramid* APyramid)
{
  std::ostringstream filename;
  std::string sep = "-";
  
  for( uint i=0; i<APyramid->nbLevels(); ++i)
    {
      filename<<".//output//pyramid"<<sep<<"topology"<<sep<<"level"<<i<<".svg";
      std::ofstream ofs( filename.str().c_str() );
      
      CSvgCreator svg( ofs );
      svg.svgBegin( APyramid->imageHeight(0), APyramid->imageWidth(0) );
      addSvgPyramidTopology(APyramid, svg, i);
      svg.svgEnd();
      ofs.close();
    }
}

//------------------------------------------------------------------------------
void CSvgExporter::drawPyramidGeometry(CPyramid* APyramid)
{
  std::ostringstream filename;
  std::string sep = "-";
  
  for( uint i=0; i<APyramid->nbLevels(); ++i)
    {
      filename<<".//output//pyramid"<<sep<<"geometry"<<sep<<"level"<<i<<".svg";
      
      std::ofstream ofs( filename.str().c_str() );
      
      CSvgCreator svg( ofs );
      svg.svgBegin( APyramid->imageHeight(0), APyramid->imageWidth(0) );
      addSvgPyramidGeometry(APyramid, svg, i);
      svg.svgEnd();
      ofs.close();
    }
}

//------------------------------------------------------------------------------
void CSvgExporter::addSvgPyramidTopology(CPyramid* APyramid, CSvgCreator& svg, uint ADepth)
{
  std::deque<CTile*>::iterator it;
  std::deque<CTile*> tiles;
  CLevel* lvl = APyramid->FLevels[ADepth];
  
  for( it = lvl->tiles().begin(); it!=lvl->tiles().end(); ++it)
    {
      std::ostringstream translate;
      translate<<"transform=\"translate("<<(*it)->xmin()<<","<<(*it)->ymin()<<")\" ";
      svg.groupBegin(translate.str());
      addSvgMapTopology((*it), svg);
      svg.groupEnd();
    }
}

//------------------------------------------------------------------------------
void CSvgExporter::addSvgPyramidGeometry(CPyramid* APyramid, CSvgCreator& svg, uint ADepth)
{
  std::deque<CTile*>::iterator it;
  std::deque<CTile*> tiles;
  CLevel* lvl = APyramid->FLevels[ADepth];
  
  for( it = lvl->tiles().begin(); it!=lvl->tiles().end(); ++it)
    {
      std::ostringstream translate;
      translate<<"transform=\"translate("
	       <<(*it)->xmin()
	       <<","
	       <<(*it)->ymin()
	       <<")\"";
      
      svg.groupBegin(translate.str());
      addSvgMapGeometry((*it), svg);
      svg.groupEnd();
    }
}

//------------------------------------------------------------------------------
void CSvgExporter::drawTile(CTile* ATile)
{
  std::ostringstream filename;
  std::string sep = "-";
  
  filename<<".//output//svg"<<sep<<"tile"<<sep<<ATile->id()<<".svg";
  std::ofstream ofs( filename.str().c_str() );

  CSvgCreator svg( ofs );
  svg.svgBegin( ATile->height(), ATile->width() );
  addSvgMapTopology(ATile, svg);
  addSvgMapGeometry(ATile, svg);
  svg.svgEnd();
  ofs.close();
}

//------------------------------------------------------------------------------
void CSvgExporter::drawTileTopology(CTile* ATile)
{
  std::ostringstream filename;
  std::string sep = "-";
 
  filename<<".//output//map-topology"<<sep<<ATile->id()<<".svg";
  std::ofstream ofs( filename.str().c_str() );

  CSvgCreator svg( ofs );
  svg.svgBegin( ATile->height(), ATile->width() );
  addSvgMapTopology(ATile, svg);
  svg.svgEnd();
  ofs.close();
}

//------------------------------------------------------------------------------
void CSvgExporter::drawTileGeometry(CTile* ATile)
{
  std::ostringstream filename;
  std::string sep = "-";
 
  filename<<".//output//map-geometry"<<sep<<ATile->id()<<".svg";
  std::ofstream ofs( filename.str().c_str() );

  CSvgCreator svg( ofs );
  svg.svgBegin( ATile->height(), ATile->width() );
  addSvgMapGeometry(ATile, svg);
  svg.svgEnd();
  ofs.close();
}

//------------------------------------------------------------------------------
void CSvgExporter::addSvgMapTopology(CTile* ATile, CSvgCreator& svg)
{
  for( CDynamicCoverageAll it(ATile); it.cont(); ++it)
    addSvgDart(ATile, *it, svg);
  
  //TODO des booleens pour mettre ces deux operations en options
  //drawFirstPixels(AMap, svg);
  //fillRegions(AMap, svg);
}

//------------------------------------------------------------------------------
void CSvgExporter::addSvgMapGeometry(CTile* ATile, CSvgCreator& svg)
{
  // Parcours de la matrice de Khalimsky
  // Affichage des pointels et des lignels allumés
  CDoublet temp;
  uint width  = ATile->width();
  uint height = ATile->height();

  for(uint x=0; x<width+1; ++x)
    for(uint y=0; y<height+1; ++y)
      {
	temp.setX(x);
	temp.setY(y);
	
	// Affichage des pointels
	if( ATile->isPCell(temp) )
	  svg.circle(x,y,0.05, "class=\"pointel\" " );
	
	// Affichage des lignels
	temp.setLinel(XPOS);
	if( ATile->isLCell(temp) )
	  svg.line(x+0.2,y, x+0.8,y,"class=\"linel\" " );
	
	temp.setLinel(YPOS);
	if( ATile->isLCell(temp) )
	  svg.line(x,y+0.2, x,y+0.8, "class=\"linel\" ");
      }
}

//------------------------------------------------------------------------------
void CSvgExporter::drawRegion(CTile* AMap, CRegion* ARegion)
{
  assert(ARegion!=NULL);
  std::ostringstream filename;
  std::string sep = "-";
  
  filename<<".//output//region"<<sep<<ARegion->getId()<<".svg";
  std::ofstream ofs( filename.str().c_str() );
  
  CSvgCreator svg( ofs );
  svg.svgBegin( 5000, 5000 );
  addSvgRegion(AMap, ARegion, svg);
  svg.svgEnd();
  ofs.close();
}

//------------------------------------------------------------------------------
void CSvgExporter::drawDart(CTile* AMap, CDart* ADart)
{
  assert(ADart!=NULL);
  std::ostringstream filename;
  std::string sep = "-";
  
  filename<<".//output//brin"<<sep<<static_cast<CPyramidalDart*>(ADart)->getId()<<".svg";
  std::ofstream ofs( filename.str().c_str() );
  
  CSvgCreator svg( ofs );
  svg.svgBegin( 5000, 5000 );
  addSvgDart(AMap, ADart, svg);
  svg.svgEnd();
  ofs.close();
}

//------------------------------------------------------------------------------
CVertex CSvgExporter::calculateGap(const CVertex & APoint, const CDoublet & current, const CDoublet & next )
{
  float gap = 0.2;
  float gapx = 0;
  float gapy = 0;
  
  if( current.getPrevLinel().getLinel() == XPOS ||
      next.getPrevLinel().getLinel() == XPOS )
    gapx = gap;
  
  else if( current.getPrevLinel().getLinel() == XNEG ||
	   next.getPrevLinel().getLinel() == XNEG )
    gapx = -gap;
    
  if( current.getPrevLinel().getLinel() == YPOS ||
      next.getPrevLinel().getLinel() == YPOS )
    gapy = gap;
  
  else if( current.getPrevLinel().getLinel() == YNEG ||
	   next.getPrevLinel().getLinel() == YNEG )
    gapy = -gap;
  
  return CVertex(APoint.getX()+gapx, APoint.getY()+gapy);
}

//------------------------------------------------------------------------------
void CSvgExporter::addSvgDart(CTile* AMap, CDart* ADart, CSvgCreator& svg)
{
  CDoublet init = AMap->getDoublet(ADart);
  CDoublet current(init);
  CDoublet next(init);
  CDoublet prev(init);

  // On récupère le doublet précédent
  do{
    prev = prev.getPrevLinel();
  }
  while(!AMap->isLCell(prev));
  prev.setNextPointel();

  // On démarre le chemin
  CVertex startPoint = calculateGap(CVertex(current.getX(), current.getY()), prev, current);

  if( AMap->isRepresentativeDart(ADart) )
    svg.pathBegin(startPoint.getX(),startPoint.getY(),
		  "class=\"representativeDart\" marker-end=\"url(#arrow)\" ");
  else
    svg.pathBegin(startPoint.getX(),startPoint.getY(),
		  "class=\"dart\" marker-end=\"url(#arrow)\"  ");

  // Parcours de l'arête
  do{
    // Calcul du doublet suivant
    next.setNextPointel();
    do{
      next = next.getNextLinel();
    }
    while(!AMap->isLCell(next));

    // On dessine si besoin en rajoutant le point au path
    if(AMap->isPCell(next) || next == init)
      {
	CVertex endPoint = calculateGap( CVertex(next.getX(),next.getY()), 
					 current, next );
	svg.pathLineTo(endPoint.getX(), endPoint.getY());
	break;
      }
    else if( current.getLinel() != next.getLinel())
      {
	CVertex endPoint = calculateGap( CVertex(next.getX(),next.getY()), 
					 current, next );
	svg.pathLineTo(endPoint.getX(), endPoint.getY());
	current = next;
      }
  }
  while ( 1 );
  svg.pathEnd();
}

//------------------------------------------------------------------------------
void CSvgExporter::addSvgRegion(CTile* AMap, CRegion* ARegion, CSvgCreator& svg)
{
  assert(ARegion!=NULL);
  
  for( CDynamicCoverage1 it(AMap, ARegion->getRepresentativeDart()); it.cont(); ++it)
    addSvgDart(AMap, *it, svg);
  
  // bords internes
  CRegion* region = ARegion;
  CRegion* firstSon = NULL;
  CRegion* brother = NULL;
  
  if( region->existSon())
    {
      firstSon = region->getFirstSon();
      
      for( CDynamicCoverage1 it( AMap, AMap->beta2(firstSon->getRepresentativeDart()) ); it.cont() ; ++it )
	addSvgDart(AMap, *it, svg);
      
      region = firstSon;
      
      while( region->existBrother() )
	{
	  brother = region->getBrother();
	  for( CDynamicCoverage1 it( AMap, AMap->beta2(brother->getRepresentativeDart()) ) ; it.cont() ; ++it )
	    addSvgDart(AMap, *it, svg);
	}
      region = brother;
    }
}

//------------------------------------------------------------------------------
void CSvgExporter::drawFirstPixels(CTile* ATile, CSvgCreator& svg)
{
  //std::cout<<"[start] drawFirstPixels "<<std::endl;
  
  // Parcours de toutes les régions de l'arbre d'inclusion
  for( CDynamicCoverageAllRegions it(ATile); it.cont(); ++it)
    for( CTraversalRegionPixels it2(ATile, static_cast<CPyramidalRegion*>(*it)); it2.cont(); ++it2)
      if( (*it2) == static_cast<CPyramidalRegion*>(*it)->getFirstPixel() && !(*it)->isInfiniteRegion() )
	{
	  svg.rect((*it2).getX()+0.3, (*it2).getY()+0.3, 0.4, 0.4, "class=\"fpixel\" ");
	  break;
	}
  
  //std::cout<<"[end] drawFirstPixels"<<std::endl;
}

//------------------------------------------------------------------------------
void CSvgExporter::fillRegions(CTile* AMap, CSvgCreator& svg)
{
  //std::cout<<"[start] fillPixels "<<std::endl;
  // Parcours de toutes les régions de l'arbre d'inclusion
  for( CDynamicCoverageAllRegions it(AMap); it.cont(); ++it)
    {
      if(!(*it)->isInfiniteRegion())
	{
	  std::stringstream ss;
	  ss << std::hex << static_cast<CPyramidalRegion*>(*it)->getAverageGrey();
	  std::string value = ss.str();
	  for( CTraversalRegionPixels it2(AMap, static_cast<CPyramidalRegion*>(*it)); it2.cont(); ++it2)
	    {
	      std::ostringstream color;
	      color<<"fill=\"#"<<value<<value<<value<<"\" ";
	      svg.rect((*it2).getX()+0.3, (*it2).getY()+0.3, 0.4, 0.4, color.str() );
	    }
	}
    }
  //std::cout<<"[end] fillRegions"<<std::endl;
}
