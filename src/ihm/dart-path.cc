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

//******************************************************************************
#include "dart-path.hh"
#include "topological-map.hh"
#include <cassert>
#include <QPainter>
#include <QSettings>
using namespace Map2d;
//******************************************************************************
CDartPath::CDartPath( )
{}
//******************************************************************************
CDartPath::~CDartPath()
{}
//******************************************************************************
CDart* CDartPath::dart()
{
  return m_dart;
}
//******************************************************************************
void CDartPath::setDart( CDart* ADart )
{
  m_dart = ADart;
}
//******************************************************************************
void CDartPath::addArrow(const CDoublet& ADirection )
{
  QPointF end = currentPosition();
  QPointF p1 = end;
  QPointF p2;
  double length = 0.2;
  double halfWidth = 0.1;

  switch(  ADirection.getLinel() )
    {
    case XPOS:
      p1.rx() -=  length;
      p2 = p1;
      p1.ry() +=  halfWidth;
      p2.ry() -=  halfWidth;
      break;
    case XNEG:
      p1.rx() +=  length;
      p2 = p1;
      p1.ry() +=  halfWidth;
      p2.ry() -=  halfWidth;
      break;
    case YPOS:
      p1.ry() -=  length;
      p2 = p1;
      p1.rx() +=  halfWidth;
      p2.rx() -=  halfWidth;
      break;
    case YNEG:
      p1.ry() +=  length;
      p2 = p1;
      p1.rx() +=  halfWidth;
      p2.rx() -=  halfWidth;
      break;
    default:
      break;
    }
  QPolygonF arrow;
  arrow << p1 << end << p2;
  addPolygon(arrow);
}
//******************************************************************************
//******************************************************************************
