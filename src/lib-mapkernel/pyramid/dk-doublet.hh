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
#ifndef DK_DOUBLET_HH
#define DK_DOUBLET_HH
//******************************************************************************
#include <vector>
#include "inline-macro.hh"
#include "doublet.hh"
#include "heap.hh"
//******************************************************************************

  /**
   * \file dk-doublet.hh
   * \brief Class to add a weight on doublet for dijkstra algorithm
   * \author Romain Goffe
   */
  class CDkDoublet : public CDoublet
  {

  protected:
    float FWeight;
    CDkDoublet*  FPrev;

  public:
    /// Constructeur
    CDkDoublet(const CDkDoublet & ADkDoublet);
    CDkDoublet(const CDoublet & ADoublet);
    CDkDoublet();
    // Destructeur
    virtual ~CDkDoublet();

    //Accesseurs
    float weight() const;
    void setWeight(float AWeight);
    void updateWeight(float AValue, Heap<CDkDoublet*> & AHeap);

    CDkDoublet* prev() const;
    void setPrev(CDkDoublet* APrev);

    CDkDoublet& operator = (const CDkDoublet & ADoublet);
    bool operator == (const CDkDoublet & ADoublet) const;
    bool operator != (const CDkDoublet & ADoublet) const;
    bool operator < (const CDkDoublet & ADoublet) const;

    friend std::ostream& operator<<(std::ostream& AStream,
				    const CDkDoublet& ADoublet);
  };
//******************************************************************************
#include INCLUDE_INLINE("dk-doublet.icc")
//******************************************************************************
#endif // DK_DOUBLET_HH
//******************************************************************************
