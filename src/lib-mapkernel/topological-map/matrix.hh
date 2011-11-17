//*****************************************************************************
#ifndef __MATRIX_HH__
#define __MATRIX_HH__
//*****************************************************************************
#include "alt-stdint.hh"
#include "coordinate.hh"
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
  TElement getValue( const CCoordinate & ACoordinate ) const;

  /** Setter of the data contained in a cell.
   *
   * @param ACoordinate coordinates of the cell
   * @param AVal value to push in the cell
   */
  void setValue( const CCoordinate & ACoordinate, const TElement & AVal );

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
  assert( Ax < FSizeX );
  assert( Ay < FSizeY );
  return Ay * FSizeX + Ax;
}

template< typename TElement >
inline
void CMatrix< TElement >::fill( const TElement & AValue )
{
  assert( FData != NULL );
  //memset(FData, AValue, FSizeX * FSizeY);
  unsigned int size = FSizeX * FSizeY;
  //std::fill_n( FData, size, const_cast<TElement>(AValue) );
  //#pragma omp parallel for
  for( unsigned int i = 0 ; i < size ; ++i )
    FData[i] = AValue;
  //FData = {};
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
TElement CMatrix< TElement >::getValue( const CCoordinate & ACoordinate ) const
{
  return getValue( ACoordinate.getX(),
		   ACoordinate.getY() );
}

template< typename TElement >
inline
void CMatrix< TElement >::setValue( const CCoordinate & ACoordinate, 
				    const TElement & AVal )
{
  setValue( ACoordinate.getX(),
	    ACoordinate.getY(), AVal );
}
//*****************************************************************************
#endif // __MATRIX_HH__
//*****************************************************************************
