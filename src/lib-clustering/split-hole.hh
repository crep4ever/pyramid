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
#ifndef SPLIT_HOLE_HH
#define SPLIT_HOLE_HH
//******************************************************************************
#include "criterion-split.hh"
//#include "pyramidal-region.hh"
//******************************************************************************
namespace clustering
{
  //class CSplitGrey;
  /**
   * Détermine si l'on doit éclater une région d'après le critère topologique suivant:
   * on split la région si sa région mère a un trou (ie, il existe un first son)
   */
  class CSplitHole : public CCriterionSplit
  {
  public:

    // Constructeurs
    CSplitHole(); //par défaut
    CSplitHole(Map2d::CRegion* ARegion);
    // Destructeur
    ~CSplitHole();

    //******************************************************************************
    //Méthodes
    //******************************************************************************

    /// Détermine si @param ARegion doit être éclatée ou non
    bool isRegionToSplit();
  };
} // namespace clustering
//******************************************************************************
#include INCLUDE_INLINE("split-hole.icc")
//******************************************************************************
#endif // SPLIT_GREY_HH
//******************************************************************************
