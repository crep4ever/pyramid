/*
 * lib-mapkernel : Un noyau de 3-cartes et des opérations.
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/
 *
 * This file is part of lib-mapkernel
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
#ifndef LIMITED_STACK_HH
#define LIMITED_STACK_HH
//******************************************************************************
#include <cassert>
//******************************************************************************
/**
 * Définit la classe CLimitedStack qui est une pile de longueur bornée
 * représentée dans un tableau. Les primitives sont étendues car on peut
 * accéder en lecture à n'importe quel élément de la pile.
 *
 * @author MOKA TEAM
 */
template <class TElement, unsigned int Size>
class CLimitedStack
{
private:
  /// Le tableau contenant les éléments de la pile.
  TElement FArray[Size];

  /// Le nombre d'élément de la pile
  unsigned int FNbElements;

public:
  // @name Constructeurs et destructeur
  // @{

  /**
   * Constructeur par défaut d'une nouvelle instance de la classe,
   * de taille maximale ASize.
   */
  CLimitedStack();

  /// Destructeur
  ~CLimitedStack();

  // @}
  
  // @name Accesseurs
  // @{

  /// Accès en lecture au nombre d'éléments dans la liste.
  unsigned int getNbElements() const;

  /// Pour savoir si la pile est vide.
  bool isEmpty() const;

  /// Accès en lecture au ieme élément de la liste
  TElement& getElement( unsigned int AIndex ) const;

  /// Accès en lecture à la tête de pile.
  TElement top() const;

  /// Ajouter un élément en haut de la pile.
  void push(TElement AElt);

  /// Enleve l'element en tête de pile.
  void pop();
  
  /// Vide la pile.
  void clear();
  
  // @}
};

//******************************************************************************
//******************************************************************************
//******************************************************************************
//- INLINE CODE
//******************************************************************************
//******************************************************************************
//******************************************************************************
template <class TElement, unsigned int Size>
inline
CLimitedStack<TElement,Size>::CLimitedStack() :
  FNbElements(0)
{}
//------------------------------------------------------------------------------
template <class TElement, unsigned int Size>
inline
CLimitedStack<TElement,Size>::~CLimitedStack()
{}
//******************************************************************************
template <class TElement, unsigned int Size>
inline
unsigned int CLimitedStack<TElement,Size>::getNbElements() const
{ return FNbElements; }
//------------------------------------------------------------------------------
template <class TElement, unsigned int Size>
inline
bool CLimitedStack<TElement,Size>::isEmpty() const
{ return (FNbElements==0); }
//------------------------------------------------------------------------------
template <class TElement, unsigned int Size>
inline
TElement& CLimitedStack<TElement,Size>::getElement( unsigned int AIndex ) const
{
  assert( AIndex<FNbElements );
  return FArray[AIndex];
}
//------------------------------------------------------------------------------
template <class TElement, unsigned int Size>
inline
TElement CLimitedStack<TElement,Size>::top() const
{
  assert( FNbElements>0 );
  return FArray[FNbElements-1];
}
//******************************************************************************
template <class TElement, unsigned int Size>
inline
void CLimitedStack<TElement,Size>::push(TElement AElt)
{
  assert( FNbElements<Size );
  FArray[FNbElements] = AElt;
  ++FNbElements;
}
//------------------------------------------------------------------------------
template <class TElement, unsigned int Size>
inline
void CLimitedStack<TElement,Size>::pop()
{
  assert( FNbElements>0 );
  --FNbElements;
}
//------------------------------------------------------------------------------
template <class TElement, unsigned int Size>
inline
void CLimitedStack<TElement,Size>::clear()
{ FNbElements = 0; }
//******************************************************************************
#endif // LIMITED_STACK_HH
//******************************************************************************
