/*
 * libImage2D : Librairie de manipulation d'une image 2D.
 * Copyright (C) 2008, Guillaume Damiand, guillaume.damiand@liris.cnrs.fr
 *               http://liris.cnrs.fr
 *
 * This file is part of libImage2D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//******************************************************************************
#ifndef PIXEL_HH
#define PIXEL_HH

#include <ostream>
//******************************************************************************
enum
  {
    FIRST_DIR = 0,

    FIRST_DIR_4CONNECTED = FIRST_DIR,

    LEFT   = FIRST_DIR,
    TOP,
    RIGHT,
    BOTTOM,

    LAST_DIR_4CONNECTED,

    LAST_DIR = LAST_DIR_4CONNECTED,

    FIRST_DIR_8CONNECTED = LAST_DIR_4CONNECTED,

    LEFT_TOP = FIRST_DIR_8CONNECTED,
    RIGHT_TOP,
    LEFT_BOTTOM,
    RIGHT_BOTTOM,

    LAST_DIR_8CONNECTED
  };
//******************************************************************************
typedef int TPixelCoord;

class CPixel
{
public:
  CPixel();
  CPixel(TPixelCoord Ax, TPixelCoord Ay);
  CPixel(const CPixel&);
  ~CPixel();

  // Accesseurs
  TPixelCoord getX() const;
  TPixelCoord x() const;
  TPixelCoord getY() const;
  TPixelCoord y() const;

  // Modificateurs
  void setX(TPixelCoord);
  void setY(TPixelCoord);
  void set(TPixelCoord, TPixelCoord);

  // Incrementation
  void incX(int ADx = 1);
  void incY(int ADy = 1);

  // Opérateurs
  CPixel& operator = (const CPixel&);

  bool operator == (const CPixel&) const;
  bool operator != (const CPixel&) const;

  friend std::ostream& operator << (std::ostream & os, const CPixel&);

  // Voisins
  CPixel left() const;
  CPixel right() const;
  CPixel top() const;
  CPixel bottom() const;

  CPixel leftTop() const;
  CPixel rightTop() const;
  CPixel leftBottom() const;
  CPixel rightBottom() const;

  CPixel neighboor(int ADir) const;

private:
  TPixelCoord Fx, Fy;
};
//******************************************************************************
#include "pixel.icc"
//******************************************************************************
#endif // PIXEL_HH
//******************************************************************************
