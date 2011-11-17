/*
 * Copyright (C) 20010, Guillaume Née <guillaume.nee@greyc.ensicaen.fr>
 *                      Romain Goffe <romain.goffe@gmail.com>
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
#include <cassert>
#include "dk-doublet.hh"
#include "heap.hh"
//******************************************************************************
namespace Map2d
{
  void Heap::push(CDkDoublet * _edge)
  {
    // First check if the int is already in the container. If so, just return.
    if (index.find(_edge) != index.end())
      return;
    
    // Push the int into the priority queue, and store a point_iterator to it.
    ThinHeap::point_iterator it = heap.push(_edge);
    
    try
      {
	// Now make the map associate the int to the point_iterator.
	index[_edge] = it;
      }
    catch(...)
      {
	// If the above failed, we need to remove the int from the
	// priority queue as well.
	heap.erase(it);
	throw;
      }
  }
  //  //------------------------------------------------------------------------------
  //  CDkDoublet* Heap::top() const 
  //  {
  //    assert(!empty());
  //    return heap.top();
  //  }
  //------------------------------------------------------------------------------
  //  void Heap::pop()
  //  {
  //    assert(!empty());
  //    index.erase(heap.top());
  //    heap.pop();
  //  }
  //------------------------------------------------------------------------------
  bool Heap::erase(CDkDoublet * _edge)
  {
    HeapIndex::point_iterator it = index.find(_edge);
    
    // If the edge is not in the map, this is a no-op.
    if (it == index.end())
      return false;

    // Otherwise, we erase it from the priority queue.
    heap.erase(it->second);

    // ...then from the map.
    index.erase(_edge);

    return true;
  }
  //------------------------------------------------------------------------------
  bool Heap::modify(CDkDoublet * _edge)
  {
    HeapIndex::point_iterator it = index.find(_edge);
    
    // If the edge is not in the map, this is a no-op.
    if (it == index.end())
      return false;

    // Otherwise, we erase it from the priority queue.
    heap.modify(it->second, it->first);

    return true;
  }
  //------------------------------------------------------------------------------
  bool Heap::empty() const 
  {
    return heap.empty(); 
  }
  //------------------------------------------------------------------------------
  size_t Heap::size() const 
  {
    return heap.size(); 
  }
  //------------------------------------------------------------------------------
  //  bool EdgeComp::operator()(const CDkDoublet & e1, const CDkDoublet & e2) const
  //  {
  //    return (e1.weight() > e2.weight());
  //  }
  //  //------------------------------------------------------------------------------ 
  //  bool EdgeComp::operator()(CDkDoublet & e1, CDkDoublet & e2)
  //  {
  //    return (e1.weight() > e2.weight());
  //  }
  //------------------------------------------------------------------------------
  bool EdgeComp::operator()(CDkDoublet * e1, CDkDoublet * e2)
  {
    return (e1->weight() > e2->weight());
  }
  //------------------------------------------------------------------------------ 
  bool EdgeComp::operator()(const CDkDoublet * e1, const CDkDoublet * e2) const
  {
    return ( e1->weight() > e2->weight() );
  }
}
