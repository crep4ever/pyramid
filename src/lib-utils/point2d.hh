/*
 * Copyright (C) 2008-2011, Alexandre Dupas <alexandre.dupas@gmail.com>
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

//*****************************************************************************
#ifndef POINT2D_HH
#define POINT2D_HH
//*****************************************************************************
#include <iostream>
#include "inline-macro.hh"
//#include "doublet.hh"
#include "macros.hh"
//*****************************************************************************

/**
 * La classe CPoint2D représente une coordonnée 2D entière.
 * @author Alexandre Dupas
 */

class CPoint2D
{
private:
  uint FCoord[2];

public:
  // @name Constructeurs et destructeur
  // @{

  /**
   * Constructeur par défaut
   * Construction d'une nouvelle instance de la classe, initialisée
   * au sommet origine (0,0).
   *
   * @return Une nouvelle instance de la classe
   */
  CPoint2D();

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (Ax,Ay).
   *
   * @param Ax La première  coordonnée du sommet
   * @param Ay La deuxième  coordonnée du sommet
   */
  CPoint2D(uint Ax, uint Ay);

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (ATab[0],ATab[1]).
   *
   * @param ATab Un tableau des 2 coordonnées du sommet
   */
  CPoint2D(uint ATab[2]);

  /**
   * Constructeur par copie
   * Construction d'une nouvelle instance de la classe, en copiant le sommet
   * passé en paramètre.
   *
   * @param AVertex le sommet à copier
   */
  CPoint2D(const CPoint2D& AVertex);

  /**
   * Constructeur prenant en paramètre un doublet.
   * Construction d'une nouvelle instance de la classe, en copiant uniquement
   * les coordonnées du pointel du doublet passé en paramètre.
   *
   * @param ADoublet le doublet à "copier"
   */
  //  CPoint2D(const CDoublet& ADoublet);

  /**
   * Destructeur
   */
  virtual ~CPoint2D();

  // @}
  // @name Accesseurs
  // @{

  /**
   * Accès en lecture à la première composante du sommet.
   */
  uint getX() const;
  uint x() const;

  /**
   * Accès en lecture à la deuxième composante du sommet.
   */
  uint getY() const;
  uint y() const;
  
  /**
   * Accès en lecture à une composante du sommet.
   *
   * @param ADim Une dimension (0 ou 1)
   */
  uint getCoord(uint ADim) const;
  uint coord(uint ADim) const;

  /**
   * Positionne la première composante du sommet à la valeur ANewX.
   *
   * @param ANewX Une valeur quelconque
   */
  void setX(uint ANewX);

  /**
   * Positionne la deuxième composante du sommet à la valeur ANewY.
   *
   * @param ANewY Une valeur quelconque
   */
  void setY(uint ANewY);

  /** Incrémente la première composante du sommet.
   *
   * @param AIncX valeur d'incrément.
   */
  void incX(uint AIncX);

  /** Incrémente la deuxième composante du sommet.
   *
   * @param AIncY valeur d'incrément.
   */
  void incY(uint AIncY);

 
  /**
   * Change la valeur de la composante ADim du sommet.
   *
   * @param ADim Une dimension (0 ou 1)
   * @param ANewCoord Une valeur quelconque
   */
  void setCoord(uint ADim, uint ANewCoord);

  /**
   * Affecte les deux composantes du sommet.
   *
   * @param ANewX Une valeur quelconque
   * @param ANewY Une valeur quelconque
   */
  void setXY(uint ANewX, uint ANewY);

  // @}
  // @name Divers opérateurs
  // @{

  CPoint2D& operator=(const CPoint2D& AVertex);

  bool operator==(const CPoint2D& AVertex) const;
  bool operator!=(const CPoint2D& AVertex) const;
  CPoint2D operator+(const CPoint2D& AVertex) const;
  CPoint2D operator-(const CPoint2D& AVertex) const;

  friend bool operator < (CPoint2D u,CPoint2D v);

  /**
   * @param  AStream  Le flot dans lequel afficher les coordonnées
   * @param  AVector  Le vecteur dont on affiche les coordoonnées
   * @return Le flot utilisé pour l'affichage
   */
  friend std::ostream& operator<<(std::ostream& AStream,
				  const CPoint2D& AVector);



};

// @}
// @name Constantes
// @{
  
/**
 * Le sommet de cordonnées (0,0).
 */
static const CPoint2D COORDINATE_ORIGIN(0,0);
  
/**
 * Le vecteur de composantes (1,0).
 */
static const CPoint2D COORDINATE_OX(1,0);
  
/**
 * Le vecteur de composantes (0,1).
 */
static const CPoint2D COORDINATE_OY(0,1);
  
  
/** BASE
 * Les trois vecteurs de base OX et OY placés dans un tableau.
 */
static const CPoint2D COORDINATE_BASE[2] = { COORDINATE_OX, 
					     COORDINATE_OY };
  
// @}

//******************************************************************************
#include INCLUDE_INLINE("point2d.icc")
//******************************************************************************
#endif // POINT2D_HH
//******************************************************************************
