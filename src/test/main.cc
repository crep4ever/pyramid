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

#include <iostream>
#include <tiffio.h>
#include "pyramid.hh"
using namespace pyramid;

int main(int argc, char * argv[])
{
    std::vector<unsigned int> m_merge;
    m_merge.push_back(50);
    m_merge.push_back(25);
    m_merge.push_back(15);
    m_merge.push_back(5);
    m_merge.push_back(5);
    m_merge.push_back(5);
  
  CPyramid* m_pyramid = new CPyramid("./img/lena-color.tif");
  m_pyramid->setNbLevels(5);
  m_pyramid->setMergeThresholds(m_merge);

  m_pyramid->setExtractMode( ConstantTileSize );
  m_pyramid->setProjectionMode( DirectProjection );
  m_pyramid->setFocusAttentionMode( FocusHisto );
  m_pyramid->setSegmentationMode( Histology );
  m_pyramid->setDetectFictiveBordersMode( DetectionOff );

  m_pyramid->setTileWidth(128);
  m_pyramid->setTileHeight(128);

  m_pyramid->build();

  delete m_pyramid;


  return 0;
}
