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
#ifndef SPLIT_GREY_HH
#define SPLIT_GREY_HH
//******************************************************************************
#include "criterion-split.hh"
//******************************************************************************

namespace clustering
{
  //class CSplitGrey;
  /**
   * Détermine si l'on doit éclater une région d'après les seuils de merge
   * utilisés ensuite pour la fusion:
   * si gris max - gris min < seuil de merge , on n'éclate pas la région
   */

  class CSplitGrey : public CCriterionSplit
  {
  protected:
    unsigned int FMergeThreshold; //Le seuil de merge

  public:
    // Constructeurs
    CSplitGrey(); //par défaut
    CSplitGrey(Map2d::CRegion* ARegion, unsigned int AMergeThreshold);

    // Destructeur
    ~CSplitGrey();

    unsigned int getMergeThreshold() const;
    void setMergeThreshold(unsigned int AMergeThreshold);

    //******************************************************************************
    //Méthodes
    //******************************************************************************

    /// Détermine si @param ARegion doit être éclatée ou non
    bool isRegionToSplit();
  };
} // namespace clustering
//******************************************************************************
#include INCLUDE_INLINE("split-grey.icc")
//******************************************************************************
#endif // SPLIT_GREY_HH
//******************************************************************************
