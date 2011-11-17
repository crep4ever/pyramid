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
#include "map-generic.hh"
using namespace Map2d;
//******************************************************************************
void CMapGeneric::markOrbit(CDart * ADart, TOrbit AOrbit, int AMarkNumber)
{
  assert(ADart!=NULL);
  CCoverage * cov= NULL;
  
  if ( AOrbit==ORBIT_12 || AOrbit==ORBIT_VERTEX)
    {
      cov = getBasicDynamicCoverage(ADart, AOrbit, AMarkNumber);
      while (cov->cont()) ++(*cov);
    }
   else
    {
      cov = getDynamicCoverage(ADart, AOrbit);      
      while (cov->cont())
	setMark((*cov)++, AMarkNumber);
    }

  delete cov;
}
//******************************************************************************
void CMapGeneric::unmarkOrbit(CDart * ADart, TOrbit AOrbit, int AMarkNumber)
{
  assert(ADart!=NULL);

  CCoverage * cov= getDynamicCoverage(ADart, AOrbit);

  while (cov->cont())
    unsetMark((*cov)++, AMarkNumber);

  delete cov;
}
//******************************************************************************
void CMapGeneric::halfMarkOrbit(CDart * ADart, TOrbit AOrbit, int AMarkNumber)
{
  assert(ADart!=NULL);
  //  assert(AOrbit>=ORBIT_SELF && AOrbit<=ORBIT_CC);
  assert(AOrbit>=ORBIT_SELF && AOrbit<=ORBIT_CC && AOrbit!=ORBIT_VERTEX);
  // Pour le moment marche pas pour les orbit vertex... TODO

  int selected = getNewMark();
  bool usedDim[3];

  usedDim[0]=
    (AOrbit == ORBIT_0)  || (AOrbit == ORBIT_1)  ||
    (AOrbit == ORBIT_12);

  usedDim[1]= usedDim[0];

  usedDim[2]=
    (AOrbit == ORBIT_2)  ||
    (AOrbit == ORBIT_12); 
  
  CCoverage * cov= getDynamicCoverage(ADart, AOrbit);

  setMark(**cov, selected);
  setMark(**cov, AMarkNumber);

  for (; cov->cont(); ++(*cov))
    if (!isMarked(**cov, selected))
      {
	for (int dim=0; dim<=2; ++dim)
	  if (usedDim[dim] && !isFree(**cov, dim))
	    {
	      setMark(beta(**cov, dim), selected);
	      setMark(beta(**cov, dim), AMarkNumber);
	    }
      }

  for (cov->reinit(); cov->cont(); ++(*cov))
    unsetMark((**cov), selected);
  
  delete cov;

  freeMark(selected);
}
//******************************************************************************
void CMapGeneric::halfUnmarkOrbit(CDart * ADart, TOrbit AOrbit, int AMarkNumber)
{
  negateMaskMark(AMarkNumber);
  halfMarkOrbit(ADart, AOrbit, AMarkNumber);
  negateMaskMark(AMarkNumber);
}
//******************************************************************************
void CMapGeneric::unmarkAll(int AMarkNumber)
{
  CDynamicCoverageAll it(this);

  while (it.cont())
    unsetMark(it++, AMarkNumber);
}
//******************************************************************************
int CMapGeneric::markIncidentCells(TOrbit AOrbit, int AMarkNumberSrce,
				   int AMarkNumberDest)
{
  assert(AMarkNumberSrce!=AMarkNumberDest);

  int treated = getNewMark();
  int nb_cells = 0;
  
  CDynamicCoverageAll it(this);

  for (; it.cont(); ++it)
    if (!isMarked(*it, treated))
      {
	if (isMarked(*it, AMarkNumberSrce))
	  {
	    markOrbit(*it, AOrbit, AMarkNumberDest);
	    markOrbit(*it, AOrbit, treated        );
	    ++nb_cells;
	  }
	else
	  setMark(*it, treated);
      }

  negateMaskMark(treated);
  freeMark(treated);

  return nb_cells;
}
//******************************************************************************
int CMapGeneric::markIncidentCells(TOrbit AOrbit, int AMarkNumber)
{
  int mark = getNewMark();

  markCopy(AMarkNumber, mark);
  int nb_cells = markIncidentCells(AOrbit, mark, AMarkNumber);
  unmarkAll(mark);

  freeMark(mark);

  return nb_cells;
}
//******************************************************************************
void CMapGeneric::markCopy(int AMarkNumberFrom, int AMarkNumberTo)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);

  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    setMarkTo(*it, AMarkNumberTo, isMarked(*it, AMarkNumberFrom));
}
//******************************************************************************
void CMapGeneric::markCopy(int AMarkNumberFrom, int AMarkNumberTo,
			    CDart * ADart, TOrbit AOrbit)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);
  assert(ADart!=NULL);

  CCoverage * cov = getDynamicCoverage(ADart, AOrbit);

  for (; cov->cont(); ++(*cov))
    setMarkTo(**cov, AMarkNumberTo, isMarked(**cov, AMarkNumberFrom));

  delete cov;
}
//******************************************************************************
void CMapGeneric::markAdd(int AMarkNumberFrom, int AMarkNumberTo)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);

  CDynamicCoverageAll it(this);

  for (; it.cont(); ++it)
    if (isMarked(*it, AMarkNumberFrom))
      setMark(*it, AMarkNumberTo);
}
//******************************************************************************
void CMapGeneric::markAdd(int AMarkNumberFrom, int AMarkNumberTo,
			   CDart * ADart, TOrbit AOrbit)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);
  assert(ADart!=NULL);

  CCoverage * cov = getDynamicCoverage(ADart, AOrbit);

  for (; cov->cont(); ++(*cov))
    if (isMarked(**cov, AMarkNumberFrom))
      setMark(**cov, AMarkNumberTo);

  delete cov;
}
//******************************************************************************
void CMapGeneric::markAnd(int AMarkNumberFrom, int AMarkNumberTo)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);

  CDynamicCoverageAll it(this);

  for (; it.cont(); ++it)
    if (!isMarked(*it, AMarkNumberFrom))
      unsetMark(*it, AMarkNumberTo);
}
//******************************************************************************
void CMapGeneric::markAnd(int AMarkNumberFrom, int AMarkNumberTo,
			   CDart * ADart, TOrbit AOrbit)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);
  assert(ADart!=NULL);

  CCoverage * cov = getDynamicCoverage(ADart, AOrbit);

  for (; cov->cont(); ++(*cov))
    if (!isMarked(**cov, AMarkNumberFrom))
      unsetMark(**cov, AMarkNumberTo);

  delete cov;
}
//******************************************************************************
void CMapGeneric::markSub(int AMarkNumberFrom, int AMarkNumberTo)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);

  CDynamicCoverageAll it(this);

  for (; it.cont(); ++it)
    if (isMarked(*it, AMarkNumberFrom))
      unsetMark(*it, AMarkNumberTo);
}
//******************************************************************************
void CMapGeneric::markSub(int AMarkNumberFrom, int AMarkNumberTo,
			   CDart * ADart, TOrbit AOrbit)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);
  assert(ADart!=NULL);

  CCoverage * cov = getDynamicCoverage(ADart, AOrbit);

  for (; cov->cont(); ++(*cov))
    if (isMarked(**cov, AMarkNumberFrom))
      unsetMark(**cov, AMarkNumberTo);

  delete cov;
}
//******************************************************************************
void CMapGeneric::markMove(int AMarkNumberFrom, int AMarkNumberTo)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);

  CDynamicCoverageAll it(this);

  for (; it.cont(); ++it)
    if (isMarked(*it, AMarkNumberFrom))
      {
	setMark  (*it, AMarkNumberTo  );
	unsetMark(*it, AMarkNumberFrom);
      }
}
//******************************************************************************
void CMapGeneric::markMove(int AMarkNumberFrom, int AMarkNumberTo,
			    CDart * ADart, TOrbit AOrbit)
{
  assert(AMarkNumberFrom!=AMarkNumberTo);
  assert(ADart!=NULL);

  CCoverage * cov = getDynamicCoverage(ADart, AOrbit);

  for (; cov->cont(); ++(*cov))
    if (isMarked(**cov, AMarkNumberFrom))
      {
	setMark  (**cov, AMarkNumberTo  );
	unsetMark(**cov, AMarkNumberFrom);
      }

  delete cov;
}
//******************************************************************************
bool CMapGeneric::isWholeCellMarked(CDart * ADart, TOrbit AOrbit,
				     int AMarkNumber)
{
  assert(ADart!=NULL);

  bool wholeCellMarked = true;
  CCoverage * it = getDynamicCoverage(ADart, AOrbit);

  for (; it->cont() && wholeCellMarked; ++(*it))
    if (!isMarked(**it, AMarkNumber))
      wholeCellMarked = false;

  delete it;

  return wholeCellMarked;
}
//******************************************************************************
