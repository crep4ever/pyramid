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
#ifndef MAXROW_HH
#define MAXROW_HH
//******************************************************************************
#include <vector>
#include "inline-macro.hh"
#include "doublet.hh"
#include "macros.hh"
//******************************************************************************
namespace pyramid
{
  /**
   * \file dp-maxrow.hh
   * \class CMaxRow "dp-maxrow.hh"
   * \author Romain Goffe
   * \brief Class to store the maxrow function for dynamic programming.
   * This class provides a matrix structure where:
   * - column 1 is a set of doublets that correspond to the possible positions
   * for a pointel xi+1 when projected from a level to another
   * - column 2 is a set of value that correspond to the max
   * - column 3 are the doublets among the possible positions
   * for a pointel xi corresponding to the maximum value of column2 depending on the matrix hi
   *
   */

  class CMaxRow
  {
  protected:
    CDoublet FDoublet1;
    uint FValue;
    CDoublet FDoublet2;

  public:
    /// Constructeur
    CMaxRow(const CDoublet & ADoublet1,
	    uint AValue,
	    const CDoublet & ADoublet2);

    CMaxRow();

    // Destructeur
    virtual ~CMaxRow();

    //Accesseurs
    CDoublet doublet1() const;
    void setDoublet1(const CDoublet & ADoublet1);

    uint value() const;
    void setValue(uint AValue);

    CDoublet doublet2() const;
    void setDoublet2(const CDoublet & ADoublet2);

  };
} // namespace pyramid
//******************************************************************************
#include INCLUDE_INLINE("dp-maxrow.icc")
//******************************************************************************
#endif // MAXROW_HH
//******************************************************************************
