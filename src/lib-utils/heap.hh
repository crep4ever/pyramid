/*
 * Copyright (C) 2012, Romain Goffe <romain.goffe@gmail.com>
 * Copyright (C) 2010, Guillaume Née <guillaume.nee@greyc.ensicaen.fr>
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
//******************************************************************************

template< typename TElement >
class EdgeComp 
{
public:
  bool operator()(const TElement e1, const TElement e2) const;
  bool operator()(TElement e1, TElement e2);
};


template<typename TElement>
struct ThinHeap
{
  typedef __gnu_pbds::priority_queue<TElement, EdgeComp<TElement>, rc_binomial_heap_tag> Type;
};

template<typename T>
struct HeapIndex
{
  typedef cc_hash_table
  < T, typename ThinHeap<T>::Type::point_iterator > Type;
};

template< typename TElement >
class Heap 
{
public:
  /**
   * @brief Push an edge in the heap 
   * If the edge is already in, this is a no-op.
   * @param _edge the edge to push
   */
  void push(TElement _edge);
    
  /**
   * @brief Returns the top of the heap 
   * @return the edge with the minimum weight
   */
  inline TElement top() const {
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
  bool erase(TElement _edge);

  /**
   * @brief Modify an arbitrary edge in the heap
   * Modify an arbitrary edge. If the edge is not in the heap, this
   * is a no-op, and the return value is false.
   * @param _edge a pointer on the edge to modify
   * @return false if the edge is not modify, true otherwise
   */
  bool modify(TElement _edge);

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
  /* Thin heap to store edges (Kaplan and Tarjan) */
  typename ThinHeap<TElement>::Type heap;
  /* Index to find the position of an element inside the heap */
  typename HeapIndex<TElement>::Type index;
};


//******************************************************************************

template< typename TElement >
inline
void Heap<TElement>::push(TElement _edge)
{
  // First check if the int is already in the container. If so, just return.
  if (index.find(_edge) != index.end())
    return;

  // Push the int into the priority queue, and store a point_iterator to it.
  typename ThinHeap<TElement>::Type::point_iterator it = heap.push(_edge);
  
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

template< typename TElement >
inline
bool Heap<TElement>::erase(TElement _edge)
{
  typename HeapIndex<TElement>::Type::point_iterator it = index.find(_edge);

  // If the edge is not in the map, this is a no-op.
  if (it == index.end())
    return false;

  // Otherwise, we erase it from the priority queue.
  heap.erase(it->second);

  // ...then from the map.
  index.erase(_edge);

  return true;
}

template< typename TElement >
inline
bool Heap<TElement>::modify(TElement _edge)
{
  typename HeapIndex<TElement>::Type::point_iterator it = index.find(_edge);
    
  // If the edge is not in the map, this is a no-op.
  if (it == index.end())
    return false;

  // Otherwise, we erase it from the priority queue.
  heap.modify(it->second, it->first);

  return true;
}

template< typename TElement >
inline
bool Heap<TElement>::empty() const 
{
  return heap.empty(); 
}

template< typename TElement >
inline
size_t Heap<TElement>::size() const 
{
  return heap.size(); 
}

template< typename TElement >
inline
bool EdgeComp<TElement>::operator()(TElement e1, TElement e2)
{
  return (e1->weight() > e2->weight());
}

template< typename TElement >
inline
bool EdgeComp<TElement>::operator()(const TElement e1, const TElement e2) const
{
  return ( e1->weight() > e2->weight() );
}

#endif  // HEAP_HH
