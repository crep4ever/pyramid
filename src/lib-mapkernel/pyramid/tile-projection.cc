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
#include "heap.hh"
#include "dk-doublet.hh"
#include <algorithm>
#include <limits>

#include INCLUDE_NON_INLINE("tile-projection.icc")
using namespace Map2d;
using namespace pyramid;

//------------------------------------------------------------------------------
void CTile::projectionStandard()
{
  //std::cout<<"\n[start] CTile::projectionStandard"<<std::endl;
  CDoublet up; CDoublet down; CPoint2D parent; CPoint2D son;

  uint ratiox = upRatioX();
  uint ratioy = upRatioY();

  const CPoint2D start = coordinateInParent(0,0,Relative);
  const CPoint2D end = coordinateInParent(khalimsky()->getSizeX(),khalimsky()->getSizeY(),Relative);

  for(uint y=start.y(); y<end.y(); ++y)
    for(uint x=start.x(); x<end.x(); ++x)
      {
	up.setPointel(x, y);
	son = coordinateFromParent(x,y,Relative);
	if( tileUp()->isPCell(up) )
	  {
	    down.setPointel(son.x(),son.y());
	    setPCell(down, true);
	  }
	up.setLinel(XPOS);
	if( tileUp()->isLCell(up) )
	  {
	    down.setDoublet(son.x(), son.y(), XPOS);
	    setLCell(down, true);
	    for(uint i=1; i<ratiox; ++i)
	      {
		down.incX();
		//std::cout<<" down = " << down<<std::endl;
		setLCell(down, true);
	      }
	  }
	up.setLinel(YPOS);
	if( tileUp()->isLCell(up) )
	  {
	    down.setDoublet(son.x(), son.y(), YPOS);
	    setLCell(down, true);
	    for(uint i=1; i<ratioy; ++i)
	      {
		down.incY();
		setLCell(down, true);
	      }
	  }
      }
  //std::cout<<"\n[end] CTile::projectionStandard()"<<std::endl;
}

//------------------------------------------------------------------------------
void CTile::projectionDijkstraConstant()
{
  //std::cout<<"\n[start] CTile::projectionDijkstraConstant"<<std::endl;

  CTile* up = tileUp();
  int done = up->getNewMark();
  std::vector< CEdge > edges;
  CPoint2D son;
  CDoublet down;
  uint ratiox = upRatioX();  uint ratioy = upRatioY();
  for(CDynamicCoverageAll it(up); it.cont(); ++it)
    {
      CPyramidalDart* dart = static_cast<CPyramidalDart*>(*it);
      CDoublet init = up->doublet(dart);
      son = CPoint2D(init.getX()*ratiox, init.getY()*ratioy);
      down.setDoublet(son.x(), son.y(), init.getLinel());
      addNewEdge(edges);
      if( !up->isMarked(dart,done) && !getRegion(dart)->isInfiniteRegion() && !getRegion(beta2(dart))->isInfiniteRegion() )
	{
	  CDoublet next(init);
	  addDoublet(edges, next);

	  //parcours de l'arête parente
	  do{
	    next.setNextPointel();
	    do
	      next.setNextLinel();
	    while(!up->isLCell(next));
	    addDoublet(edges, next);

	    if(up->isPCell(next) || next == init)
	      break;

	  }
	  while(1);

	  for(uint i=0; i<edges.size(); ++i)
	    projectPath(edges[i]);
	  edges.clear();

	  up->setMark(dart, done);
	  up->setMark(beta2(dart), done);
	}
    }
  up->negateMaskMark(done);
  up->freeMark(done);

  setId(++(up->FCount));
  //std::cout<<"\n[end] CTile::projectionDijkstraConstant"<<std::endl;
}


