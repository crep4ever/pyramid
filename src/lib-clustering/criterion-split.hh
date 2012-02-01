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

//******************************************************************************
#ifndef CRITERION_SPLIT_HH
#define CRITERION_SPLIT_HH
//******************************************************************************

namespace Map2d
{
  /**
   * \file criterion-split.hh
   *
   * Abstract class for splitting criteria of a region.
   *
   * \author Romain Goffe
   */

  class CRegion;
  class CCriterionSplit
  {
  protected:
    CRegion* FRegion; //La région à splitter

  public :
    // Constructeurs
    CCriterionSplit(); //par défaut
    CCriterionSplit(CRegion* ARegion);

    // Destructeur
    virtual ~CCriterionSplit() = 0;

    CRegion* getRegion() const;
    void setRegion(CRegion* ARegion);

    //******************************************************************************
    //Méthodes
    //******************************************************************************

    /// Détermine si @param ARegion doit être segmentée ou non
    virtual bool isRegionToSplit() = 0;

  };
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("criterion-split.icc")
//******************************************************************************
#endif // CRITERION_SPLIT_HH
//******************************************************************************
