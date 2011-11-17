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
#include "inline-macro.hh"
#include "region.hh"
#include INCLUDE_NON_INLINE("region.icc")

using namespace Map2d;
//******************************************************************************
CRegion::~CRegion()
{
  //if ( existSon()        ) delete getFirstSon  ();
  //if ( existBrother()    ) delete getBrother   ();
  //if ( existNextSameCC() ) delete getNextSameCC();
}
//------------------------------------------------------------------------------
void CRegion::draw()
{
   std::cout << "================= Arbre d'inclusion =================" 
             << std::endl;

   std::cout << "région de départ de l'arbre d'inclusion: " << this 
            << std::endl;

   CDynamicCoverageRegions it(this);
   while (it.cont())
   {
      if (it->isInfiniteRegion()) std::cout << "Infinite region : ";
      else std::cout << it->getId() << " : ";

      CRegion* prev = NULL;
      std::cout << "( ";
      for (CDynamicCoverageSons it2(*it); it2.cont(); ++it2)
      {
         if (prev != NULL && prev->getNextSameCC() != *it2)
            std::cout << ") ( ";

         std::cout << it2->getId() << " ";

         prev = *it2;
      }

      std::cout << ")\n";

      ++it;
   }
   std::cout << "============== Fin d'affichage de l'arbre ===========" 
             << std::endl;
}
//------------------------------------------------------------------------------
CInfiniteRegion::~CInfiniteRegion()
{}
//------------------------------------------------------------------------------
CDynamicCoverageCCSons::~CDynamicCoverageCCSons()
{}
//------------------------------------------------------------------------------
CDynamicCoverageSons::~CDynamicCoverageSons()
{}
//------------------------------------------------------------------------------
CDynamicCoverageCCRegions::~CDynamicCoverageCCRegions()
{}
//------------------------------------------------------------------------------
CDynamicCoverageRegions::~CDynamicCoverageRegions()
{}
//------------------------------------------------------------------------------
CDynamicCoverageAllCCRegions::~CDynamicCoverageAllCCRegions()
{}
//------------------------------------------------------------------------------
CDynamicCoverageAllRegions::~CDynamicCoverageAllRegions()
{}
//******************************************************************************


