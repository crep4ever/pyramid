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
#ifndef __DART_PATH_HH__
#define __DART_PATH_HH__
//******************************************************************************
#include <QPainter>
//******************************************************************************

namespace Map2d
{
  class CDart;
};

class CDoublet;

/** 
 * \file dart-path.hh
 * \class CDartPath "dart-path.hh" 
 * \author Romain Goffe
 * \brief CDartPath draw a dart as a QGraphicsItem
 */

class CDartPath : public QPainterPath
{

public:
  /// Constructor 
  CDartPath();

  /// Destructor
  virtual ~CDartPath();
  
  Map2d::CDart* dart();

  void setDart(Map2d::CDart* dart);

  /// add an arrow at the end of the path according to the direction
  void addArrow(const CDoublet& ADirection );

protected:
  Map2d::CDart * m_dart;

};

#endif  // __DART_PATH_HH__
