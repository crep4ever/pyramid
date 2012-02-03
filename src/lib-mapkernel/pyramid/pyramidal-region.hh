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
#ifndef PYRAMIDAL_REGION_HH
#define PYRAMIDAL_REGION_HH
//******************************************************************************
#include "region.hh"
#include "point2d.hh"
//******************************************************************************

namespace Map2d
{
  /**
   * \file pyramidal-region.hh
   * \class CPyramidalRegion
   * \brief CPyramidalRegion is a region within a top-down pyramid
   *
   * A pyramidal region contains additionnal information such as up/down relationships.
   * The region "up" is the parent region in the previous level.
   * The region "down" is the first child (also called 'representative') in the next level.
   * The region "down" contains the projection of the region's first pixel (top-left).
   */

  class CCriterionSplit;
  class CCriterionMerge;

  class CPyramidalRegion: public CRegion
  {
  public:
    /// \enum Label
    /// This enum describes the different kinds of regions
    /// that compose an histological image
    enum Label {Background, Tissue, DarkTissue, BrightTissue, Lesion, Cancer, Stroma, Mitosis, InSitu, Invalid};

  private:
    CPyramidalRegion* m_up;   ///< region up
    CPyramidalRegion* m_down; ///< region down
    Label m_label;            ///< region's label
    CPoint2D  m_firstPixel;   ///< first pixel (top-left) of the region

  public:
    /// Default constructor
    CPyramidalRegion();

    /// Copy constructor
    /// @param  region: the region to copy
    CPyramidalRegion(const CRegion& region);

    /// Copy constructor without father/brother/son relationships
    /// @param region: the region to copy
    CPyramidalRegion(const CPyramidalRegion& region);

    /// Destructor
    virtual ~CPyramidalRegion();

    /// Getter on the region's representative dart
    /// This dart is always chose of the external face
    /// and the first pointel of its embedding usually corresponds
    /// to the coordinate of the region's first pixel
    /// @return the representative dart
    virtual CDart* getRepresentativeDart() const;

    /// Setter on the region's representative dart
    /// This dart is always chose of the external face
    /// and the first pointel of its embedding usually corresponds
    /// to the coordinate of the region's first pixel
    /// @param dart: the representative dart
    virtual void setRepresentativeDart(CDart* dart);

    /// Getter on the parent region (region up) in previous level
    /// @return the region up
    CPyramidalRegion* up()   const;

    /// Setter on the region that represents the set of this region's children in next level
    /// The region "down" contains the projection of this region's first pixel (top-left).
    /// @param region: the region down
    void setUp(CPyramidalRegion* region);

    /// Getter on the region that represents the set of this region's children in next level
    /// The region "down" contains the projection of this region's first pixel (top-left).
    /// @return the region down
    CPyramidalRegion* down() const;

    /// Setter on the parent region (region up) in previous level
    /// @param region: the region up
    void setDown(CPyramidalRegion* region);

    /// Getter on the region's label
    /// @return the label
    Label label() const;

    /// Setter on the region's label
    /// @param label: the label
    void setLabel(const Label & label);

    /// Getter on the region's brother
    /// The brother denotes a region of the same level in the inclusion tree
    /// but from a different connected component
    /// @return the brother region
    CPyramidalRegion* getBrother()    const;

    /// Getter on the region's father
    /// The father denotes the parent region within  the inclusion tree
    /// @return the father region
    CPyramidalRegion* getFather()     const;

    /// Getter on the region's first son
    /// The first son denotes the first region in the imbricated connected component
    /// @return the first son region
    CPyramidalRegion* getFirstSon()   const;

    /// Getter on the next region within the same connected component
    /// @return the next region within the same connected component
    CPyramidalRegion* getNextSameCC() const;

    /// Getter on the minimum grey level of the pixels of the region
    /// A grey level is stored as a value between 0 and 255
    /// @return the minimum grey level
    uint getGreyMin()   const;

    /// TODO: protected?
    /// Setter on the minimum grey level of the pixels of the region
    /// A grey level is stored as a value between 0 and 255
    /// @param greyValue: a minimum grey level
    void setGreyMin(TRegionId greyValue);

    /// Getter on the maximum grey level of the pixels of the region
    /// A grey level is stored as a value between 0 and 255
    /// @return the maximum grey level
    uint getGreyMax()   const;

    /// TODO: protected?
    /// Setter on the maximum grey level of the pixels of the region
    /// A grey level is stored as a value between 0 and 255
    /// @param greyValue: a minimum grey level
    void setGreyMax(TRegionId greyValue);

