/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
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

#ifndef CRITERION_SPLIT_HH
#define CRITERION_SPLIT_HH

#include "region.hh"

namespace clustering
{
  /**
   * \file criterion-split.hh
   * \class CCriterionSplit
   *
   * Abstract class for splitting criteria.
   *
   * The method isRegionToSplit must be reimplemented according to
   * the desired criterion. Thus, it determines whether or not the region
   * is homogeneous according to this criterion. If the region is not homogeneous
   * then, it should be refined (split).
   *
   */

  class CCriterionSplit
  {
  public :
    // Constructor
    CCriterionSplit();
    CCriterionSplit(Map2d::CRegion* region);

    // Destructor
    virtual ~CCriterionSplit() = 0;

    Map2d::CRegion* region() const;
    void setRegion(Map2d::CRegion* region);

    /// Determines if the region should be split
    /// @return true if the region should be split, false otherwise
    virtual bool isRegionToSplit() = 0;

  private:
    Map2d::CRegion* m_region; ///< the region to be split
  };
} // namespace clustering
#include INCLUDE_INLINE("criterion-split.icc")
#endif // CRITERION_SPLIT_HH
