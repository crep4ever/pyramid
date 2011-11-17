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
#include <iostream>
#include <sstream>
using namespace std;
using namespace Map2d;
//******************************************************************************
int CMapGeneric::degree(CDart* ADart, int ACellDim)
{
  assert( ADart!=NULL );
  assert( 0<=ACellDim && ACellDim<2 );

  TOrbit countOrbit = ORBIT_CELL[ACellDim+1];

  int nbIncident = 0;
  int treated = getNewMark();

  CCoverage * cov = getDynamicCoverage(ADart, ORBIT_CELL[ACellDim]);
  for (; cov->cont(); ++(*cov))
    if (!isMarked(**cov, treated))
      {
	++nbIncident;
	markOrbit(**cov, countOrbit, treated);
      }

  cov->reinit();
  for (; cov->cont(); ++(*cov))
    if (isMarked(**cov, treated))
      unmarkOrbit(**cov, countOrbit, treated);
 
  delete cov;

  freeMark(treated);

  assert(nbIncident!=0);

  return nbIncident;
}
//******************************************************************************
// int CMapGeneric::localDegree(CDart* ADart, int ACellDim)
// {
//   assert( ADart!=NULL );
//   assert( 0<=ACellDim && ACellDim<2 );

//   TOrbit countOrbit = ORBIT_NONE;

//   switch (ACellDim)
//     {
//     case 0: countOrbit = ORBIT_23; break;
//     case 1: countOrbit = ORBIT_3; break;
//     case 2: countOrbit = ORBIT_1; break;
//     default: assert(false);
//     }
  
//   int nbIncident = 0;
//   int treated = getNewMark();

//   CCoverage * cov = getDynamicCoverage(ADart, ORBIT_CELL[ACellDim]);
//   for (; cov->cont(); ++(*cov))
//     if (!isMarked(**cov, treated))
//       {
// 	++nbIncident;
// 	if ( ACellDim==0 )
// 	  halfMarkOrbit(**cov, countOrbit, treated);
// 	else
// 	  markOrbit(**cov, countOrbit, treated);
//       }

//   for (cov->reinit(); cov->cont(); ++(*cov))
//     if (isMarked(**cov, treated))
//       {
// 	if ( ACellDim==0 )
// 	  halfUnmarkOrbit(**cov, countOrbit, treated);
// 	else
// 	  unmarkOrbit(**cov, countOrbit, treated);
//       }

//   delete cov;

//   freeMark(treated);

//   assert(nbIncident!=0);

//   return nbIncident;
//} 
//******************************************************************************
void CMapGeneric::countCells(int AMarkNumber,
			     unsigned int* ANb0, unsigned int* ANb1,
			     unsigned int* ANb2, unsigned int* ANb3,
			     unsigned int* ANbDarts)
{
  unsigned int * count[4] = { ANb0, ANb1, ANb2, ANb3};
  unsigned int dim[4];
  int cellMark[4];
  int nbAsked = 0;
  unsigned int nbDarts = 0;
  int i;
  
  // Initialisations:
  for (i=0; i<4; ++i)
    if ( count[i]!=NULL )
      {
	count[nbAsked] = count[i];
	* count[nbAsked] = 0;
	dim[nbAsked] = i;
	cellMark[nbAsked] = getNewMark();
	++nbAsked;
      }

  // Comptage et marquage des cellules:
  CDynamicCoverageAll it(this);
  for (; it.cont(); ++it)
    {
      if (isMarked(*it, AMarkNumber))
	{
	  ++nbDarts;
	  for (i=0; i<nbAsked; ++i)
	    if (!isMarked(*it, cellMark[i]))
	      {
		++ * count[i];
		markOrbit(*it, ORBIT_CELL[dim[i]], cellMark[i]);
	      }
	}
    }
  
  if (ANbDarts != NULL)
    * ANbDarts = nbDarts;

  // Démarquage:
  for (it.reinit(); it.cont(); ++it)
    for (i=0; i<nbAsked; ++i)
      if (isMarked(*it, cellMark[i]))
	unmarkOrbit(*it, ORBIT_CELL[dim[i]], cellMark[i]);

  // Libérations:
  for (i=0; i<nbAsked; ++i)
    freeMark(cellMark[i]);
}
//******************************************************************************
void CMapGeneric::countCells(CDart* ADart, TOrbit AOrbit,
			     unsigned int* ANb0, unsigned int* ANb1,
			     unsigned int* ANb2, unsigned int* ANb3,
			     unsigned int* ANbDarts)
{
  assert(ADart != NULL);

  unsigned int * count[4] = { ANb0, ANb1, ANb2, ANb3 };
  int dim[4];
  int cellMark[4];
  int nbAsked = 0;
  unsigned int nbDarts = 0;
  int i;

  // Initialisations:
  for (i=0; i<4; ++i)
    if (count[i] != NULL)
      {
	count[nbAsked] = count[i];
	* count[nbAsked] = 0;
	dim[nbAsked] = i;
	cellMark[nbAsked] = getNewMark();
	++nbAsked;
      }

  CCoverage * cov = getDynamicCoverage(ADart, AOrbit);

  // Comptage et marquage des cellules:
  for (; cov->cont(); ++(*cov), ++nbDarts)
    for (i=0; i<nbAsked; ++i)
      if (!isMarked(**cov, cellMark[i]))
	{
	  ++ * count[i];
	  markOrbit(**cov, ORBIT_CELL[dim[i]], cellMark[i]);
	}

  if (ANbDarts != NULL)
    * ANbDarts = nbDarts;

  // Démarquage:
  for (cov->reinit(); cov->cont(); ++(*cov))
    for (i=0; i<nbAsked; ++i)
      if (isMarked(**cov, cellMark[i]))
	unmarkOrbit(**cov, ORBIT_CELL[dim[i]], cellMark[i]);

  // Libérations:
  delete cov;

  for (i=0; i<nbAsked; ++i)
    freeMark(cellMark[i]);
}
//******************************************************************************
void CMapGeneric::getGlobalCharacteristics(unsigned int * ANbDarts,
					   unsigned int * ANbVertices,
					   unsigned int * ANbEdges,
					   unsigned int * ANbFaces,
					   unsigned int * ANbCC)
{
  int mark = getNewMark();

  negateMaskMark(mark);

  countCells(mark, ANbVertices, ANbEdges, ANbFaces, ANbCC, ANbDarts);

  negateMaskMark(mark);
  freeMark(mark);
}
//******************************************************************************
// void CMapGeneric::getFaceCharacteristics( CDart* ADart,
// 					  unsigned int* ANbDarts,
// 					  unsigned int* ANbVertices,
// 					  unsigned int* ANbEdges )
// {
//   assert(ADart != NULL);

