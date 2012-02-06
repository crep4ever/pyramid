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
#include "traversal-region-pixels.hh"
#include "tile.hh"
using namespace Map2d;
using namespace pyramid;
//*****************************************************************************
CTraversalRegionPixels::CTraversalRegionPixels(CTile* ATile,
					       CPyramidalRegion* ARegion):
  FTile(ATile),
  FRegion(ARegion),
  FStop(false)
{
  if( ARegion->isInfiniteRegion() ) return;

  FPixelsStack.push(static_cast<CPyramidalRegion*>(ARegion)->firstPixel());
  this->operator++();
  FTile->markPixel(FCurrentPixel.getX(),FCurrentPixel.getY());
}
//*****************************************************************************
CTraversalRegionPixels::~CTraversalRegionPixels()
{
  FTile->unmarkAllPixels();
}
//*****************************************************************************
void CTraversalRegionPixels::reinit()
{
  FStop = false;
  FTile->unmarkAllPixels();
  while(!FPixelsStack.empty())
    {
      FPixelsStack.pop();
    }
  FPixelsStack.push(static_cast<CPyramidalRegion*>(FRegion)->firstPixel());
  this->operator++();
  FTile->markPixel(FCurrentPixel.getX(),FCurrentPixel.getY());
}
//*****************************************************************************
void CTraversalRegionPixels::operator++()
{
  if( FPixelsStack.empty() )
    {
      FStop = true;
      return;
    }

  FCurrentPixel = FPixelsStack.top();
  FPixelsStack.pop();

  CDoublet t(FCurrentPixel.getX(),FCurrentPixel.getY());
  t.setLinel(XPOS);
  if( !FTile->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setY(FCurrentPixel.getY()-1);
      if(!FTile->isPixelMarked(n.getX(),n.getY()))
	{
	  FTile->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    }

  t.setLinel(YPOS);
  if( !FTile->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setX(FCurrentPixel.getX()-1);
      if(!FTile->isPixelMarked(n.getX(),n.getY()))
	{
	  FTile->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    }

 t = CDoublet(FCurrentPixel.getX()+1,FCurrentPixel.getY()+1);
 t.setLinel(XNEG);
  if( !FTile->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setY(FCurrentPixel.getY()+1);
      if(!FTile->isPixelMarked(n.getX(),n.getY()))
	{
	  FTile->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    }

  t.setLinel(YNEG);
  if( !FTile->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setX(FCurrentPixel.getX()+1);
      if(!FTile->isPixelMarked(n.getX(),n.getY()))
	{
	  FTile->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    }
}
//*****************************************************************************
const CPoint2D & CTraversalRegionPixels::operator++(int)
{
  this->operator ++();
  return FCurrentPixel;
}
//*****************************************************************************
bool CTraversalRegionPixels::cont() const
{
  return !FStop;
}
//*****************************************************************************
const CPoint2D & CTraversalRegionPixels::operator*()
{
  return FCurrentPixel;
}
//*****************************************************************************
