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
#ifndef MERGE_GREY_HH
#define MERGE_GREY_HH
//******************************************************************************
#include "criterion-merge.hh"
using namespace Map2d;
//******************************************************************************
//class CMergeGrey;
/** 
 * Fusionne deux régions adjacentes si la différence de leurs moyennes
 * de niveaux de gris est inférieure à un seuil défini
 */

namespace Map2d
{
  class CMergeGrey : public CCriterionMerge
  {

  protected:
    unsigned int FMergeThreshold; //Le seuil de merge
  
  public:
    // Constructeurs
    CMergeGrey(); //par défaut
    CMergeGrey(CRegion* ARegion1, CRegion* ARegion2, unsigned int AMergeThreshold);
  
    // Destructeur
    ~CMergeGrey();
  
    unsigned int getMergeThreshold() const;
    void setMergeThreshold(unsigned int AMergeThreshold);

    //******************************************************************************	  
    //Méthodes
    //******************************************************************************    

    /// Détermine si @param ARegion doit être éclatée ou non
    bool isRegionToMerge();
  };
}
//******************************************************************************
#include INCLUDE_INLINE("merge-grey.icc")
//******************************************************************************
#endif // MERGE_GREY_HH
//******************************************************************************
