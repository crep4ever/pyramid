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

#include "traversal-region-pixels.hh"
#include "tile.hh"

using namespace Map2d;
using namespace pyramid;

CTraversalRegionPixels::CTraversalRegionPixels(CTile* ATile,
					       CPyramidalRegion* ARegion):
  m_tile(ATile),
  m_region(ARegion),
  m_stop(false)
{
  if( ARegion->isInfiniteRegion() ) return;

  m_pixelsStack.push(static_cast<CPyramidalRegion*>(ARegion)->firstPixel());
  this->operator++();
  m_tile->markPixel(m_currentPixel.x(),m_currentPixel.y());
}

CTraversalRegionPixels::~CTraversalRegionPixels()
{
  m_tile->unmarkAllPixels();
}

void CTraversalRegionPixels::reinit()
{
  m_stop = false;
  m_tile->unmarkAllPixels();
  while(!m_pixelsStack.empty())
    {
      m_pixelsStack.pop();
    }
  m_pixelsStack.push(static_cast<CPyramidalRegion*>(m_region)->firstPixel());
  this->operator++();
  m_tile->markPixel(m_currentPixel.x(),m_currentPixel.y());
}

void CTraversalRegionPixels::operator++()
{
  if( m_pixelsStack.empty() )
    {
      m_stop = true;
      return;
    }

  m_currentPixel = m_pixelsStack.top();
  m_pixelsStack.pop();

  CDoublet t(m_currentPixel.x(),m_currentPixel.y());
  t.setLinel(XPOS);
  if( !m_tile->isLCell(t) )
    {
      CPoint2D n(m_currentPixel);
      n.setY(m_currentPixel.y()-1);
      if(!m_tile->isPixelMarked(n.x(),n.y()))
	{
	  m_tile->markPixel(n.x(),n.y());
	  m_pixelsStack.push(n);
	}
    }

  t.setLinel(YPOS);
  if( !m_tile->isLCell(t) )
    {
      CPoint2D n(m_currentPixel);
      n.setX(m_currentPixel.x()-1);
      if(!m_tile->isPixelMarked(n.x(),n.y()))
	{
	  m_tile->markPixel(n.x(),n.y());
	  m_pixelsStack.push(n);
	}
    }

 t = CDoublet(m_currentPixel.x()+1,m_currentPixel.y()+1);
 t.setLinel(XNEG);
  if( !m_tile->isLCell(t) )
    {
      CPoint2D n(m_currentPixel);
      n.setY(m_currentPixel.y()+1);
      if(!m_tile->isPixelMarked(n.x(),n.y()))
	{
	  m_tile->markPixel(n.x(),n.y());
	  m_pixelsStack.push(n);
	}
    }

  t.setLinel(YNEG);
  if( !m_tile->isLCell(t) )
    {
      CPoint2D n(m_currentPixel);
      n.setX(m_currentPixel.x()+1);
      if(!m_tile->isPixelMarked(n.x(),n.y()))
	{
	  m_tile->markPixel(n.x(),n.y());
	  m_pixelsStack.push(n);
	}
    }
}

const CPoint2D & CTraversalRegionPixels::operator++(int)
{
  this->operator ++();
  return m_currentPixel;
}

bool CTraversalRegionPixels::cont() const
{
  return !m_stop;
}

const CPoint2D & CTraversalRegionPixels::operator*()
{
  return m_currentPixel;
}

