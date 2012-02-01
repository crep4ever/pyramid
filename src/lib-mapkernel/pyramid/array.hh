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
    CVolume<CDart*>* FArray;
    unsigned int FXmin;
    unsigned int FYmin;

  public:
    /// Constructeur
    /// @param AXmin: l'abscisse minimum de la région
    /// @param AYmin: l'ordonnée minimum de la région
    /// @param AWidth: la largeur de la région
    /// @param AHeight : la hauteur de la région
    CArray(unsigned int AXmin, unsigned int AYmin, unsigned int AWidth, unsigned int AHeight);

    // Destructeur
    virtual ~CArray();

    /// Récupérer un brin à partir d'un doublet
    /// @param ADoublet: un doublet
    /// @return le brin correspondant
    CDart* getDart(const CDoublet& ADoublet) const;

    /// Ajouter un brin à la structure de façon à pouvoir le
    /// récupérer par la suite en donnant son doublet
    /// @param ADart: le brin
    void setDart(CDart* ADart);

  };
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("array.icc")
//******************************************************************************
#endif // ARRAY_HH
//******************************************************************************
