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

#ifndef PYRAMID_HH
#define PYRAMID_HH

#include <vector>
#include <deque>
#include <string>
#include "point2d.hh"
#include "point3d.hh"
#include "inline-macro.hh"
#include "macros.hh"

class CImageTiff;

/**
 * \namespace pyramid
 * \brief construction of tiled top-down pyramids
 */

namespace pyramid
{
  /**
   * \file pyramid.hh
   * \class CPyramid
   * \brief CPyramid is a tiled top-down pyramid.
   *
   * A tiled top-down pyramid is a combinatorial pyramid of tiled maps
   * that is built according to a top-down construction scheme.
   *
   * The definition of a tiled top-down pyramid is a combination of 3 models:
   * \li a pyramid of images that correspond to the multi-pages tiff image
   * \li a pyramid of tiles that is similar to a quadtree structure
   * \li a pyramid of tiled maps that encode topological information
   * \image html pyramid.svg "A tiled top-down pyramid: a combination of three hierarchical models" width=4cm
   *
   * Tiles, regions and darts have up/down relationships in the previous/next levels.
   * \image html up-down.svg "Up/down relationships within a tiled top-down pyramid" width=8cm
   *
   * The top-down scheme consists in refining regions from a level k to build the level k+1
   * This operation can be decomposed in two main steps:
   * \li a projection of regions' borders: a border that exists in level k will exist in level k+1
   * \li a region split according to a segmentation criterion that is computed on the image
   *
   * \image html construction.svg "Top-down construction scheme" width=4cm
   */

  class CTile;
  class CLevel;

  class CPyramid
  {

  private:
    CImageTiff* m_image; ///< The pyramid of images (a multi-pages tiff)
    uint m_tileWidth;  ///< tile width in the top level of the pyramid
    uint m_tileHeight; ///< tile height in the top level of the pyramid
    uint m_nbLevels;   ///< number of levels in the pyramid
    uint m_extractMode;      ///< extraction mode: builds topological tiles from image data
    uint m_segmentationMode; ///< segmentation mode: merging criteria between regions
    uint m_projectionMode;   ///< projection mode: borders projection from level k onto level k+1
    uint m_focusAttentionMode; ///< focus of attention mode: determines regions of interest
    uint m_detectFictiveBordersMode; ///< detect fictive borders on tiles' borders
    uint m_tileTotal;    ///< total number of tiles in the pyramid
    uint m_tileCounter;  ///< a tile counter
    uint m_levelCounter; ///< a level counter
    uint m_maxMemory;    ///< stores the maximum amount of memory required during the construction
    std::deque<CLevel*> m_levels; ///< the levels (tiled maps) of the pyramid
    std::deque<CLevel*>::iterator it; ///< an iterator on the pyramid's levels
    std::vector<uint> m_mergeThresholds; ///< m_mergeThresholds[k] is a threshold (grey level) to merge regions at level k

  public:
    /// Constructor
    /// @param filename a tiff image
    CPyramid(const std::string & filename);
    CPyramid();

    // Destructor
    virtual ~CPyramid();

    /// Getter on the pyramid of images (multi-pages tiff)
    /// @return the image
    CImageTiff* image() const;

    /// Setter on the pyramid of images (multi-pages tiff)
    /// @param image the image
    void setImage(CImageTiff* image);

    /// Getter on the levels of the pyramid
    /// @return the levels
    std::deque<CLevel*> levels() const;

    /// Setter on the levels of the pyramid
    /// @param levels the levels
    void setLevels(std::deque<CLevel*>& levels);

    /// Getter on the tile width property
    /// @return the tile width in the top level
    uint tileWidth() const;

    /// Setter on the tile width property
    /// @param value the tile width in the top level
    void setTileWidth(uint value);

    /// Getter on the tile height property
    /// @return the tile height in the top level
    uint tileHeight() const;

    /// Setter on the tile height property
    /// @param value the tile height in the top level
    void setTileHeight(uint value);

