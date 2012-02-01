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
#ifndef PYRAMID_HH
#define PYRAMID_HH
//******************************************************************************
#include <vector>
#include <deque>
#include <string>
#include "point2d.hh"
#include "point3d.hh"
#include "inline-macro.hh"
#include "macros.hh"
//******************************************************************************

class CImageTiff;

namespace Map2d
{
  /**
   * \file pyramid.hh
   * \class CPyramid "pyramid.hh"
   * \author Romain Goffe
   * \brief CPyramid is a tiled top-down pyramid
   */

  class CTile;
  class CLevel;

  class CPyramid
  {

  private:
    // L'image
    CImageTiff* FImage;

  protected:
    uint FNbPixels; //split crit
    float FDeviation;
    uint FTileWidth;
    uint FTileHeight;
    uint FNbLevels;
    uint FExtractMode;
    uint FSegmentationMode;
    uint FProjectionMode;
    uint FFocusAttentionMode;
    uint FDetectFictiveBordersMode;

  public:
    uint FTileTotal;
    uint FTileCounter;
    uint FLevelCounter;
    uint FMaxMemory;

  public:
    // Niveaux de la pyramide
    std::deque<CLevel*> FLevels;
    std::deque<CLevel*>::iterator it;

    //Seuil de merge des niveaux
    std::vector<uint> FMergeThresholds;

    /// Constructeur depuis une image
    /// @param AFilename : le fichier image
    CPyramid(const std::string & AFilename);
    CPyramid();

    // Destructeur
    virtual ~CPyramid();

    // Accesseurs
    CImageTiff* image() const;
    void setImage( CImageTiff* AImage );

    std::deque<CLevel*> levels() const;
    void setLevels( std::deque<CLevel*>& ALevels );

    uint nbPixels() const;
    void setNbPixels(uint ANbPixels);

    uint splitOracle() const;
    void setSplitOracle(uint ASplitOracle);

    float deviation() const;
    void  setDeviation(float ADeviation);

    uint tileWidth() const;
    void setTileWidth(uint ATileWidth);

    uint tileHeight() const;
    void setTileHeight(uint ATileHeight);

    uint nbLevels() const;
    void setNbLevels(uint ATileHeight);

    uint extractMode() const;
    void setExtractMode(const ExtractMode & AMode);

    uint detectFictiveBordersMode() const;
    void setDetectFictiveBordersMode(const DetectFictiveBordersMode & AMode);

    CLevel* level(uint ALevel) const;
    void addLevel();

    uint segmentationMode() const;
    void setSegmentationMode( const SegmentationMode & AMode );

    uint projectionMode() const;
    void setProjectionMode( const ProjectionMode & AMode );

    uint focusAttentionMode() const;
    void setFocusAttentionMode( const FocusAttentionMode & AMode );

    std::vector<uint> mergeThresholds() const;
    void setMergeThresholds( std::vector<uint> AMergeThresholds );

    /// @return the number of topological tiles in the pyramid
    uint nbTiles() const;

    //******************************************************************************
    // Méthodes sur l'image
    //******************************************************************************


    /// @return la largeur totale de l'image
    uint imageWidth(uint ALevel) const;

    /// @return la hauteur totale de l'image
    uint imageHeight(uint ALevel) const;

    /// @return le nombre de tuiles topologiques sur la largeur de l'image
    uint nbTilesWidth(uint APage) const;

    /// @return le nombre de tuiles topologiques sur la hauteur de l'image
    uint nbTilesHeight(uint APage) const;

    /// Donne les ratios en x et y d'un niveau par rapport au précédent
    CPoint2D ratios(uint ALevel) const;

    //******************************************************************************
    // Propriétés sur les tuiles
    //******************************************************************************


    /// Abscisse maximum d'une tuile
    /// @param ATile: la tuile
    /// @return l'abscisse
    uint xmax(CTile* ATile) const;

    /// Ordonnée maximum d'une tuile
    /// @param ATile: la tuile
    /// @return l'ordonnée
    uint ymax(CTile* ATile) const;

    /// Abscisse minimum d'une tuile
    /// @param ATile: la tuile
    /// @return l'abscisse
    uint xmin(CTile* ATile) const;

    /// Ordonnée minimum d'une tuile
    /// @param ATile: la tuile
    /// @return l'ordonnée
    uint ymin(CTile* ATile) const;

