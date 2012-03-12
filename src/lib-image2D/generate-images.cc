/*
 * loadImage2D : Outils pour manipuler une image 2D.
 * Copyright (C) 2008, Guillaume Damiand, guillaume.damiand@liris.cnrs.fr
 *               http://liris.cnrs.fr
 *
 * This file is part of loadImage2D
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
#include "image2D.hh"

//******************************************************************************
void generateSquares(CImage2D & AImage, unsigned int AWidth)
{
   if (!AImage.isOk()) return;
   
   unsigned x,y;
   for (y=0;y<AImage.getYSize();++y)   
      for (x=0;x<AImage.getXSize();++x)
      {
      }
}
//******************************************************************************
void generateLines(CImage2D & AImage, unsigned int AWidth)
{
   if (!AImage.isOk()) return;
   
   TLabel val = 0;
   unsigned x,y;
   for (y=0;y<AImage.getYSize();++y)   
   {   
      for (x=0;x<AImage.getXSize();++x)
      {
         AImage.setPixel(x,y,y/val);
      }
   }   
}
//******************************************************************************