    /// Getter on the number of levels of the pyramid
    /// @return the number of levels of the pyramid
    uint nbLevels() const;

    /// Setter on the number of levels of the pyramid
    /// @param value the number of levels of the pyramid
    void setNbLevels(uint value);

    /// Getter on the pyramid's extraction mode
    /// @return the extract mode
    uint extractMode() const;

    /// Setter on the pyramid's extract mode
    /// @param mode the pyramid's extract mode
    void setExtractMode(const ExtractMode & mode);

    /// Getter on the pyramid's fictive borders detection mode
    /// @return the fictive borders detection mode
    uint detectFictiveBordersMode() const;

    /// Setter on the pyramid's fictive borders detection mode
    /// @param mode the fictive borders detection mode
    void setDetectFictiveBordersMode(const DetectFictiveBordersMode & mode);

    /// Getter on a pyramid's level
    /// @param index the index (0 is the top level)
    /// @return the level corresponding to this index
    CLevel* level(uint index) const;

    /// Adds a new level in the pyramid in the pyramid of tiles
    void addLevel();

    /// Getter on the pyramid's fictive borders detection mode
    /// @return the fictive borders detection mode
    uint segmentationMode() const;

    /// Setter on the pyramid's segmentation mode
    /// @param mode the segmentation mode
    void setSegmentationMode(const SegmentationMode & mode);

    /// Getter on the pyramid's projection mode
    /// @return the projection mode
    uint projectionMode() const;

    /// Setter on the pyramid's projection mode
    /// @param mode the projection mode
    void setProjectionMode(const ProjectionMode & mode);

    /// Getter on the pyramid's focus of attention mode
    /// @return the focus of attention mode
    uint focusAttentionMode() const;

    /// Setter on the pyramid's focus of attention mode
    /// @param mode the focus of attention mode
    void setFocusAttentionMode(const FocusAttentionMode & mode);

    /// Getter on the levels' merge thresholds
    /// @return the levels' merge thresholds
    std::vector<uint> mergeThresholds() const;

    /// Setter on the levels' merge thresholds
    /// @param thresholds the levels' merge thresholds
    void setMergeThresholds(std::vector<uint> thresholds);

    /// Getter on the total number of tiles within the pyramid
    /// @return the number of topological tiles in the pyramid
    uint nbTiles() const;

    //******************************************************************************
    // Image
    //******************************************************************************

    /// Getter on a tiff page width
    /// @param index a level
    /// @return image width at index level
    uint imageWidth(uint index) const;

    /// Getter on a tiff page height
    /// @param index a level
    /// @return image height at index level
    uint imageHeight(uint index) const;

    /// Getter on the number of tiles in a single row
    /// @param index a level
    /// @return the number of tiles on width at index level
    uint nbTilesWidth(uint index) const;

    /// Getter on the number of tiles in a single column
    /// @param index a level
    /// @return the number of tiles on height at index level
    uint nbTilesHeight(uint APage) const;

    /// Getter on image ratios between a level and the previous one
    /// @param index current level
    /// @return (x,y) ratio between current level and previous level. x and y should be greater than one.
    CPoint2D ratios(uint ALevel) const;

    //******************************************************************************
    // Tiles
    //******************************************************************************

    /// Maximum abscissa of a tile in the image referential
    /// @param tile the tile
    /// @return the tile maximum abscissa
    uint xmax(CTile* tile) const;

    /// Maximum ordinate of a tile in the image referential
    /// @param tile the tile
    /// @return the tile maximum ordinate
    uint ymax(CTile* tile) const;

    /// Minimum abscissa of a tile in the image referential
    /// @param tile the tile
    /// @return the tile minimum abscissa
    uint xmin(CTile* tile) const;

    /// Minimum ordinate of a tile in the image referential
    /// @param tile the tile
    /// @return the tile minimum ordinate
    uint ymin(CTile* tile) const;

