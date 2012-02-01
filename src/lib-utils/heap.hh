/*
 * Copyright (C) 20010, Guillaume Née <guillaume.nee@greyc.ensicaen.fr>
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
#ifndef HEAP_HH
#define HEAP_HH
//******************************************************************************
#include <functional>
#include <ext/pb_ds/priority_queue.hpp>
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;
using namespace std;
//******************************************************************************

namespace Map2d
{
  class CDkDoublet;

  class EdgeComp 
  {
  public:
    // bool operator()(const CDkDoublet & e1, const CDkDoublet & e2) const;
    // bool operator()(CDkDoublet & e1, CDkDoublet & e2);
    bool operator()(const CDkDoublet * e1, const CDkDoublet * e2) const;
    bool operator()(CDkDoublet * e1, CDkDoublet * e2);
  };
  
  class Heap 
  {
  public:
    /**
     * @brief Push an edge in the heap 
     * If the edge is already in, this is a no-op.
     * @param _edge the edge to push
     */
    void push(CDkDoublet * _edge);
    
    /**
     * @brief Returns the top of the heap 
     * @return the edge with the minimum weight
     */
    inline CDkDoublet* top() const {
      assert(!empty());
      return heap.top();
    };

    /**
     * @brief Erases the top of the heap
     * Erases the edge with minimum weight
     */

    inline  void pop(){
      assert(!empty());
      index.erase(heap.top());
      heap.pop();
    };

    /**
     * @brief Erases an arbitrary edge in the heap
     * Erases an arbitrary edge. If the edge is not in the heap, this
     * is a no-op, and the return value is false.
     * @param _edge a pointer on the edge to erase
     * @return false if the edge is not erase, true otherwise
     */
    bool erase(CDkDoublet * _edge);

    /**
     * @brief Modify an arbitrary edge in the heap
     * Modify an arbitrary edge. If the edge is not in the heap, this
     * is a no-op, and the return value is false.
     * @param _edge a pointer on the edge to modify
     * @return false if the edge is not modify, true otherwise
     */
    bool modify(CDkDoublet * _edge);

    /**
     * @brief Test if the heap is empty
     * @return true if empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief Gives the size of the heap
     * @return the size of the heap
     */
    size_t size() const;


  public:
    typedef __gnu_pbds::priority_queue<CDkDoublet*, EdgeComp, rc_binomial_heap_tag> ThinHeap;
    typedef cc_hash_table<CDkDoublet*, ThinHeap::point_iterator> HeapIndex;
    /* Thin heap to store edges (Kaplan and Tarjan) */
    ThinHeap heap;
    /* Index to find the position of an element inside the heap */
    HeapIndex index;
    
  };
}
#endif  // HEAP_HH
