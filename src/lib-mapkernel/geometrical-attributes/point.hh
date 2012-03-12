/***************************************************************************
 *   Copyright (C) 2007 by David Coeurjolly, CNRS   *
 *   david.coeurjolly@liris.cnrs.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//******************************************************************************
#ifndef POINT_H
#define POINT_H

/**
	@author David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
*/

#include <iostream>
//******************************************************************************
template<typename T>
class Point {
  public:
    // Constructeurs / destructeur
    Point();
    Point(T ,T);
    Point(const Point &p);

    ~Point();

    // Modificateurs
    Point &operator= (const Point &);
    void set         (const T,const T);
    Point &operator+=(const Point &);
    Point &operator-=(const Point &);
    Point &operator/=(const T);

    // Opérations
    Point operator+ (const Point &) const;
    Point operator- (const Point &) const;
    Point operator* (const Point &) const;
    Point operator/ (const T);

    T dot(const Point &) const;

    // Tests
    bool operator==  (const Point &) const;
    bool operator!=  (const Point &) const;

    bool is4Connected(const Point &) const;

    //Return the direction between the two points [this->APoint]
    // PRECOND : this and M are 4connected
    // 1=droite; 2=haut; -1=gauche; -2=bas
    int getDirection(const Point &) const;

    // Return the direction between the two points [APoint->this]
    // PRECOND : this and M are 4connected
    // 1=droite; 2=haut; -1=gauche; -2=bas
    int getReverseDirection(const Point &) const;

    // Shift the current point in direction ADir.
    void shiftInDirection(int ADir);

    // Shift the current point in direction -ADir.
    void shiftInReverseDirection(int ADir);

    // Return the neighbor point of this in direction ADir.
    Point getPointInDirection(int ADir) const;

    //Affichage
    template<typename T1>
      friend std::ostream& operator<<(std::ostream&, const Point<T1> &);

//  private:
    // Datas
    T x,y;
};

//-----------------------------------------------------------------------------------------
//Implementation
//-----------------------------------------------------------------------------------------

template <typename T>
    Point<T>::Point()
{}

template <typename T>
    Point<T>::Point(T px,T py)
{
  x = px;
  y = py;
}

template <typename T>
    Point<T>::Point(const Point<T> &p)
{
  x = p.x;
  y = p.y;
}

template <typename T>
    Point<T>::~Point()
{}

template <typename T>
    Point<T> &Point<T>::operator=(const Point<T> &source)
{
  x = source.x;
  y = source.y;
  return *this;
}

template<typename T>
void Point<T>::set(const T a,const T b)
{
  x = a;
  y = b;
}

template <typename T>
    Point<T> &Point<T>::operator+=(const Point<T> &M)
{
  x += M.x;
  y += M.y;
  return *this;
}

template <typename T>
    Point<T> &Point<T>::operator-=(const Point &M)
{
  x -= M.x;
  y -= M.y;

  return *this;
}

template <typename T>
    Point<T> &Point<T>::operator/=(const T a)
{
  x /= a;
  y /= a;

  return *this;
}

template <typename T>
    Point<T> Point<T>::operator+(const Point &M) const
{
  Point<T> p;
  p.x = this->x + M.x;
  p.y = this->y + M.y;
  return p;
}

template <typename T>
    Point<T> Point<T>::operator-(const Point &M) const
{
  Point p;
  p.x = this->x - M.x;
  p.y = this->y - M.y;
  return p;
}

template <typename T>
    Point<T> Point<T>::operator*(const Point &M) const
{
  Point<T> p;
  p.x = this->x * M.x;
  p.y = this->y * M.y;
  return p;
}

template <typename T>
    Point<T> Point<T>::operator/(const T a)
{
  Point<T> p;
  p.x = x/a;
  p.y = y/a;

  return p;
}

template<typename T>
    T Point<T>::dot(const Point <T> &a) const
{
  return x * a.x + y * a.y;
}

template <typename T>
    bool Point<T>::operator==(const Point &M) const
{
  return ((x == M.x) and (y == M.y));
}


template <typename T>
    bool Point<T>::operator!=(const Point &M) const
{
  return ((x != M.x) or (y != M.y));
}

template <typename T>
    bool Point<T>::is4Connected(const Point &M) const
{
  return (abs(x-M.x)<=1 and abs(y-M.y)<=1 and abs(x-M.x)+abs(y-M.y)==1);
}

template <typename T>
    int Point<T>::getDirection(const Point &APoint) const
{
  assert( is4Connected(APoint) );

  int dx = APoint.x - x;
  int dy = APoint.y - y;

  if (dy == 0)
    {
      if (dx == 1)
	return 1; // droite
      else //dx==-1
	return -1; // gauche
    }
  else //dy!=0 -> 1 or -1, dx==0
    {
      if (dy == 1)
	return 2; // haut
      else
	return -2; // bas
    }
  assert(false);
  return 0;
}

template <typename T>
    int Point<T>::getReverseDirection(const Point &APoint) const
{ return APoint.getDirection(*this); }


template <typename T>
    void Point<T>::shiftInDirection(int ADir)
{
  switch ( ADir )
    {
    case -1: --x; break;
    case  1: ++x; break;
    case -2: --y; break;
    case  2: ++y; break;
    case  0: break;
    default: assert(false);
    }
}

template <typename T>
    void Point<T>::shiftInReverseDirection(int ADir)
{
  switch ( ADir )
    {
    case  1: --x; break;
    case -1: ++x; break;
    case  2: --y; break;
    case -2: ++y; break;
    case  0: break;
    default: assert(false);
    }
}

template <typename T>
    Point<T> Point<T>::getPointInDirection(int ADir) const
{
  Point<T> res(*this);
  res.shiftInDirection(ADir);
  return res;
}


template <typename T>
    std::ostream& operator<<(std::ostream& flux, const Point<T> &P)
{
  return flux <<"("<<P.x<<","<<P.y<<")";
}

//******************************************************************************
#endif
//******************************************************************************
