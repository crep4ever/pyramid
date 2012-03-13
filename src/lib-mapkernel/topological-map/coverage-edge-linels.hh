/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
 * Copyright (C) Moka Team, guillaume.damiand@liris.cnrs.fr
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

//*****************************************************************************
#ifndef __COVERAGE_EDGE_LINELS_HH__
#define __COVERAGE_EDGE_LINELS_HH__
//*****************************************************************************
#include "doublet.hh"
//*****************************************************************************
namespace Map2d
{
  class CTopologicalMap;
  class CDart;

/** Coverage of all the linel belonging to the embedding of an edge
 * @author Alexandre Dupas
 */
class CCoverageEdgeLinels
{
public:
  /** Constructor from a starting pointel and linel
   * @param AMap topological map (containing the interpixel matrix)
   * @param ADoublet doublet giving the starting pointel and linel
   */
  CCoverageEdgeLinels( CTopologicalMap * AMap, const CDoublet & ADoublet );

  /** Constructor from a starting dart
   * @param AMap topological map (containing the intervoxel matrix)
   * @param ADart dart giving the edge to cover
   */
  CCoverageEdgeLinels( CTopologicalMap * AMap, CDart * ADart );

  /** Destructor of the coverage.
   */
  ~CCoverageEdgeLinels();

  /** Prefix iterator: set the coverage to the next linel.
   */
  void operator++();

  /** Postfix iterator: set the coverage to the next linel.
   * @return the previous linel
   */
  CDoublet operator++(int);

  /** Prefix iterator: set the coverage to the previous linel.
   */
  void operator--();

  /** Postfix iterator: set the coverage to the previous linel.
   * @return the previous linel
   */
  CDoublet operator--(int);

  /** Stopping flag for the coverage.
   * @return true if it is possible to continue forward
   */
  bool cont() const;

  /** Stopping flag for the coverage.
   * @return true if it is possible to continue backward
   */
  bool reverseCont() const;

  /** Return the code of the previous move
   */
  int getMove() const;

  /** Dereference of the coverage in a linel.
   * @return the current designated linel
   */
  CDoublet operator*() const;

  /** Reinitialize the coverage to the starting position.
   */
  void reinit();

protected:
  /// Topological map
  CTopologicalMap * FMap;

  /// First linel
  CDoublet lFirst;

  /// Current linel
  CDoublet lCur;

  /// Flag for the end of the coverage
  bool FStop;

  /// Code of the previous move
  int FMove;
};
//*****************************************************************************
} // namespace Map2d
//*****************************************************************************
#include INCLUDE_INLINE("coverage-edge-linels.icc")
//*****************************************************************************
#endif // __COVERAGE_EDGE_LINELS_HH__
//*****************************************************************************
