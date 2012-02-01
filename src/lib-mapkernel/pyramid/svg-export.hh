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
#ifndef SVG_EXPORT_HH
#define SVG_EXPORT_HH
//******************************************************************************
#include "svg-creator.hh"
#include "vertex.hh"
#include "doublet.hh"
//******************************************************************************
namespace Map2d
{
  class CDart;
  class CRegion;
  class CTile;
  class CPyramid;

  /**
   * \file svg-export.hh
   * \class CSvgExporter "svg-export.hh"
   * \author Romain Goffe
   * \brief Svg exporter for tiled top-down pyramids
   */
  class CSvgExporter
  {
  public:
    /// Export svg de la pyramide
    /// vers ./output/pyramid.svg
    /// @param APerspective: ajoute un effet de perspective aux niveaux
    void drawPyramid(CPyramid* APyramid, bool APerspective, bool ATopology, bool AGeometry);

    /// Export svg des niveaux de la pyramide
    /// vers ./output/pyramid-level-id.svg
    void drawPyramidLevels(CPyramid* APyramid, bool ATopology=true, bool AGeometry=true);

    /// Export svg des informations topologiques des niveaux de la pyramide
    /// vers ./output/pyramid-topology-level-id.svg
    void drawPyramidTopology(CPyramid* APyramid);

    /// Export svg des informations géométriques des niveaux de la pyramide
    /// vers ./output/pyramid-geometry-level-id.svg
    void drawPyramidGeometry(CPyramid* APyramid);

    /// Export svg des informations topologiques de la carte
    /// vers ./output/map-topology-id.svg
    void drawTileTopology(CTile* ATile);

    /// Export svg des informations géométriques de la carte
    /// vers ./output/map-geometry-id.svg
    void drawTileGeometry(CTile* ATile);

    /// Export svg de la carte  vers ./output/map-id.svg
    void drawTile(CTile* ATile);

    /// Export svg d'une région vers ./output/region-id.svg
    /// @param ARegion: la région à exporter
    void drawRegion(CTile* AMap, CRegion* ARegion);

    /// Export svg d'un brin vers ./output/brin-id.svg
    /// @param ADart: le brin à exporter
    void drawDart(CTile* AMap, CDart* ADart);

    /// Représente les first pixels des régions dans l'export svg
    /// @param ASvg: le flux dans lequel on ajoute les informations des first pixels
    void drawFirstPixels(CTile* AMap, CSvgCreator& svg);

    /// Génération du contenu svg de la topologie d'un niveau
    /// @param ADepth: le niveau de  profondeur
    /// @param ASvg: le flux dans lequel on ajoute les informations
    void addSvgPyramidTopology(CPyramid* APyramid, CSvgCreator& svg, unsigned int ADepth);

    /// Génération du contenu svg de la géométrie d'un niveau
    /// @param ADepth: le niveau de profondeur
    /// @param ASvg: le flux dans lequel on ajoute les informations
    void addSvgPyramidGeometry(CPyramid* APyramid, CSvgCreator& svg, unsigned int ADepth);

    /// Génération du contenu svg de la topologie de la carte
    /// @param ASvg: le flux dans lequel on ajoute les informations
    void addSvgMapTopology(CTile* AMap, CSvgCreator& svg);

    /// Génération du contenu svg de la géométrie de la carte
    /// @param ASvg: le flux dans lequel on ajoute les informations
    void addSvgMapGeometry(CTile* AMap, CSvgCreator& svg);

    /// Génération du contenu svg de la région
    /// @param ARegion: la région concernée
    /// @param ASvg: le flux dans lequel on ajoute les informations de la région
    void addSvgRegion(CTile* AMap, CRegion* ARegion, CSvgCreator& ASvg);

    /// Génération du contenu svg du brin
    /// @param ADart: le brin concerné
    /// @param ASvg: le flux dans lequel on ajoute les informations du brin
    void addSvgDart(CTile* AMap, CDart* ADart, CSvgCreator& ASvg);

    /// Colorise les régions d'une carte d'après leur niveau de gris moyen
    /// @param AMap: la carte
    void fillRegions(CTile* AMap, CSvgCreator& svg);

    /// Calcule le décalage des points de passage des paths pour "séparer" les
    /// brins lors du rendu svg
    /// @param x1,y1: les coordonnées du point de départ
    /// @param current: le doublet partant de x1,y1 et indiquant la direction à suivre
    /// @param next: le doublet suivant
    /// @return CVertex: les coordonnées "décalées" du point entre current et next
    CVertex calculateGap(const CVertex & APoint, const CDoublet & current, const CDoublet & next );

  };
}
//******************************************************************************
#endif // SVG_EXPORT_HH
//******************************************************************************
