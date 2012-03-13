/*
 * Copyright (C) Alexandre Dupas <alexandre.dupas@gmail.com>
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
#ifndef __PRIORITY_QUEUE_HH__
#define __PRIORITY_QUEUE_HH__
//******************************************************************************

/** Simple priority queue implementation using an ordered doubly-linked list.
 * @author Alexandre Dupas
 *
 * This priority queue implementation specifically allows to increase or
 * decrease the priority of an element. Any future implementation of this
 * data structure, should preserve this property (auto-balanced binary search
 * tree anyone?)
 */
template < typename TElement, typename TPriority >
class CPriorityQueue
{
protected:
  struct st_cell
  {
    st_cell * prev;
    st_cell * next;
    TPriority priority;
    TElement content;
  };

  st_cell * head;

  /// Alter the priority of the element contained in item
  /// O(N)
  void update( st_cell * item, const TPriority & APriority );

public:
  /** Define an iterator class on priority queue.
   * @author Alexandre Dupas
   */
  class iterator
  {
  public:
    /// Default constructor
    iterator();

    /// Constructor
    iterator( CPriorityQueue<TElement,TPriority> & pq );
    
    /// Copy constructor
    iterator( iterator & it );
    
    /// Destructor
    ~iterator();
    
    /// Affectation operator
    iterator& operator=(const iterator&);

    /// Is there any more elements?
    bool cont() const;
    
    /// Prefix ++
    iterator & operator++();
    
    // Postfix ++
    iterator operator++(int);
    
    // Current element
    TElement & operator*() const;

    // Priority of the element under the iterator
    TPriority priority() const;
    
  protected:
    /// Constructor
    iterator( st_cell * item );

  private:
    st_cell * cur;

    friend class CPriorityQueue;
  };

  typedef  typename CPriorityQueue<TElement, TPriority>::iterator iterator;

public:
  /// Constructor
  CPriorityQueue();

  /// Destructor
  ~CPriorityQueue();

  /// Is the queue empty?
  bool empty() const;

  /// Getter on the element of maximum priority
  /// O(1)
  const TElement & top() const;

  /// Getter on the maximum priority
  /// O(1)
  const TPriority & priority() const;

  /// Getter on the element of maximum priority
  /// O(1)
  TElement & top();

  /// Remove the element of higher priority
  /// O(1)
  void pop();

  /// Add the element with the specified priority
  /// O(N)
  iterator insert( const TElement & AElement, const TPriority & APriority );

  /// Alter the priority of the element designated by AIterator
  /// O(N)
  void update( const iterator & AIterator, const TPriority & APriority );

