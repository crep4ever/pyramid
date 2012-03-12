/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/
 *
 * This file is part of lib-mapkernel
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
// Ce fichier fait partie de topological-map.hh
//******************************************************************************

public:
    /**
     * Transforme l'arbre d'inclusion en la liste de régions ordonnées (en
     * utilisant les champs de la manière expliquée pour computeInclusionTree)
     * C'est la méthode "inverse" de computeInclusionTree.
     */
    void transformInclusionTreeInOrderedList();

    /**
     * Parcours toute la carte en fusionnant les couples r1,r2 de régions
     * voisines vérifiant r1->AMethod(r2,AThreshold).
     * @param AThreshold le seuil en dessous duquel on fusionne
     * @param ASort un booléen pour trier les arêtes par gradient croissant
     *       si vrai (si faux les arêtes sont randomisées).
     * @return le nombre de régions fusionnées
     */
    unsigned int mergeRegions(bool (CRegion::*AMethod)
                              (CTopologicalMap*,CRegion*,int),
                              int AThreshold, bool ASort=true);

    /**
     * Parcours toute la carte en fusionnant les couples r1,r2 de
     * régions voisines vérifiant r1->AMethod(r2,AThreshold) et
     * r1->AMethod2(r2,currentDart,AConstraint)
     * @param AThreshold le seuil en dessous duquel on fusionne
     *@param ASort un booléen pour trier les arêtes par gradient
     * croissant si vrai (si faux les arêtes sont randomisées).
     * @return le nombre de régions fusionnées
     */
     unsigned int mergeRegionsWithEdgeConstraint
     (bool (CRegion::*AMethod)(CTopologicalMap*,CRegion*,int),
      int AThreshold, bool (CRegion::*AMethod2)(CTopologicalMap*,CDart*,int),
      int AConstraint, bool ASort);

    /// Segmente la carte en fusionnant les régions proche selon AThreshold.
    /// @param AThreshold le seuil en dessous duquel on fusionne
    /// @param ASort un booléen pour trier les arêtes par gradient croissant
    ///        si vrai (si faux les arêtes sont randomisées).
    /// @return le nombre de régions fusionnées
    unsigned int segment(unsigned int AThreshold, bool ASort=true);

    /// Segmente la carte en fusionnant les régions proche selon AThreshold,
    /// le seuil est pondéré par la longueur des arêtes
    /// @param AThreshold le seuil en dessous duquel on fusionne
    /// @param ASort un booléen pour trier les arêtes par gradient croissant
    ///        si vrai (si faux les arêtes sont randomisées).
    /// @return le nombre de régions fusionnées
    unsigned int segmentWithPonderation(unsigned int AThreshold,
				      bool ASort=true);

    /// Supprime les petites régions (de taille < à AThreshold) en les
    /// fusionnants avec leur région voisine la plus proche en terme de couleur.
    /// @return le nombre de petites régions supprimées
    unsigned int removeSmallRegions(unsigned int AThreshold);

    /**
     * Fonction relabélisant les brins après la fusion de régions pour qu'ils
     * appartiennent tous à la région mère dans les arbres union-find
     * utilisés de manière temporaire durant la fusion. De plus, l'arbre
     * d'inclusion est transformé en liste ordonnée.
     */
    void relabelDartsFromInclusionTree();

    /// Segmente la carte en fusionnant les régions proche selon AThreshold.
    /// et en respectant la contrainte ACavity
    /// @param AThreshold le seuil en dessous duquel on fusionne
    /// @param ACavity contrainte de cavité (-2 -> diminution, -1 ->
    /// no change, sinon convergence)
    /// @param ASort un booléen pour trier les arêtes par gradient croissant
    ///        si vrai (si faux les arêtes sont randomisées).
    /// @return le nombre de régions fusionnées
    unsigned int segmentWithCavityConstraint(unsigned int AThreshold, int ACavity, bool ASort=true);

    /**
     * Fonction calculant pour chaque région le nombre de cavité
     */
    void initializeCavityCount();


/**
 * Fonction stockant la dernière valeur de cavité
 * @param ALastCavityCount dernière valeur de cavité
 */
void setLastCavityCount(unsigned int ALastCavityCount);

/**
 * Fonction retournant la dernière valeur de cavité sotckée
 * @return dernière valeur de cavité stockée
 */
unsigned int lastCavityCount() const;
