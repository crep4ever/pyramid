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
#ifndef CRITERION_MERGE_HH
#define CRITERION_MERGE_HH
//******************************************************************************

namespace Map2d
{
  /**
   * \file criterion-merge.hh
   *
   * Abstract class for merging criteria of a region.
   * Merging is a local operation comparing two regions that share the same parent.
   *
   * \author Romain Goffe
   */

  class CRegion;

  class CCriterionMerge
  {
  public :

    CRegion* FRegion1;
    CRegion* FRegion2;

    // Constructeurs
    CCriterionMerge(); //par défaut
    CCriterionMerge(CRegion* ARegion1, CRegion* ARegion2);

    // Destructeur
    virtual ~CCriterionMerge() = 0;

    // Accesseurs
    CRegion* getRegion1() const;
    void setRegion1(CRegion* ARegion);

    CRegion* getRegion2() const;
    void setRegion2(CRegion* ARegion);

    //******************************************************************************
    //Méthodes
    //******************************************************************************

    /// Détermine si @param ARegion1 et @param ARegion2 doivent être fusionnées
    virtual bool isRegionToMerge() = 0;

  };
}
//******************************************************************************
#include INCLUDE_INLINE("criterion-merge.icc")
//******************************************************************************
#endif // CRITERION_MERGE_HH
//******************************************************************************
