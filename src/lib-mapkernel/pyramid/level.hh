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
#ifndef LEVEL_HH
#define LEVEL_HH
//******************************************************************************
#include <deque>
#include "point2d.hh"
#include "imageTiff.hh"
#include "inline-macro.hh"
#include "CImg.h"
#include "chrono.hh"
#include "macros.hh"
//******************************************************************************

namespace Map2d
{
  /**
   * \file level.hh
   * \class CLevel "level.hh"
   * \author Romain Goffe
   * \brief CLevel is a tiled map that corresponds to a level in
   * a tiled top-down pyramid
   */

  class CTile;
  class CDart;
  class CLevel;

  class CLevel
  {
    protected:
    // Profondeur dans la pyramide
    uint FDepth;
    // Seuil de merge
    uint FMergeThreshold;
    // Dimensions d'une tuile
    uint FTileWidth;
    uint FTileHeight;
    //Compteur de tuiles
    uint FTileCounter;
    // L'image
    CImageTiff* FImage;
    // Hiérarchie
    CLevel* FLevelUp;
    CLevel* FLevelDown;
    // Tuiles du niveau
    std::deque<CTile*> FTiles;

    // Compteur mémoire
    uint FMemory;

  public:
    uint8* FClassif;
    CVolume<uint>* FHisto;
    CImg<char>* FAssignment;

  private:
    // Iterateur pour le parcours des tuiles
    std::deque<CTile*>::iterator it;

  public:
    // Constructeurs
    CLevel(); //par défaut
    // Destructeur
    virtual ~CLevel();

    // Accesseurs
    std::deque<CTile*> tiles() const;
    void setTiles( std::deque<CTile*>& ATiles );

    // Image
    CImageTiff* image() const;
    void setImage( CImageTiff* AImage );

    //profondeur dans la pyramide
    uint depth() const;
    void setDepth(uint ADepth);

    //seuil pour les fusions
    uint mergeThreshold() const;
    void setMergeThreshold(uint AMergeThreshold);

    //largeur d'une tuile
    uint tileWidth() const;
    void setTileWidth(uint AWidth);

    //hauteur d'une tuile
    uint tileHeight() const;
    void setTileHeight(uint AHeight);

    CLevel* levelUp() const;
    void setLevelUp(CLevel* ALevel);

    CLevel* levelDown() const;
    void setLevelDown(CLevel* ALevel);

    /// Relie 2 levels (up/down)
    //void linkLevelUpDown(CLevel* ALevelUp, CLevel* ALevelDown);

    CDart* beta0(CDart* ADart) const;
    CDart* beta1(CDart* ADart) const;
    CDart* beta2(CDart* ADart) const;

    //todo
    CDart* delta1(CDart* ADart) const;
    CDart* delta2(CDart* ADart) const;


    //******************************************************************************
    // Extraction
    //******************************************************************************

    void createTopLevel();
    void extract(const ExtractMode & AExtractMode,
		 const SegmentationMode & ASegmentationMode,
		 const ProjectionMode & AProjectionMode,
		 const FocusAttentionMode & AFocusAttentionMode,
		 const DetectFictiveBordersMode & ADetectFictiveBordersMode);

    ///@return : memory
    uint extractTile( const CPoint2D & APos,
		      const SegmentationMode & ASegmentationMode,
		      const ProjectionMode & AProjectionMode,
		      const FocusAttentionMode & AFocusAttentionMode,
		      const DetectFictiveBordersMode & ADetectFictiveBordersMode,
		      CChrono & AChrono);

    void preprocessing(const SegmentationMode & ASegmentationMode);

    //redéfinir le critère de split suivant la profondeur dans la pyramide

    //******************************************************************************
    // Méthodes sur l'image
    //******************************************************************************

    /// Renseigne les données image d'une tuile en lisant l'image globale
    /// de la pyramide
    /// @param ATile: la tuile
    /// @return l'image associée à la tuile
    void loadTileImage(CTile* ATile);

    /// @return la largeur de l'image associée au niveau
    uint imageWidth() const;

    /// @return la largeur de l'image associée au niveau
    uint imageHeight() const;

    /// @return la largeur d'un component (tuile image)
    uint componentWidth() const;

    /// @return la hauteur d'un component (tuile image)
    uint componentHeight() const;

    /// @return le nombre de components sur la largeur de l'image
    uint nbComponentsWidth() const;

    /// @return le nombre de components sur la hauteur de l'image
    uint nbComponentsHeight() const;

    /// @return le nombre de tuiles topologiques sur la largeur de l'image
    uint nbTilesWidth() const;

    /// @return le nombre de tuiles topologiques sur la hauteur de l'image
    uint nbTilesHeight() const;

    /// @return ratio en x,y entre ce niveau et le niveau précédent
    CPoint2D upRatios() const;
    /// @return ratio en x,y entre le niveau suivant et ce niveau
    CPoint2D downRatios() const;

    //******************************************************************************
    // Système de coordonnées pour les tuiles
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

    /// @return le point haut-gauche
    CPoint2D tileTopLeft(CTile* ATile) const;

    /// @return le point bas-droite
    CPoint2D tileBottomRight(CTile* ATile) const;

    /// @return le point haut-droite
    CPoint2D tileTopRight(CTile* ATile) const;

    /// @return le point bas-gauche
    CPoint2D tileBottomLeft(CTile* ATile) const;

    /// @return l'indice de l'abscisse
    uint tileIndexI(CTile* ATile) const;

    /// @return l'indice de l'ordonnée
    uint tileIndexJ(CTile* ATile) const;

