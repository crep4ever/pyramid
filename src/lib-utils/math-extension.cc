/*
 * lib-mapkernel : Un noyau de 3-cartes et des opérations.
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
#include "math-extension.hh"
#include "vertex.hh"
//******************************************************************************
const TCoordinate EPSILON = 1E-4;
//******************************************************************************
TCoordinate distancePointToLine(const CVertex& APoint,
				const CVertex& AL1, const CVertex& AL2)
{
  assert(AL1!=AL2);

  return fabs(det(AL2-AL1, AL1-APoint)) / (AL2-AL1).norm();
}
//------------------------------------------------------------------------------
TCoordinate getAngle(const CVertex& AV1, const CVertex& AV2, const CVertex& AV3)
{
  CVertex v1(AV2-AV1); // Vecteur (AV1,AV2)
  CVertex v2(AV3-AV2); // Vecteur (AV2,AV3)

  // Formule:  v1.v2 = Norm(v1) * Norm(v2) * Cos ( Angle(v1,v2) )

  if (!v1.isNull()) v1 /= v1.norm();
  if (!v2.isNull()) v2 /= v2.norm();

  double cosinus = v1.dot(v2);

  if (cosinus > +1) cosinus = +1;
  if (cosinus < -1) cosinus = -1;

  TCoordinate angle = dAcos(cosinus);

  return ((v1.normVectoriel(v2))>0 ? +angle : -angle);
}
//------------------------------------------------------------------------------
TCoordinate det(const CVertex & ALine1, const CVertex & ALine2)
{
  return (ALine1.getX()*ALine2.getY() - ALine1.getY()*ALine2.getX());
}
//******************************************************************************
