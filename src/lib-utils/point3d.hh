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

//*****************************************************************************
#ifndef POINT3D_HH
#define POINT3D_HH
//*****************************************************************************
#include "inline-macro.hh"
//#include "doublet.hh"
#include "macros.hh"
#include <iostream>
//*****************************************************************************

/**
 * La classe CPoint3D représente une coordonnée 3D entière.
 * @author Romain Goffe
 */
class CPoint3D
{
private:
  uint FCoord[3];
  
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
  CPoint3D();

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (Ax,Ay).
   *
   * @param Ax La première  coordonnée du point
   * @param Ay La deuxième  coordonnée du point
   * @param Ay La deuxième  coordonnée du point
   */
  CPoint3D(uint Ax, uint Ay, uint Az);

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (ATab[0],ATab[1]).
   *
   * @param ATab Un tableau des 2 coordonnées du sommet
   */
  CPoint3D(uint ATab[3]);

  ///Constructeur par copie
  CPoint3D(const CPoint3D& AVertex);

  /**
   * Destructeur
   */
  virtual ~CPoint3D();

  // @}
  // @name Accesseurs
  // @{


  //getters
  uint x() const;
  uint y() const;
  uint z() const;

  //setters
  void setX(uint value);
  void setY(uint value);
  void setZ(uint value);

  //raisers
  void incX(uint value);
  void incY(uint value);
  void incZ(uint value);

  void setXYZ(uint ANewX, uint ANewY, uint ANewZ);

  //operators
  CPoint3D& operator=(const CPoint3D& AVertex);
  bool operator==(const CPoint3D& AVertex) const;
  bool operator!=(const CPoint3D& AVertex) const;
  friend bool operator < (CPoint3D u,CPoint3D v);
  friend std::ostream& operator<<(std::ostream& AStream,
				  const CPoint3D& AVector);

};

// @}
// @name Constantes
// @{
  
/**
 * Le sommet de cordonnées (0,0).
 */
static const CPoint3D POINT3D_ORIGIN(0,0,0);
  
/**
 * Le vecteur de composantes (1,0).
 */
static const CPoint3D POINT3D_OX(1,0,0);
  
/**
 * Le vecteur de composantes (0,1).
 */
static const CPoint3D POINT3D_OY(0,1,0);
  
/**
 * Le vecteur de composantes (0,1).
 */
static const CPoint3D POINT3D_OZ(0,0,1);
  
  
/** BASE
 * Les trois vecteurs de base OX et OY placés dans un tableau.
 */
static const CPoint3D POINT3D_BASE[3] = { POINT3D_OX, 
					  POINT3D_OY,
					  POINT3D_OZ};

// @}

//******************************************************************************
#include INCLUDE_INLINE("point3d.icc")
//******************************************************************************
#endif // POINT3D_HH
//******************************************************************************