    /// Retourne la tuile adjacente de gauche
    CTile* tileLeft(CTile* ATile);

    /// Retourne la tuile adjacente d'au-dessus
    CTile* tileTop(CTile* ATile);

    /// Existence d'une voisine à gauche
    /// @param ATile la tuile
    /// @return true si la voisine existe
    bool existTileLeft (CTile* ATile) const;

    /// Existence d'une voisine au dessus
    /// @param ATile la tuile
    /// @return true si la voisine existe
    bool existTileTop (CTile* ATile) const;


    //******************************************************************************
    // Extraction
    //******************************************************************************



    //******************************************************************************
    // Opérations sur les tuiles
    //******************************************************************************


    /// Split d'une tuile par burst and merge
    /// Les régions dont splitting_criterion=true sont burstées (régions unitaires)
    /// Puis passe de fusion d'après merging_criterion
    /// @param ATile : la tuile
    ///@return : memory
    uint burstAndMergeTile( const CPoint2D & APos,
			    const SegmentationMode & ASegmentationMode,
			    const ProjectionMode & AProjectionMode,
			    const FocusAttentionMode & AFocusAttentionMode,
			    const DetectFictiveBordersMode & ADetectFictiveBordersMode );

    /// Eclate tout le contour d'une tuile en brins unitaires
    /// @param ATile : la tuile
    void splitTileBorder(CTile* ATile);

    /// Eclate tout le contour d'une tuile en brins unitaires
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void splitTileBorder(const CPoint2D & APos);

    /// Renseigne le vector FCorners d'une tuile en définissant
    /// les brins left/up/right/down correspondant aux 4 coins du bord
    /// externe de la tuile avec les plongements suivantes
    /// left: (0,0,YPOS), up: (1,0,XNEG), right: (width,1,YNEG), down: (0,height,XPOS)
    /// @param ATile: la tuile
    void retrieveTileCorners(CTile* ATile) const;

    /// Renseigne le vector FCorners d'une tuile en définissant
    /// les brins left/up/right/down correspondant aux 4 coins du bord
    /// externe de la tuile
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void retrieveTileCorners(const CPoint2D & APos);

    /// Renseigne la map des beta2 pour les bords haut et gauche d'une tuile
    /// Définit l'opérateur beta2' pour la connexion avec les tuiles adjacentes
    /// @param ATile: la tuile
    void retrieveTileBeta2(CTile* ATile,
			   const SegmentationMode & ASegmentationMode,
			   const DetectFictiveBordersMode & AMode);

    /// Renseigne la map des beta2 pour les bords haut et gauche d'une tuile
    /// Définit l'opérateur beta2' pour la connexion avec les tuiles adjacentes
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void retrieveTileBeta2(const CPoint2D & APos,
			   const SegmentationMode & ASegmentationMode,
			   const DetectFictiveBordersMode & AMode);

    /// Eclate les régions d'une tuile suivant le critère sélectionné
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void splitTile(const CPoint2D & APos,
		   const FocusAttentionMode & AFocusAttentionMode,
		   const SegmentationMode & ASegmentationMode);

    /// Supprime les sommets de degré 2 de la carte associée à la tuile (bords exclus)
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void simplifyTile(const CPoint2D & APos);

    /// Simplification des sommets de degré 2 des bords de la tuile
    /// @param ATile: la tuile
    void simplifyTileBorder(CTile* ATile);
    void simplifyTileTopBorder(CTile* ATile);
    void simplifyTileLeftBorder(CTile* ATile);

    void delTile(CTile* ATile);

    //******************************************************************************
    // Swap/Load des tuiles
    //******************************************************************************

    /// Retourne la tuile de coordonnées (i,j)
    /// directement depuis le vecteur FTiles si elle est en mémoire ou
    /// depuis un load si elle a été swappée.
    CTile* tile(const CPoint2D & APos);

    /// Ajoute une tuile au niveau courant
    void addTile(CTile* ATile);

    /// Enregistre une tuile sur disque dur
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void writeTile(const CPoint2D & APos);

    /// Enregistre une tuile sur disque dur
    /// @param ATile : la tuile
    void writeTile(CTile* ATile);

    /// Charge une tuile en mémoire
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    /// @return la tuile chargée
    CTile* loadTile(const CPoint2D & APos);
    CTile* loadTile(uint i, uint j);

    /// Charge les propriétés de la tuile
    /// @param ATile : la tuile
    /// @param Ai, Aj, Ak : les coordonnées de la tuile
    void loadTileUpDown(CTile* ATile);

    /// Charge toutes les tuiles en mémoire
    void loadAllTiles();

    /// Supprime la tuile (Ai,Aj,Ak) de la mémoire (du veteur FTiles)
    /// @return true si la tuile a bien été supprimée
    bool unloadTile(const CPoint2D & APos);
    bool unloadTile(uint i, uint j);

    /// Décharge toutes les tuiles en mémoire
    void unloadAllTiles();

    /// unload the whole level
    void unload();

    //******************************************************************************
    // Opérations de contrôle et de vérification
    //******************************************************************************

    /// @return true si la tuile (i,j,k) est chargée en mémoire
    bool isTileLoaded(const CPoint2D & APos);

    /// @return le nombre total de brins du niveau
    uint nbDarts();

    /// @return le nombre de brins fictifs du niveau
    uint nbFictiveDarts();

    /// @return le nombre total de regions dans la pyramide
    uint nbRegions();

    void histogram();

    void regularization(int nb_iteration, int pp, double lambda, int voisinage);

    void printHistogram();

    void print();
  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("level.icc")
//******************************************************************************
#endif // LEVEL_HH
//******************************************************************************
