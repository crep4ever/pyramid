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
#ifndef VERTEX_HH
#define VERTEX_HH
//******************************************************************************
#include "math-extension.hh"
#include "inline-macro.hh"
#include "doublet.hh"

#include <iostream>
//******************************************************************************
/**
 * Définit la classe CVertex qui représente un sommet 2D.
 * Les instances de cette classe peuvent aussi être utilisées comme
 * des vecteurs 2D.
 *
 * Conventions:
 *
 * alpha.b signifie  produit scalaire  entre alpha et b
 *     résultat: ax*bx + ay*by 
 *
 *Inutile en 2D:
 * alpha*b signifie  produit vectoriel entre alpha et b
 *     résultat: { ay*bz-az*by, az*bx-ax*bz, ax*by-ay*bx }
 *
 * @author MOKA TEAM
 */
class CVertex
{
private:
  TCoordinate FCoord[2];

public:
  // @name Constructeurs et destructeur
  // @{

  /**
   * Constructeur par défaut
   * Construction d'une nouvelle instance de la classe, initialisée
   * au sommet origine (0,0,0).
   *
   * @return Une nouvelle instance de la classe
   */
  CVertex();

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (Ax,Ay).
   *
   * @param Ax La première  coordonnée du sommet
   * @param Ay La deuxième  coordonnée du sommet
   */
  CVertex(TCoordinate Ax, TCoordinate Ay);

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (ATab[0],ATab[1]).
   *
   * @param ATab Un tableau des 2 coordonnées du sommet
   */
  CVertex(TCoordinate ATab[2]);

  /**
   * Constructeur par copie
   * Construction d'une nouvelle instance de la classe, en copiant le sommet
   * passé en paramètre.
   *
   * @param AVertex le sommet à copier
   */
  CVertex(const CVertex& AVertex);

  /**
   * Constructeur prenant en paramètre un doublet.
   * Construction d'une nouvelle instance de la classe, en copiant uniquement
   * les coordonnées du pointel du doublet passé en paramètre.
   *
   * @param ADoublet le doublet à "copier"
   */
  CVertex(const CDoublet& ADoublet);

  /**
   * Destructeur
   */
  ~CVertex();

  // @}
  // @name Accesseurs
  // @{

  /**
   * Accès en lecture à la première composante du sommet.
   */
  TCoordinate getX() const;

  /**
   * Accès en lecture à la deuxième composante du sommet.
   */
  TCoordinate getY() const;

  /**
   * Accès en lecture à une composante du sommet.
   *
   * @param ADim Une dimension (0 ou 1)
   */
  TCoordinate getCoord(int ADim) const;

  /**
   * Positionne la première composante du sommet à la valeur ANewX.
   *
   * @param ANewX Une valeur quelconque
   */
  void setX(TCoordinate ANewX);

  /**
   * Positionne la deuxième composante du sommet à la valeur ANewY.
   *
   * @param ANewY Une valeur quelconque
   */
  void setY(TCoordinate ANewY);

   /**
   * Change la valeur de la composante ADim du sommet.
   *
   * @param ADim Une dimension (0 ou 1)
   * @param ANewCoord Une valeur quelconque
   */
  void setCoord(int ADim, TCoordinate ANewCoord);

  /**
   * Affecte les trois composantes du sommet.
   *
   * @param ANewX Une valeur quelconque
   * @param ANewY Une valeur quelconque
  
   */
  void setXY(TCoordinate ANewX, TCoordinate ANewY);

  // @}
  // @name Divers opérateurs
  // @{

  CVertex& operator=(const CVertex& AVertex);

  bool operator==(const CVertex& AVertex) const;
  bool operator!=(const CVertex& AVertex) const;

  CVertex& operator+=(const CVertex&);
  CVertex& operator-=(const CVertex&);

  CVertex& operator*=(TCoordinate);
  CVertex& operator/=(TCoordinate);

  CVertex  operator*(TCoordinate) const;
  CVertex  operator/(TCoordinate) const;

  CVertex  operator+(const CVertex&) const;
  CVertex  operator-(const CVertex&) const;
  CVertex  operator+(              ) const;
  CVertex  operator-(              ) const;

  // @}
  // @name Fonctions mathématiques
  // @{

  /**
   * @return La norme du produit vectoriel
   */
  TCoordinate normVectoriel(const CVertex& AVector) const;

  /**
   * Produit scalaire
   */
  TCoordinate dot(const CVertex& AVector) const;

  // @}
  // @name Fonctions diverses
  // @{

  /**
   * Distance euclidienne du sommet à l'origine.
   */
  TCoordinate norm() const;

  /**
   * Distance euclidienne au carré du sommet à l'origine.
   */
  TCoordinate sqrNorm() const;

  /**
   * Teste si le sommet est l'origine en utilisant la fonction isZero
   * de la bibliothèque "math".
   */
  bool isNull() const;

  /**
   * Calcule le produit composante par composante de *this et AVertex.
   *
   * @param AVector Un vecteur quelconque
   * @return (X.X' Y.Y'')
   */
  CVertex multiply(const CVertex& AVector) const;

  /**
   * Calcule la division composante par composante de *this et AVector.
   *
   * @param AVector Un vecteur quelconque
   * @return (X/X' Y/Y')
   */
  CVertex divide(const CVertex& AVector) const;

  // @}
};

// @name Produit d'un vecteur par une constante
// @{

/**
 * Calcule le produit d'un vecteur par une constante, lorsque la constante est
 * membre gauche de la multiplication.
 */
CVertex operator*(TCoordinate ACoef, const CVertex& AVertex);

// @}
// @name Affichage
// @{

/**
 * @param AStream Le flot dans lequel afficher le sommet
 * @param AVertex Le sommet à afficher
 * @return Le flot utilisé pour l'affichage
 */
std::ostream& operator<<(std::ostream& AStream, const CVertex& AVertex);

// @}
// @name Constantes
// @{

/**
 * Le sommet de cordonnées (0,0).
 */
static const CVertex ORIGIN(0,0);

/**
 * Le vecteur de composantes (1,0,0).
 */
static const CVertex OX(1,0);

/**
 * Le vecteur de composantes (0,1,0).
 */
static const CVertex OY(0,1);


/** BASE
 * Les deux vecteurs de base OX et OY placés dans un tableau.
 */
static const CVertex BASE[2] = { OX, OY };

// @}
//******************************************************************************
#include INCLUDE_INLINE("vertex.icc")
//******************************************************************************
#endif // VERTEX_HH
//******************************************************************************
