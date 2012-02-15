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

#ifndef MERGE_DEVIATION_HH
#define MERGE_DEVIATION_HH

#include "criterion-split.hh"

namespace clustering
{
  /**
   * \file split-deviation.hh
   * \class CSplitDeviation
   *
   * Split a region if its standard deviation is greater than its parent region
   *
   */

  class CSplitDeviation : public CCriterionSplit
  {
  public:

    // Constructor
    CSplitDeviation();
    CSplitDeviation(Map2d::CRegion* region, float threshold);

    // Destructor
    virtual ~CSplitDeviation();

    bool isRegionToSplit();

  private:
    float m_threshold;
  };
} // namespace clustering
#include INCLUDE_INLINE("split-deviation.icc")
#endif // MERGE_DEVIATION_HH
