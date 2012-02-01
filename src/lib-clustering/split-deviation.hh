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
#ifndef MERGE_DEVIATION_HH
#define MERGE_DEVIATION_HH
//******************************************************************************
#include "criterion-split.hh"
#include <cmath>
//******************************************************************************
namespace Map2d
{
  //class CMergeDeviation;
  /**
   * Fusionne deux régions adjacentes si l'écart-type de leur région mère
   * est inférieur à un seuil défini.
   * Critère top-down: plus la région mère est homogène, plus on va adapter
   * le seuil pour mettre en évidence les détails. En fait, c'est comme
   * le critère merge-grey sauf que le seuil n'est pas définit par l'utilisateur
   * mais par l'écart-type de la région mère.
   */

  class CRegion;

  class CSplitDeviation : public CCriterionSplit
  {
  public:

    // Constructeurs
    CSplitDeviation(); //par défaut
    CSplitDeviation(CRegion* ARegion, unsigned int AMergeThreshold);

    // Destructeur
    virtual ~CSplitDeviation();

    //******************************************************************************
    //Méthodes
    //******************************************************************************

    /// Détermine si @param ARegion doit être éclatée ou non
    bool isRegionToSplit();

  protected:
    unsigned int FMergeThreshold; //Le seuil de merge


  };
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("split-deviation.icc")
//******************************************************************************
#endif // MERGE_DEVIATION_HH
//******************************************************************************
