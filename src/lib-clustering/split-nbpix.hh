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
#ifndef SPLIT_NBPIX_HH
#define SPLIT_NBPIX_HH
//******************************************************************************
#include "criterion-split.hh"
//******************************************************************************

namespace clustering
{
  /**
   * Split une région suivant sa taille en pixels
   */

  class CSplitNbPix : public CCriterionSplit
  {
  protected:
    unsigned int FThreshold; //Le seuil (en nombre de pixels)

  public:
    // Constructeurs
    CSplitNbPix(); //par défaut
    CSplitNbPix(Map2d::CRegion *ARegion, unsigned int ANbPix);

    // Destructeur
    virtual ~CSplitNbPix();

    unsigned int getThreshold() const;
    void setThreshold(unsigned int AThreshold);

    //******************************************************************************
    //Méthodes
    //******************************************************************************

    /// Détermine si @param ARegion doit être éclatée ou non
    bool isRegionToSplit();
  };
} // namespace clustering
//******************************************************************************
#include INCLUDE_INLINE("split-nbpix.icc")
//******************************************************************************
#endif // SPLIT_NBPIX_HH
//******************************************************************************
