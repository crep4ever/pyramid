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
bool CRegion::canMergeWithCavityConstraint(CTopologicalMap* m,
					   CDart* d,
					   int ACavity)
{
  CRegion *r1 = m->getRegion(d);
  CRegion *r2 = m->getRegion(m->beta2(d));
  if (r1 == r2 || r1->isInfiniteRegion() || r2->isInfiniteRegion())
    return false;

  int mInner = m->getNewMark();
  int mProcessed = m->getNewMark();

  for (CDynamicCoverage1 dIt(m, d); dIt.cont(); ++dIt)
    {
      if (m->getRegion(m->beta2(*dIt)) == r2)
	{
	  m->setMark(*dIt, mInner);
	  m->setMark(m->beta2(*dIt), mInner);
	  m->setMark(*dIt, mProcessed);
	  m->setMark(m->beta2(*dIt), mProcessed);
	}
    }

  CDart *dCC;
  CDart *dCur = d;
  unsigned int k = 0;
  do
    {
      if (!m->isMarked(dCur, mProcessed))
	{
	  k++;
	  // cover the connected component and mark all the dart as processed
	  dCC = dCur;
	  do
	    {
	      m->setMark(dCC, mProcessed);
	      dCC = m->beta1(dCC);
	      while (m->isMarked(dCC, mInner) || m->getRegion(m->beta2(dCC)) == r1)
		dCC = m->beta21(dCC);
	    }
	  while (!m->isMarked(dCC, mProcessed));
	}

      // find the next dart and go
      dCur = m->beta1(dCur);
      while (dCur != d && m->getRegion(m->beta2(dCur)) == r1)
	dCur = m->beta21(dCur);
    }
  while (dCur != d);

  // démarquer
  for (CDynamicCoverage1 dIt(m, d); dIt.cont(); ++dIt)
    {
      m->unsetMark(*dIt, mInner);
      m->unsetMark(*dIt, mProcessed);
      m->unsetMark(m->beta2(*dIt), mInner);
      m->unsetMark(m->beta2(*dIt), mProcessed);
    }
  for (CDynamicCoverage1 dIt(m, m->beta2(d)); dIt.cont(); ++dIt)
    {
      m->unsetMark(*dIt, mInner);
      m->unsetMark(*dIt, mProcessed);
      m->unsetMark(m->beta2(*dIt), mInner);
      m->unsetMark(m->beta2(*dIt), mProcessed);
    }

  m->freeMark(mInner);
  m->freeMark(mProcessed);

  int cavityR1 = r1->getCavityCount();
  int boundaryR1 = cavityR1 + 1;
  int cavityR2 = r2->getCavityCount();
  int boundaryR2 = cavityR2 + 1;
  int boundaryR = boundaryR1 + boundaryR2 + k - 2;
  int cavityR = boundaryR - 1;

  m->setLastCavityCount(cavityR);

  switch (ACavity)
    {
    case -2: // decrease
      //std::cout << "decrease : " << (cavityR <= cavityR1 + cavityR2) << std::endl;
      return cavityR <= cavityR1 + cavityR2;
    case -1: // constant
      //std::cout << "constant : " << (cavityR == cavityR1 + cavityR2) << std::endl;
      return cavityR == cavityR1 + cavityR2;
    default: // converge
      //std::cout << "converge : " << cavityR - ACavity << "/"<< cavityR1 + cavityR2 - ACavity << " " << (cavityR - ACavity <= cavityR1 + cavityR2 - ACavity) << std::endl;
      return std::abs(cavityR - ACavity) <= std::abs(cavityR1 + cavityR2 - ACavity);
    }
}
//******************************************************************************