  /// Static method allowing to compute the stack size
  static unsigned int size( CPriorityQueue<TElement,TPriority> & pq )
  {
    unsigned int res = 0;
    for( iterator it(pq) ; it.cont() ; ++it )
      {
	++res;
      }
    return res;
  }

};
//******************************************************************************
// INLINE FUNCTIONS
//******************************************************************************
template <typename TElement, typename TPriority>
inline
CPriorityQueue<TElement,TPriority>::CPriorityQueue() :
  head( NULL )
{}
//------------------------------------------------------------------------------
template <typename TElement, typename TPriority>
inline
CPriorityQueue<TElement,TPriority>::~CPriorityQueue()
{
  st_cell * tmp = NULL;
  while( !empty() )
    {
      tmp = head;
      head = tmp->prev;
      delete tmp;
    }
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
bool CPriorityQueue<TElement,TPriority>::empty() const
{
  return head == NULL;
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
const TElement & CPriorityQueue<TElement,TPriority>::top() const
{
  assert( head != NULL );
  return head->content;
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
TElement & CPriorityQueue<TElement,TPriority>::top()
{ 
  assert( head != NULL );
  return head->content;
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
const TPriority & CPriorityQueue<TElement,TPriority>::priority() const
{
  assert( head != NULL );
  return head->priority;
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
typename CPriorityQueue<TElement, TPriority>::iterator 
CPriorityQueue<TElement, TPriority>::insert(const TElement& AElement, 
					    const TPriority& APriority)
{
  st_cell * prev = head;
  head = new st_cell;
  iterator it( head );
  head->prev = prev;
  head->next = NULL;
  head->content = AElement;
  if( prev != NULL )
    {
      prev->next = head;
      head->priority = prev->priority;
      update( head, APriority );
    }
  else
    {
      head->priority = APriority;
    }
  return it;
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
void CPriorityQueue<TElement,TPriority>::update( st_cell * item,
						 const TPriority & APriority )
{
  st_cell * cur = item;
  if( APriority < item->priority )
    {
      // déplacement vers la droite -> vers prev -> priorité décroissante
      while( cur->prev != NULL && cur->prev->priority > APriority )
	{
	  cur = cur->prev;
	}
    }
  if( APriority > item->priority )
    {
      // déplacement vers la gauche -> vers next -> priorité croissante
      while( cur->next != NULL && cur->next->priority < APriority )
	{
	  cur = cur->next;
	}
    }
  // définition de la nouvelle priorité
  item->priority = APriority;

  // a présent cur pointe à peu près où l'on veut.
  if( cur == item ) return;
  
  if( item == head && item->prev != NULL )
    {
      head = item->prev;
    }
  // déconnexion
  if( item->next != NULL )
    item->next->prev = item->prev;
  if( item->prev != NULL )
    item->prev->next = item->next;
  item->prev = NULL;
  item->next = NULL;

  if( cur->priority <= APriority )
    {
      // je m'insère à gauche entre cur et cur->next
      // attention head
      item->next = cur->next;
      item->prev = cur;
      if( cur->next != NULL )
	{
	  cur->next->prev = item;
	}
      else
	{
	  head = item;
	}
      cur->next = item;
    }
  else
    {
      // je m'insère à droite entre cur et cur->prev
      item->prev = cur->prev;
      item->next = cur;
      if( cur->prev != NULL )
	cur->prev->next = item;
      cur->prev = item;
    }

}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
void CPriorityQueue<TElement,TPriority>::update( const iterator & AIterator, 
						 const TPriority & APriority )
{
  update( AIterator.cur, APriority );
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
void CPriorityQueue<TElement,TPriority>::pop()
{
  assert( head != NULL );
  st_cell * tmp = head;
  head = head->prev;
  if( head )
    head->next = NULL;
  delete tmp;
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
CPriorityQueue<TElement,TPriority>::iterator::iterator() :
  cur(NULL)
{}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
CPriorityQueue<TElement,TPriority>::iterator::iterator( CPriorityQueue<TElement,TPriority> & pq ) :
  cur(pq.head)
{}
//******************************************************************************
/// Copy constructor
template <typename TElement, typename TPriority>
inline
CPriorityQueue<TElement,TPriority>::iterator::iterator( iterator & it ) :
  cur(it.cur)
{}
//******************************************************************************
/// Destructor
template <typename TElement, typename TPriority>
inline
CPriorityQueue<TElement,TPriority>::iterator::~iterator()
{}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
typename CPriorityQueue<TElement,TPriority>::iterator & 
CPriorityQueue<TElement,TPriority>::iterator::operator=(const CPriorityQueue<TElement,TPriority>::iterator& it)
{
  cur = it.cur;
  return *this;
}
//******************************************************************************
/// Is there any more elements?
template <typename TElement, typename TPriority>
inline
bool CPriorityQueue<TElement,TPriority>::iterator::cont() const
{ return cur != NULL; }
//******************************************************************************
/// Prefix ++
template <typename TElement, typename TPriority>
inline
typename CPriorityQueue<TElement,TPriority>::iterator & CPriorityQueue<TElement,TPriority>::iterator::operator++()
{
  cur = cur->prev;
  return *this;
}
//******************************************************************************
// Postfix ++
template <typename TElement, typename TPriority>
inline
typename CPriorityQueue<TElement,TPriority>::iterator CPriorityQueue<TElement,TPriority>::iterator::operator++(int)
{
  iterator it( *this );
  operator++();
  return it;
}
//******************************************************************************
template <typename TElement, typename TPriority>
inline
TElement & CPriorityQueue<TElement,TPriority>::iterator::operator*() const
{ return cur->content; }
//******************************************************************************
template <typename TElement, typename TPriority>
inline
TPriority CPriorityQueue<TElement,TPriority>::iterator::priority() const
{ return cur->priority; }

//******************************************************************************
template <typename TElement, typename TPriority>
inline
CPriorityQueue<TElement,TPriority>::iterator::iterator( st_cell * item ) :
  cur(item)
{}
//******************************************************************************
#endif // __PRIORITY_QUEUE_HH__
//******************************************************************************