//   unsigned int nc[3];  // nbCells

//   // Réservation des ressources:
//   int memoBeta3 = getNewDirectInfo();
//   assert( memoBeta3!=-1 );
  
//   CDynamicCoverageVolume vol(this, ADart);
//   // Isolation du volume:
//   for (; vol.cont(); ++vol)
//     {
//       setDirectInfo(*vol, memoBeta3, beta3(*vol));
//       (*vol)->setFree3();
//     }
  
//   countCells  (ADart, ORBIT_FACE, &nc[1], &nc[2], NULL, NULL, NULL, &nc[0]);
  
//   // Restauration des 3-coutures:
//   for (vol.reinit(); vol.cont(); ++vol)
//     {
//       if (getDirectInfoAsDart(*vol, memoBeta3)!=NULL)
// 	(*vol)->setBeta3(getDirectInfoAsDart(*vol, memoBeta3));
//     }
  
//   // Libération des ressources:
//   freeDirectInfo(memoBeta3);
  
//   if (ANbDarts       != NULL) * ANbDarts    	= nc[0];
//   if (ANbVertices    != NULL) * ANbVertices 	= nc[1];
//   if (ANbEdges       != NULL) * ANbEdges    	= nc[2];  
//}
//******************************************************************************

//******************************************************************************
bool CMapGeneric::isConnex()
{
  bool connex = true;
  int  reached = getNewMark();
  bool first = true;
  
  // Parcours des brins de la G-carte:
  for (CDynamicCoverageAll it(this); it.cont() && connex; ++it)
    {
      if ( first )
	{
	  // Marquage d'une composante connexe:
	  markOrbit(*it, ORBIT_CC, reached);
	  first = false;
	}
      else
	{
	  if (! isMarked(*it, reached))
	    connex = false;
	}
    }

  if (connex)
    negateMaskMark(reached);
  else
    unmarkAll(reached);

  freeMark(reached);

  return connex;
}
//******************************************************************************
bool CMapGeneric::checkTopology()
{
  // 1: On vérifie que pour tout brin B, on a beta0(beta1(B)) == B
  //    et beta1(beta0(B)) == B.
  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      if (*it != beta(beta(*it,1),0))
	{
	  cerr << "CMapGeneric::integrity: Le brin " << *it
	       << " ne vérifie pas la contrainte "
	       << "beta10 (brin) == brin." << endl;
	  
	  return false;
	}

      if (*it != beta(beta(*it,0),1))
	{
	  cerr << "CMapGeneric::integrity: Le brin " << *it
	       << " ne vérifie pas la contrainte "
	       << "beta01 (brin) == brin." << endl;
	  
	  return false;
	}
    }      

  // 2: On vérifie que pour tout brin B, on beta B == beta(beta(B,i),i)
  int dim=2; 
    for (CDynamicCoverageAll it(this); it.cont(); ++it)
      if (!isFree(*it, dim) && *it != beta(beta(*it,dim),dim))
	{
	  cerr << "CMapGeneric::integrity: Le brin " << *it
	       << " ne vérifie pas la contrainte "
	       << "beta" << dim << "(beta" << dim << "(brin)) == brin." << endl;
	  
	  return false;
	}
  
  return true;
}
//******************************************************************************
