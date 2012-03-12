/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
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
#include "map.hh"
#include INCLUDE_NON_INLINE("map.icc")
using namespace Map2d;
//******************************************************************************
CMap::~CMap()
{
  empty();
}
//******************************************************************************
CCoverage* CMap::getDynamicCoverage(const CDart* ADart, TOrbit AOrbit)
{
  switch (AOrbit)
    {
    case ORBIT_SELF:   return new CDynamicCoverageSelf  (this, ADart);
    case ORBIT_0:      return new CDynamicCoverage0     (this, ADart);
    case ORBIT_1:      return new CDynamicCoverage1     (this, ADart);
    case ORBIT_2:      return new CDynamicCoverage2     (this, ADart);
    case ORBIT_12:     return new CDynamicCoverage12    (this, ADart);
    case ORBIT_VERTEX: return new CDynamicCoverageVertex(this, ADart);
    default: assert(false);
    }

  return NULL;
}
//******************************************************************************
CCoverage* CMap::getBasicDynamicCoverage(const CDart* ADart, TOrbit AOrbit,
					 int AMark, int ADirectIndex)
{
  switch (AOrbit)
    {
    case ORBIT_12:     return new CBasicDynamicCoverage12(this, ADart,
							  AMark, ADirectIndex);
    case ORBIT_VERTEX: return new CBasicDynamicCoverageVertex(this, ADart,
							      AMark,
							      ADirectIndex);
    default: assert(false);
    }

  return NULL;
}
//******************************************************************************
CCoverage* CMap::getStaticCoverage(const CDart* ADart, TOrbit AOrbit)
{
  switch (AOrbit)
    {
    case ORBIT_SELF:   return new CStaticCoverageSelf  (this, ADart);
    case ORBIT_0:      return new CStaticCoverage0     (this, ADart);
    case ORBIT_1:      return new CStaticCoverage1     (this, ADart);
    case ORBIT_2:      return new CStaticCoverage2     (this, ADart);
    case ORBIT_12:     return new CStaticCoverage12    (this, ADart);
    case ORBIT_VERTEX: return new CStaticCoverageVertex(this, ADart);
    default: assert(false);
   }

  return NULL;
}
//******************************************************************************
