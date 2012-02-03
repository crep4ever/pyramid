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
#ifndef ARRAY_HH
#define ARRAY_HH
//******************************************************************************
#include "pyramidal-dart.hh"
#include "doublet.hh"
#include "volume.hh"
//******************************************************************************
namespace Map2d
{
  /**
   * \file array.hh
   * \class CArray "array.hh"
   * \author Romain Goffe
   * \brief CArray allows to retrieve a dart from his doublet.
   * This data structure performs better than a std::map for this use-case
   */

  class CArray
  {
  protected:
    CVolume<CDart*>* m_array;
    unsigned int m_xmin;
    unsigned int m_ymin;

  public:
    /// Constructor
    /// @param xmin: the minium abscissa of the region
    /// @param ymin: the minimum ordinate of the region
    /// @param width: region's width
    /// @param height : region's height
    CArray(unsigned int xmin, unsigned int ymin, unsigned int width, unsigned int height);

    // Destructor
    virtual ~CArray();

    /// Retrieve a dart from its embedding
    /// @param doublet: the doublet that belongs to the dart's embedding
    /// @return the corresponding dart in the map
    CDart* getDart(const CDoublet& doublet) const;

    /// Add a dart to the table so that it can be retrieved from its embedding later
    /// @param dart: the dart
    void setDart(CDart* dart);

  };
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("array.icc")
//******************************************************************************
#endif // ARRAY_HH
//******************************************************************************