    /// @return la largeur de la tuile
    uint tileWidth(CTile* ATile) const;

    /// @return la hauteur de la tuile
    uint tileHeight(CTile* ATile) const;

    //******************************************************************************
    // Opérations sur les tuiles
    //******************************************************************************


    /// Ajouter une tuile à la pyramide
    /// @param ATile : la tuile
    void addTile(CTile* ATile, uint ALevel);

    /// Relie deux tuiles up et down dans les 2 sens
    /// @param ATileUp: la tuile up
    /// @param ATileDown: la tuile down
    void linkTileUpDown(CTile* ATileUp, CTile* ATileDown);

    /// Retourne la tuile de coordonnées (i,j) du level k
    /// directement depuis le vecteur FTiles si elle est en mémoire ou
    /// depuis un load si elle a été swappé.
    CTile* tile(const CPoint3D & APos);

    /// Enregistre une tuile sur disque dur
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void writeTile(const CPoint3D & APos);

    /// Charge une tuile en mémoire
    /// et met à jour les liens up/down entre tuiles s'ils existent
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    /// @return la tuile chargée
    void linkTilesUpDown();

    /// Charge les propriétés de la tuile
    /// @param ATile : la tuile
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void loadTileUpDown(CTile* ATile);

    /// Charge toutes les tuiles en mémoire
    void loadAllTiles();

    /// Supprime la tuile (Ai,Aj,Ak) de la mémoire (du veteur FTiles)
    /// @return true si la tuile a bien été supprimée
    bool unloadTile(uint Ai, uint Aj, uint Ak);

    /// Décharge toutes les tuiles en mémoire
    void unloadAllTiles();


    //******************************************************************************
    //Opérations de construction
    //******************************************************************************

    /// Utilise la libImTiff pour l'extraction de l'image
    CImageTiff* importImTiff(const std::string & filename);

    /// Construit la pyramide
    void build();

    /// Construction d'un niveau de la pyramide
    /// @param ALevel: le niveau (0=sommet)
    uint extractLevel(uint id, uint ALevel);


    //******************************************************************************
    // Appels système pour sauvegarde/export
    //******************************************************************************

    /// construit une pyramide d'après les fichiers d'un dossier
    /// @param APath: le chemin d'accès au dossier contenant la pyramide
    void open(const std::string & APath);

    /// Enregistre l'image qui a seri à construire la pyramide
    /// sous ./result/image.tif
    /// @param AImagePaht: le chemin absolu de l'image
    /// @param AImageName: le nom du fichier image (inutilisé)
    void copyImageInWorkingDirectory(const std::string & AImagePath, const std::string & AImageName);

    /// Enregistre la pyramide en copiant le dossier result
    ///@param APath: le répertoire de destination
    void save(const std::string & APath);

    /// Supprime les répertoires de travail "output" et "result"
    void clear();

    /// Initialise le répertoire de travail "output"
    void init();

    /// Déplace le répertoire de travail "output" vers "result"
    void exportData();


    //******************************************************************************
    // Opérations de contrôle et de vérification
    //******************************************************************************

    /// @return true si la tuile (i,j,k) est chargée en mémoire
    bool isTileLoaded(uint Ai, uint Aj, uint Ak);

    /// @return la mémoire occupée par la pyramide (sans le vecteur des tuiles)
    unsigned long int getMemoryForPyramid() const;

    /// Affiche la mémoire occupée par la pyramide + les tuiles chargées
    void printInfosMemory() const;

    /// Empreinte mémoire de la pyramide locale
    /// @return la taille en Bytes de l'ensemble des tuiles actuellement
    /// chargées dans la pyramide
    uint getMemoryForLocalPyramid() const;

    /// @return le nombre total de brins dans la pyramide
    uint nbDarts();

    /// @return le nombre total de regions dans la pyramide
    uint nbRegions();

    /// Affiche la mémoire nécessaire si toutes les tuiles étaient chargées
    void totalMemoryRequired() const;

    /// Vérifie certaines contraintes sur la structure pyramidale
    void print();

    /// Vérifie certaines contraintes sur la structure pyramidale
    // bool isPyramidOk();

  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("pyramid.icc")
//******************************************************************************
#endif // PYRAMID_HH
//******************************************************************************