    /// Getter on the sum of the region's pixels grey levels
    /// ie: grey(pixel1) + grey(pixel2) + ...
    /// A grey level is stored as a value between 0 and 255
    /// @return the sum for region's pixels grey levels
    uint getGreySum() const;

    /// TODO: protected?
    /// Setter on the sum of the region's pixels grey levels
    /// A grey level is stored as a value between 0 and 255
    /// @param sum: sum of region's pixels grey levels
    void setGreySum (TRegionId sum);

    /// Getter on the square sum of the region's pixels grey levels
    /// ie: grey(pixel1)² + grey(pixel2)² + ...
    /// A grey level is stored as a value between 0 and 255
    /// @return the sum for region's pixels grey levels
    uint getGreySquareSum() const;

    /// TODO: protected?
    /// Setter on the square sum of the region's pixels grey levels
    /// ie: grey(pixel1)² + grey(pixel2)² + ...
    /// A grey level is stored as a value between 0 and 255
    /// @param sum: the square sum for region's pixels grey levels
    void setGreySquareSum (uint sum);

    /// Getter on the region's number of pixels
    /// @return the number of pixels
    uint getNbPixels() const;

    /// Setter on the region's number of pixels
    /// @param value: a number of pixels
    void setNbPixels(uint value);

    /// Getter on the region's first pixel
    /// The first pixel (fp) denotes that is at the left of the top line
    /// Note that for each pixel p in the region, y(fp) <= y(p)
    /// but that x(fp) <= x(p) may not be always true
    /// @return the first pixel
    CPoint2D firstPixel() const;

    /// Setter on the region's first pixel (top-left)
    /// The first pixel (fp) denotes that is at the left of the top line
    /// Note that for each pixel p in the region, y(fp) <= y(p)
    /// but that x(fp) <= x(p) may not be always true
    /// @param pixel: a first pixel
    void setFirstPixel(const CPoint2D& pixel);

    /// Getter on the region's last pixel (bottom-right)
    /// The last pixel (lp) denotes that is at the right of the bottom line
    /// Note that for each pixel p in the region, y(lp) >= y(p)
    /// but that x(lp) >= x(p) may not be always true
    /// @return the last pixel
    CPoint2D lastPixel() const;

    /// Setter on the region's last pixel (bottom-right)
    /// The last pixel (lp) denotes that is at the right of the bottom line
    /// Note that for each pixel p in the region, y(lp) >= y(p)
    /// but that x(lp) >= x(p) may not be always true
    /// @param pixel: a last pixel
    void setLastPixel(const CPoint2D& pixel);

    /// Getter on the region's identifier
    /// The identifier is a unique positive integer
    /// @return the region's identifier
    uint id() const;

    //******************************************************************************
    //Opérations sur la région
    //******************************************************************************
    virtual CRegion* addSon( TRegionId );
    virtual CRegion* addBrother( TRegionId );
    virtual CRegion* addSameCC( TRegionId );
    virtual void     addPixels(CRegion* region);
    virtual void addPixel( TRegionId ACoul, CDart* dart=NULL );
    void addGrey(TRegionId AGrey);
    TRegionId getAverageGrey() const;

    void mergeWith(CPyramidalRegion* region);

    //******************************************************************************
    //Opérations de contrôle et de vérification
    //******************************************************************************

    /// Teste l'existence d'une région up
    /// @return true si la région a une up
    bool existUp();

    /// Teste l'existence d'une région down
    /// @return true si la région a une down
    bool existDown();

    /// Affiche les caractéristiques d'une région
    void print();

    /// Vérifie des contraintes sur la région
    /// @return true si la région est valide
    bool isOk();

  };

  /// La classe région infinie. La seule différence concerne la méthode
  /// isInfiniteRegion qui va désormais retourner vrai.
  class CInfinitePyramidalRegion : public CPyramidalRegion
  {
  public:
    CInfinitePyramidalRegion();
    virtual ~CInfinitePyramidalRegion();

    bool isInfiniteRegion() const;
  };

  //******************************************************************************
  //******************************************************************************

  class CLessRegionFirstPixel : public std::binary_function<CPyramidalRegion*, CPyramidalRegion*, bool>
  {
    public :
      bool operator()( CPyramidalRegion* region1, CPyramidalRegion* region2 )
        {
          return region1->firstPixel() < region2->firstPixel();
        }
   };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("pyramidal-region.icc")
//******************************************************************************
#endif // PYRAMIDAL_REGION_HH
//******************************************************************************