//------------------------------------------------------------------------------
void CTile::projectionDijkstra()
{
  //std::cout<<"\n[start] CTile::projectionDijkstra"<<std::endl;
  //todo: on stocke le path dans une map avec le pointeur du brin up
  //et comme ça on évite la matrice lignel/dart de l'extraction

  CTile* up = tileUp();
  int done = up->getNewMark();
  std::vector< CEdge > edges;
  CPoint2D son;
  CDoublet down;
  for(CDynamicCoverageAll it(up); it.cont(); ++it)
    {
      CPyramidalDart* dart = static_cast<CPyramidalDart*>(*it);
      CDoublet init = up->doublet(dart);
      son = coordinateFromParent(init, Relative);
      down.setDoublet(son.x(), son.y(), init.getLinel());
      addNewEdge(edges);
      if( !up->isMarked(dart,done) && !isLinelOnBorder(down,Relative))
	{
	  CDoublet next(init);
	  if(isDoubletInTile(down, true, Relative))
	    addDoublet(edges, next);

	  //parcours de l'arête parente
	  do{
	    next.setNextPointel();
	    do
	      next.setNextLinel();
	    while(!up->isLCell(next));
	    son = coordinateFromParent(next, Relative);
	    down.setDoublet(son.x(), son.y(), next.getLinel());
	    if(isDoubletInTile(down, false, Relative))
	      addDoublet(edges, next);

	    if(up->isPCell(next) || next == init)
	      break;

	    if(isDoubletInTile(down, false, Relative))
	      {
		if(isPointelOnBorder(down, Relative))
		  {
		    addNewEdge(edges);
		    assert(edges.back().empty());
		  }
		else if (isPointelOnBorder(down, Relative) && !isPointelACorner(down,Relative))
		  {
		    addDoublet(edges, next);
		  }
	      }
	  }
	  while(1);

	  for(uint i=0; i<edges.size(); ++i)
	    projectPath(edges[i]);
	  edges.clear();

	  up->setMark(dart, done);
	  up->setMark(beta2(dart), done);
	}
    }
  up->negateMaskMark(done);
  up->freeMark(done);

  setId(++(up->FCount));
  //std::cout<<"\n[end] CTile::projectionDijkstra"<<std::endl;
}


