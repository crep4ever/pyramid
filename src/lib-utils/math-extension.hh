/*
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/
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
#ifndef MATH_EXTENSION_HH
#define MATH_EXTENSION_HH
//******************************************************************************
#include <cassert>
#include <cmath> // Pour la macro M_PI et fabs
//******************************************************************************
class CVertex;
//******************************************************************************
/**
 * Bibliothèque de fonctions mathématiques.
 *
 * Conventions:
 *
 * alpha.b signifie  produit scalaire  entre alpha et b
 *   résultat: ax*bx + ay*by + az*bz
 * alpha*b signifie  produit vectoriel entre alpha et b
 *   résultat: { ay*bz-az*by, az*bx-ax*bz, ax*by-ay*bx }
 *
 * @author MOKA TEAM
 */
//******************************************************************************
// @name Macros
// @{

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif //MIN

#ifndef  MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif //MAX

#ifndef M_PI
#define M_PI (3.141592653589793238462643383279)
#endif // M_PI

// @}
//******************************************************************************
// @name Types de base
// @{

/**
 * TCoordinate est le type de base utilisé dans tous les calculs mathématiques.
 * Les coordonnées des sommets, les composantes d'un vecteur, les cellules
 * d'une matrice, etc, sont de ce type.
 */
typedef double TCoordinate;

/**
 * EPSILON représente la tolérance utilisée pour la comparaison de deux valeurs.
 */
extern const TCoordinate EPSILON;

/**
 * TFunctionType est un type utilisé principalement par la classe CGMapVertex
 * pour effectuer des déformations pondérées.
 * Ces déformations prennent en paramètre une classe de fonction correspondant
 * à une valeur de type TFunctionType.
 *
 * Pour plus d'informations, voir CGMapVertex::applyFunctionOnParameter.
 */
typedef enum
{
  FUNCTION_LIN  , //@< x -> x
  FUNCTION_QUAD , //@< x -> x^2
  FUNCTION_EXP  , //@< x -> e^x
  FUNCTION_LOG  , //@< x -> ln(x) si x>0, x -> 0
  FUNCTION_SIN  , //@< x -> sin(x)
  FUNCTION_COS    //@< x -> cos(x)
}
TFunctionType;

/**
 * Le nombre de classes de fonctions répertoriées par TFunctionType.
 */
static const int NB_FUNCTION_TYPES = 6;

// @}
//******************************************************************************
// @name Fonctions mathématiques de base
// @{

/**
 * Calcule le carré d'un entier ou d'un flottant.
 *
 * @param AValue Un nombre quelconque (entier ou flottant)
 * @return AValue au carré
 */
inline int sqr(int AValue)
{
  return AValue*AValue;
}

inline float sqr(float AValue)
{
  return AValue*AValue;
}

inline double sqr(double AValue)
{
  return AValue*AValue;
}

/**
 * Teste si AValue est positif ou nul (à epsilon près).
 *
 * @param AValue Une valeur quelconque
 * @return true ssi AValue > 0
 */
inline bool isPositive(const TCoordinate& AValue)
{
  // return AValue > 0 || areEqual(fabs(AValue), 0.0);
  return AValue > -EPSILON;
}

/**
 * Teste si AValue est négatif ou nul (à epsilon près).
 *
 * @param AValue Une valeur quelconque
 * @return true ssi AValue > -EPSILON
 */
inline bool isNegative(const TCoordinate& AValue)
{
  // return AValue < 0 || areEqual(fabs(AValue), 0.0);
  return AValue < +EPSILON;
}

/**
 * Teste si AValue est nul (à epsilon près).
 *
 * @param AValue Une valeur quelconque
 * @return true ssi abs(AValue)<EPSILON
 */
inline bool isZero(const TCoordinate& AValue)
{
  // return areEqual(fabs(AValue), 0.0);
  return fabs(AValue) < EPSILON;
}
  
/**
 * Donne le signe de AValue.
 *
 * @param AValue Une valeur quelconque
 * @return -1 si AValue<=-EPSILON, +1 si AValue>=+EPSILON, 0 sinon
 */
inline int sign(const TCoordinate& AValue)
{
  if (isZero(AValue))
    return 0;

  return AValue<0 ? -1 : +1;
}

/**
 * Combine les signes ASign1 et ASign2.
 * La valeur retournée vaut ASign1*ASign2
 *
 * @param ASign1 Un signe (-1, 0 ou 1)
 * @param ASign2 Un signe (-1, 0 ou 1)
 * @return La combinaison des signes ASign1 et ASign2
 */
inline int combineSigns(int ASign1, int ASign2)
{
  assert(-1 <= ASign1 && ASign1 <= +1);
  assert(-1 <= ASign2 && ASign2 <= +1);

  if (ASign1==0 || ASign2==0)
    return 0;

  return ASign1==ASign2 ? +1 : -1;
}

/**
 * Teste si AValue1 est strictement inférieur à AValue2, à EPSILON près.
 * (renvoie vrai si AValue1 + EPSILON < AValue2)
 *
 * @param AValue1 Une valeur quelconque
 * @param AValue2 Une valeur quelconque
 * @return Un booléen
 */
inline bool isLessThan(const TCoordinate& AValue1, const TCoordinate& AValue2)
{
  return AValue1 + EPSILON < AValue2;
}
  
