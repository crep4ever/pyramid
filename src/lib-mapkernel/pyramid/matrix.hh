/*
 * Copyright (C) 2008-2011, Alexandre Dupas <alexandre.dupas@gmail.com>
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

//*****************************************************************************
#ifndef __MATRIX_HH__
#define __MATRIX_HH__
//*****************************************************************************
#include "alt-stdint.hh"
#include "point2d.hh"
//*****************************************************************************
/** Matrix template class used to represent a 2D matrix.
 *
 */
template< typename TElement >
class CMatrix {
public:
  /** Constructor of a matrix with a given size.
   * @param ASizeX width of the matrix
   * @param ASizeY height of the matrix
   */
  CMatrix( uint32_t ASizeX, uint32_t ASizeY );
  
  /** Copy constructor
   */
  CMatrix( const CMatrix< TElement > & AMatrix );

  /** Destructor
   */
  ~CMatrix();

  /** Fill the matrix with a value.
   *
   * @param AValue value to fill the matrix with
   */
  void fill( const TElement & AValue );
  void fill();
  /** Getter on the width of the matrix.
   *
   * @return width of the matrix ( X size )
   */
  uint32_t getSizeX() const;

  /** Getter on the height of the matrix.
   *
   * @return height of the matrix ( Y size )
   */
  uint32_t getSizeY() const;


  /** Getter on the data contained in a cell.
   *
   * @param Ax X coordinate of the cell
   * @param Ay Y coordinate of the cell
   *
   * @return value stored in the (Ax,Ay) cell
   */
  TElement getValue( uint32_t Ax, uint32_t Ay ) const;

  /** Setter of the data contained in a cell.
   *
   * @param Ax X coordinate of the cell
   * @param Ay Y coordinate of the cell
   * @param AVal value to push in the cell
   */
  void setValue( uint32_t Ax, uint32_t Ay, const TElement & AVal );

  /** Getter on the data contained in a cell.
   *
   * @param ACoordinate coordinates of the cell
   *
   * @return value stored in the (Ax,Ay,Az) cell
   */
  TElement getValue( const CPoint2D & ACoordinate ) const;

  /** Setter of the data contained in a cell.
   *
   * @param ACoordinate coordinates of the cell
   * @param AVal value to push in the cell
   */
  void setValue( const CPoint2D & ACoordinate, const TElement & AVal );

  //private:
public :
  /** Private default constructor as a matrix should have a non-zero size.
   */
  CMatrix();

  /** Index in the data array of the specified cell.
   *
   * @param Ax X coordinate of the cell
   * @param Ay Y coordinate of the cell
   * @param Az Z coordinate of the cell
   *
   * @return index of the (Ax,Ay,Az) cell in the data array
   */
  unsigned long indexOf( uint32_t Ax, uint32_t Ay ) const;
  
  // Size of the current matrix.
  uint32_t FSizeX, FSizeY;

  // Data array
  TElement * FData;
};
//*****************************************************************************
//*****************************************************************************
#include <cassert>
#include <cstdlib>
//*****************************************************************************
template< typename TElement >
inline
CMatrix< TElement >::CMatrix()
{}

template< typename TElement >
inline
CMatrix< TElement >::CMatrix( uint32_t ASizeX, 
			      uint32_t ASizeY ) :
  FSizeX( ASizeX ),
  FSizeY( ASizeY ),
  FData( NULL )
{
  assert( FSizeX != 0 );
  assert( FSizeY != 0 );
  FData = new TElement[FSizeX*FSizeY]; 
}

template< typename TElement >
inline
CMatrix< TElement >::CMatrix( const CMatrix< TElement > & AMatrix ) :
  FSizeX( AMatrix.FSizeX ),
  FSizeY( AMatrix.FSizeY ),
  FData( NULL )  
{
  FData = new TElement[ FSizeX * FSizeY ];
  //for( int i = 0 ; i <  FSizeX * FSizeY ; ++i )
  //  FData[i] = AMatrix.FData[i];
  std::cout<<"memcpy pas testé"<<std::endl;
  memcpy(FData, AMatrix.FData, FSizeX * FSizeY);
  
}

template< typename TElement >
inline
CMatrix< TElement >::~CMatrix()
{
  assert( FData != NULL );
  delete [] FData;
}

template< typename TElement >
inline
uint32_t CMatrix< TElement >::getSizeX() const
{ return FSizeX; }

template< typename TElement >
inline
uint32_t CMatrix< TElement >::getSizeY() const
{ return FSizeY; }

template< typename TElement >
inline
unsigned long CMatrix< TElement >::indexOf( uint32_t Ax, 
					    uint32_t Ay ) const
{   
  assert( Ax <= FSizeX );
  assert( Ay <= FSizeY );
  return Ay * FSizeX + Ax;
}

template< typename TElement >
inline
void CMatrix< TElement >::fill( const TElement & AValue )
{
  assert( FData != NULL );
  memset(FData, (intptr_t) AValue, FSizeX * FSizeY);
}

template< typename TElement >
inline
void CMatrix< TElement >::fill( )
{
  assert( FData != NULL );
  bzero(FData, FSizeX * FSizeY * sizeof(TElement));
}


template< typename TElement >
inline
TElement CMatrix< TElement >::getValue( uint32_t Ax, 
					uint32_t Ay ) const
{
  assert( FData != NULL );
  return FData[ indexOf( Ax, Ay ) ];
}

template< typename TElement >
inline
void CMatrix< TElement >::setValue( uint32_t Ax, 
				    uint32_t Ay, 
				    const TElement & AVal )
{
  assert( FData != NULL );
  FData[ indexOf( Ax, Ay ) ] = AVal;
}

template< typename TElement >
inline
TElement CMatrix< TElement >::getValue( const CPoint2D & ACoordinate ) const
{
  return getValue( ACoordinate.getX(),
		   ACoordinate.getY() );
}

template< typename TElement >
inline
void CMatrix< TElement >::setValue( const CPoint2D & ACoordinate, 
				    const TElement & AVal )
{
  setValue( ACoordinate.getX(),
	    ACoordinate.getY(), AVal );
}
//*****************************************************************************
#endif // __MATRIX_HH__
//*****************************************************************************