//------------------------------------------------------------------------------
uint CTile::projectPath(const CEdge & AEdge)
{
  //std::cout<<"[start] CTile::projectPath"<<std::endl;
  if(AEdge.empty()) return 0;

  //std::cout<<" arête = ";
  //for(uint i=0; i<AEdge.size(); ++i)
  //  std::cout<<AEdge[i] << " ";
  //std::cout<<std::endl;

  //Dijkstra
  Heap<CDkDoublet*> nodes;
  CDkDoublet* n1, *n2, *last;
  last = findNodes(AEdge, nodes);
  if(!last) return 0;
  std::vector<CDkDoublet*> neighbors;
  while(!nodes.empty())
    {
      n1 = nodes.top(); //retourne le noeud de poids minimum
      if( n1->weight()==std::numeric_limits<float>::max() )
	break;
      nodes.pop();
      neighbors = findNeighbors(n1, nodes);
      for(uint i=0; i<neighbors.size(); ++i)
	{
	  n2 = neighbors[i];
	  float alt = n1->weight() + weight(n1, n2);
	  if( alt < n2->weight() )
	    {
	      n2->updateWeight(alt, nodes);
	      n2->setPrev(n1);
	    }
	}
    }

  //dessin du chemin minimal dans la matrice
  CDkDoublet* node = last;
  CDkDoublet* next;
  setPCell(*node, true);
  assert(isPCell(*last));
  // std::cout<<"\n dijktra pointel1 = "<<(CDoublet) *last<<std::endl;
  while(node->prev())
    {
      next = node;
      node = node->prev();
      //On positionne correctement le lignel
      if( node->getX() == next->getX()-1)
	node->setLinel(XPOS);
      else if( node->getX() == next->getX()+1)
	node->setLinel(XNEG);
      else if( node->getY() == next->getY()-1)
	node->setLinel(YPOS);
      else if( node->getY() == next->getY()+1)
	node->setLinel(YNEG);
      else
	std::cout<<"CTile::projectPath : Warning node is not a neighbor of next"<<std::endl;

      setLCell(*node, true);
    }
  setPCell(*node, true);

  //std::cout<<"[end] CTile::projectPath"<<std::endl;
  return 0;
}
//------------------------------------------------------------------------------
CDkDoublet* CTile::findNodes(const CEdge & AEdge, Heap<CDkDoublet*> & AHeap)
{
  if(AEdge.size()==1)
    {
      CPoint2D son = coordinateFromParent( AEdge[0], Relative );
      CDoublet tmp;
      tmp.setDoublet(son.x(), son.y(), AEdge[0].getLinel());
      if(isLinelOnBorder(tmp,Relative))
	return 0;
    }

  //std::cout<<"[start] CTile::findNodes"<<std::endl;

  //std::cout<<" arête = ";
  //for(uint i=0; i<AEdge.size(); ++i)
  //  std::cout<<AEdge[i] << " ";
  //std::cout<<std::endl;

  //tous les doublets à prendre en compte lors de la recherche de chemin minimal
  //entre ADoublet1 et ADoublet2 (ADoublet1<ADoublet2)
  std::vector<CDoublet> tmp;
  CDkDoublet* current = NULL;
  CDkDoublet* last = NULL;
  // Le graphe sur lequel on va appliquer Dijkstra est composé
  // de tous les pointels potentiels
  //sauf que le premier et le dernier pointel ne doivent pas être déplacés
  //on ne calcule pas le chemin depuis le premier mais depuis le deuxieme pointel
  //car il faut conserver la direction imposée par le premier lignel

  //on impose le premier pointel et le premier lignel
  CDoublet start, end;
  CPoint2D son = coordinateFromParent( AEdge[0], Relative );
  start.setDoublet(son.x(), son.y(), AEdge[0].getLinel());
  setPCell(start, true);
  setLCell(start, true);

  //on impose le dernier pointel et le dernier lignel
  son = coordinateFromParent( AEdge[AEdge.size()-1], Relative );

  end.setDoublet(son.x(), son.y(), AEdge[AEdge.size()-1].getLinel());
  setPCell(end, true);

  //La direction du dernier lignel à conserver est donnée par l'avant dernier doublet up
  CDoublet beforelast (end);
  if(AEdge.size()==1)
    beforelast.setLinel( CDoublet::getOppositeLinel( AEdge[0].getLinel() ) );
  else
    beforelast.setLinel( CDoublet::getOppositeLinel( AEdge[AEdge.size()-2].getLinel() ) );
  setLCell(beforelast, true);


  bool loop = start.samePointel(end);
  uint size = (loop)?AEdge.size()-1:AEdge.size();
  bool firstNodeSet = false;
  bool lastNodeSet = false;
  if(loop)
    {
      CDkDoublet* extrem1 = new CDkDoublet(start.getNextPointel());
      extrem1->setWeight(0);
      AHeap.push(extrem1);
      firstNodeSet = true;
      last = new CDkDoublet(beforelast.getNextPointel());
      AHeap.push(last);
      lastNodeSet = true;
    }

  for(uint i=0; i<size; ++i)
    {
      tmp = reductionWindow(AEdge[i]);
      // On push tous les pointels dans le tas
      for(uint j=0; j<tmp.size(); ++j)
	{
	  current = new CDkDoublet(tmp[j]);
	  if(current->samePointel(start.getNextPointel()) && !firstNodeSet)
	    {
	      current->setWeight(0);
	      firstNodeSet = true;
	    }
	  if(current->samePointel(beforelast.getNextPointel()) && !lastNodeSet)
	    {
	      last = new CDkDoublet(*current);
	      lastNodeSet = true;
	      AHeap.push(last);
	    }
	  AHeap.push(current);
	}
    }
  return last;
  //std::cout<<"[end] CTile::findNodes"<<std::endl;
}