/**
 * Teste si AValue1 et AValue2 sont égaux, à EPSILON près.
 *
 * @param AValue1 Une valeur quelconque
 * @param AValue2 Une valeur quelconque
 * @return Un booléen
 */
inline bool areEqual(const TCoordinate& AValue1, const TCoordinate& AValue2)
{
  return fabs(AValue1 - AValue2) < EPSILON;
}

/**
 * Convertit AAngle (exprimé en radians) en degrés.
 *
 * @param AAngle Une valeur quelconque
 * @return AAngle*180/PI
 */
inline TCoordinate deg(const TCoordinate& AAngle)
{
  return AAngle/M_PI*180.0;
}

/**
 * Convertit AAngle (exprimé en degrés) en radians.
 *
 * @param AAngle Une valeur quelconque
 * @return AAngle*PI/180
 */
inline TCoordinate rad(const TCoordinate& AAngle)
{
  return AAngle*M_PI/180.0;
}

/**
 * Calcule le sinus de AAngle. AAngle est exprimé en degrés.
 *
 * @param AAngle Une valeur quelconque
 * @return sin(rad(AAngle))
 */
inline TCoordinate dSin(const TCoordinate& AAngle)
{
  return sin(rad(AAngle));
}

/**
 * Calcule le cosinus de AAngle. AAngle est exprimé en degrés.
 *
 * @param AAngle Une valeur quelconque
 * @return cos(rad(AAngle))
 */
inline TCoordinate dCos(const TCoordinate& AAngle)
{
  return cos(rad(AAngle));
}

/**
 * Calcule la tangente de AAngle. AAngle est exprimé en degrés.
 *
 * @param AAngle Une valeur quelconque
 * @return tan(rad(AAngle))
 */
inline TCoordinate dTan(const TCoordinate& AAngle)
{
  return tan(rad(AAngle));
}

/**
 * Calcule l'arc-sinus de AValue. Le résultat est exprimé en degrés.
 *
 * @param AValue Une valeur quelconque
 * @return deg(asin(AValue))
 */
inline TCoordinate dAsin(const TCoordinate& AValue)
{
  return deg(asin(AValue));
}

/**
 * Calcule l'arc-cosinus de AValue. Le résultat est exprimé en degrés.
 *
 * @param AValue Une valeur quelconque
 * @return deg(acos(AValue))
 */
inline TCoordinate dAcos(const TCoordinate& AValue)
{
  return deg(acos(AValue));
}

/**
 * Calcule l'arc-tangente de AValue. Le résultat est exprimé en degrés.
 *
 * @param AValue Une valeur quelconque
 * @return deg(atan(AValue))
 */
inline TCoordinate dAtan(const TCoordinate& AValue)
{
  return deg(atan(AValue));
}

/**
 * Calcule la valeur A du segment ]-180,180] telle que A = AAngle + 360*k.
 *
 * @param AValue Une valeur quelconque
 * @return La valeur A du segment ]-180,180] telle que A = AAngle + 360*k.
 */
inline TCoordinate limit(const TCoordinate& AAngle)
{
  TCoordinate angle = AAngle;

  if (angle<0)
    while (angle<=-180)
      angle += 360;
  else
    while (angle> +180)
      angle -= 360;

  return angle;
}

/**
 * Calcule l'angle formé entre le vecteur OX et le vecteur (Ax,Ay).
 *
 * @param Ax Une valeur quelconque
 * @param Ay Une valeur quelconque
 * @return angle(OX,(Ax,Ay))
 */
inline TCoordinate angle(const TCoordinate& Ax, const TCoordinate& Ay)
{
  if (isZero(Ax))
    {
      if (isZero(Ay))
	return 0;

      if (Ay<0)
	return -90;
      else
	return +90;
    }

  if (Ax>0)
    return dAtan(Ay/Ax);
  else
    return dAtan(Ay/Ax)+180.0;
}

/**
 * @return L'angle entre les 2 vecteurs (AV1,AV2) et (AV2,AV3)
 */
TCoordinate getAngle(const CVertex& AV1,const CVertex& AV2,const CVertex& AV3);

/**
 * Calcule le déterminant de la matrice 2*2 donnée par les sommets ALine1,
 * ALine2 .
 *
 * @param ALine- Des sommets correspondant aux lignes de la matrice
 * @return Le déterminant de la matrice 2*2
 */
TCoordinate det(const CVertex& ALine1, const CVertex& ALine2);

/**
 * @return la distance de APoint à la droite (AL1, AL2)
 */
TCoordinate distancePointToLine(const CVertex& APoint,
				const CVertex& AL1, const CVertex& AL2);

/**
 * @param Ax Une valeur entière positive ou nulle.
 * @param Ay Une valeur entière positive ou nulle.
 * @result Le plus grand commun diviseur de Ax et Ay
 */
inline int pgcd(int Ax, int Ay)
{
  assert(0 <= Ax);
  assert(0 <= Ay);

  if (Ax < Ay)
    {
      int temp = Ax;
      Ax = Ay;
      Ay = temp;
    }

  while (Ay != 0)
    {
      int temp = Ax % Ay;
      Ax = Ay;
      Ay = temp;
    }

  return Ax;
}

// @}
//******************************************************************************
#endif // MATH_EXTENSION_HH
//******************************************************************************