    /// Getter on a tile's width
    /// @param tile the tile
    /// @return the tile's width
    uint tileWidth(CTile* tile) const;

    /// Getter on a tile's height
    /// @param tile the tile
    /// @return the tile's height
    uint tileHeight(CTile* tile) const;

    /// Add a tile to the pyramid
    /// @param tile the tile
    /// @param index the level
    void addTile(CTile* tile, uint index);

    /// Link together two tiles up and down
    /// @param  up the tile up
    /// @param  down the tile down
    void linkTileUpDown(CTile* up, CTile* down);

    /// Returns the tile (i,j,k)
    /// If the tile is swapped, it is loaded into memory
    /// @param pos (i,j,k) position
    /// @return the tile
    CTile* tile(const CPoint3D & pos);

    /// Saves a tile on disk
    /// @param pos (i,j,k) position
    void writeTile(const CPoint3D & pos);

    /// Updates up/down relationships between tiles
    /// Called when a new level is added to the pyramid of tiles
    void linkTilesUpDown();

    /// Load a tile's properties
    /// @param tile the tile
    void loadTileUpDown(CTile* tile);

    /// Load all tiles in memory
    /// likely to crash with large images
    void loadAllTiles();

    /// Remove the tile (i,j,k) from memory
    /// @return true if the operation succeeds, flase otherwise
    bool unloadTile(uint i, uint j, uint k);

    /// Remove all tiles from memory
    /// This operation does not save the tiles on disk
    void unloadAllTiles();


    //******************************************************************************
    // Top-down construction scheme
    //******************************************************************************

    /// Construction of the pyramid of images with lib-imtiff
    CImageTiff* importImTiff(const std::string & filename);

    /// Construction of the pyramid of tiled maps
    void build();

    /// Construction a single level
    /// @param index the level (0 is the top level)
    uint extractLevel(uint id, uint index);


    //******************************************************************************
    // Pyramid Save/Export
    //******************************************************************************

    /// Opens an already saved pyramid from path location
    /// @param path the path to the saved pyramid
    void open(const std::string & path);

    /// Saves the pyramid of images
    /// @param imagePath absolute path to the image
    /// @param imageName image filename
    void copyImageInWorkingDirectory(const std::string & imagePath, const std::string & imageName);

    /// Saves the pyramid
    /// @param path target directory
    void save(const std::string & path);

    /// Removes directories associated to the current pyramid
    /// directories are ./result and ./output
    void clear();

    /// Initialisation of the working directory
    /// directory ./output
    void init();

    /// Move the current working directory from output to result
    void exportData();


    //******************************************************************************
    // Checking/Testing operations
    //******************************************************************************

    /// Determines if a tile is loaded into memory
    /// @param (i,j,k) is the position of the tile within the pyramid
    /// @return true if the tile (i,j,k) is loaded into memory, false otherwise
    bool isTileLoaded(uint Ai, uint Aj, uint Ak);

    /// Getter on the amount of memory required by the pyramid
    /// This amount does not take into account the tiles loaded in memory
    /// @return the memory in bytes
    unsigned long int getMemoryForPyramid() const;

    /// Display the memory required by the pyramid
    /// and by the tiles loaded in memory
    void printInfosMemory() const;

    /// Getter on the memory required by tiles loaded in memory
    /// @return the memory in bytes
    uint getMemoryForLocalPyramid() const;

    /// Getter on the total number of darts in the pyramid
    /// @return the total number of darts in the pyramid
    uint nbDarts();

    /// Getter on the total number of regions in the pyramid
    /// @return the total number of regions in the pyramid
    uint nbRegions();

    /// Estimate the total amount of required memory
    /// if all the tiles were to be loaded
    void totalMemoryRequired() const;

    /// Display some information about this pyramid
    void print();

    /// Vérifie certaines contraintes sur la structure pyramidale
    // bool isPyramidOk();
  };
} // namespace pyramid
#include INCLUDE_INLINE("pyramid.icc")
#endif // PYRAMID_HH
