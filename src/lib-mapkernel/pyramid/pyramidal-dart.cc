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
#include "inline-macro.hh"
#include "pyramidal-dart.hh"
#include INCLUDE_NON_INLINE("pyramidal-dart.icc")
using namespace Map2d;
//******************************************************************************
void CPyramidalDart::print( std::ostream& AOs ) const
{
  CTopologicalDart::print(AOs);
  CDoublet upDoublet(0,0,XPOS);
  if(up()) upDoublet = up()->doublet();
  CDoublet downDoublet(0,0,XPOS);
  if(up()) downDoublet = down()->doublet();
  AOs<<", up: "<<upDoublet<<", down: "<<downDoublet;
}
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& AOs, const CPyramidalDart &ADart)
{
  ADart.print(AOs);
  return AOs;
}
//******************************************************************************
