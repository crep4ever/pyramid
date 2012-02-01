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
#ifndef __VOLUME_HH__
#define __VOLUME_HH__
//*****************************************************************************
#include "alt-stdint.hh"
#include "point2d.hh"
#include "string.h"
//*****************************************************************************
/** 
 * Volume template class used to represent a 3D matrix.
 */
template< typename TElement >
class CVolume {
public:
  /** Constructor of a volume with a given size.
   * @param ASizeX width of the volume
   * @param ASizeY height of the volume
   * @param ASizeZ depth of the volume
   */
  CVolume( uint32_t ASizeX, uint32_t ASizeY, uint32_t ASizeZ );
  
  /** Copy constructor
   */
  CVolume( const CVolume< TElement > & AVolume );

  /** Destructor
   */
  ~CVolume();

  /** Fill the volume with a value.
   *
   * @param AValue value to fill the volume with
   */
  void fill( const TElement & AValue );

  void fill();

  /** Getter on the width of the volume.
   *
   * @return width of the volume ( X size )
   */
  uint32_t getSizeX() const;

  /** Getter on the height of the volume.
   *
   * @return height of the volume ( Y size )
   */
  uint32_t getSizeY() const;

  /** Getter on the depth of the volume.
   *
   * @return depth of the volume ( Z size )
   */
  uint32_t getSizeZ() const;

  /** Getter on the data contained in a cell.
   *
   * @param Ax X coordinate of the cell
   * @param Ay Y coordinate of the cell
   * @param Az Z coordinate of the cell
   *
   * @return value stored in the (Ax,Ay,Az) cell
   */
  TElement getValue( uint32_t Ax, uint32_t Ay, uint32_t Az ) const;

  /** Setter of the data contained in a cell.
   *
   * @param Ax X coordinate of the cell
   * @param Ay Y coordinate of the cell
   * @param Az Z coordinate of the cell
   * @param AVal value to push in the cell
   */
  void setValue( uint32_t Ax, uint32_t Ay, uint32_t Az, const TElement & AVal );

  /** Getter on the data contained in a cell.
   *
   * @param ACoordinate coordinates of the cell
   *
   * @return value stored in the (Ax,Ay,Az) cell
   */
  //  TElement getValue( const CPoint2D & ACoordinate ) const;

  /** Setter of the data contained in a cell.
   *
   * @param ACoordinate coordinates of the cell
   * @param AVal value to push in the cell
   */
  //void setValue( const CPoint2D & ACoordinate, const TElement & AVal );


private:
  /** Private default constructor as a volume should have a non-zero size.
   */
  CVolume();

  /** Index in the data array of the specified cell.
   *
   * @param Ax X coordinate of the cell
   * @param Ay Y coordinate of the cell
   * @param Az Z coordinate of the cell
   *
   * @return index of the (Ax,Ay,Az) cell in the data array
   */
  unsigned long indexOf( uint32_t Ax, uint32_t Ay, uint32_t Az ) const;
  
  // Size of the current volume.
  uint32_t FSizeX, FSizeY, FSizeZ;

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
CVolume< TElement >::CVolume()
{}

template< typename TElement >
inline
CVolume< TElement >::CVolume( uint32_t ASizeX, 
			      uint32_t ASizeY, 
			      uint32_t ASizeZ ) :
  FSizeX( ASizeX ),
  FSizeY( ASizeY ),
  FSizeZ( ASizeZ ),
  FData( NULL )
{
  assert( FSizeX != 0 );
  assert( FSizeY != 0 );
  assert( FSizeZ != 0 );
  FData = new TElement[FSizeX*FSizeY*FSizeZ]; 
}

template< typename TElement >
inline
CVolume< TElement >::CVolume( const CVolume< TElement > & AVolume ) :
  FSizeX( AVolume.FSizeX ),
  FSizeY( AVolume.FSizeY ),
  FSizeZ( AVolume.FSizeZ ),
  FData( NULL )  
{
  FData = new TElement[ FSizeX * FSizeY * FSizeZ ];
  //for( int i = 0 ; i <  FSizeX * FSizeY * FSizeZ ; ++i )
  //FData[i] = AVolume.FData[i];
  std::cout<<"memcpy pas testé"<<std::endl;
  memcpy(FData, AVolume.FData, FSizeX * FSizeY * FSizeZ);
}

template< typename TElement >
inline
CVolume< TElement >::~CVolume()
{
  assert( FData != NULL );
  delete [] FData;
}

template< typename TElement >
inline
uint32_t CVolume< TElement >::getSizeX() const
{ return FSizeX; }

template< typename TElement >
inline
uint32_t CVolume< TElement >::getSizeY() const
{ return FSizeY; }

template< typename TElement >
inline
uint32_t CVolume< TElement >::getSizeZ() const
{ return FSizeZ; }

template< typename TElement >
inline
unsigned long CVolume< TElement >::indexOf( uint32_t Ax, 
					    uint32_t Ay, 
					    uint32_t Az ) const
{   
  assert( Ax < FSizeX );
  assert( Ay < FSizeY );
  assert( Az < FSizeZ );
  return Az * FSizeX * FSizeY + Ay * FSizeX + Ax;
}

template< typename TElement >
inline
void CVolume< TElement >::fill( const TElement & AValue )
{
  assert( FData != NULL );
  //#pragma omp parallel for
  for( unsigned int i = 0 ; i < FSizeX*FSizeY*FSizeZ ; ++i )
    FData[i] = AValue;
}

template< typename TElement >
inline
void CVolume< TElement >::fill( )
{
  assert( FData != NULL );
  bzero(FData, FSizeX * FSizeY * FSizeZ * sizeof(TElement));
}


template< typename TElement >
inline
TElement CVolume< TElement >::getValue( uint32_t Ax, 
					uint32_t Ay, 
					uint32_t Az ) const
{
  assert( FData != NULL );
  return FData[ indexOf( Ax, Ay, Az ) ];
}

template< typename TElement >
inline
void CVolume< TElement >::setValue( uint32_t Ax, 
				    uint32_t Ay, 
				    uint32_t Az, 
				    const TElement & AVal )
{
  assert( FData != NULL );
  FData[ indexOf( Ax, Ay, Az ) ] = AVal;
}

/*
  template< typename TElement >
  inline
  TElement CVolume< TElement >::getValue( const CPoint2D & ACoordinate ) const
  {
  return getValue( ACoordinate.getX(),
  ACoordinate.getY(),
  ACoordinate.getZ() );
  }

  template< typename TElement >
  inline
  void CVolume< TElement >::setValue( const CPoint2D & ACoordinate, 
  const TElement & AVal )
  {
  setValue( ACoordinate.getX(),
  ACoordinate.getY(),
  ACoordinate.getZ(), AVal );
  }
*/


//*****************************************************************************
#endif // __VOLUME_HH__
//*****************************************************************************
