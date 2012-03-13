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
    ///@name Pour l'extraction d'images.
    //@{

    /**
     *  Extrait la carte topologique de l'image passé en paramètre.
     *
     *  @param AImage un pointeur vers l'image à extraire.
     *  @param ALevel pour donner le niveau de la carte à extraire
     *         0 = carte complète (tout les pixels)
     *         1 = carte lignel (tout les lignels frontières)
     *         2 = carte topologique
     */
    void extractTopologicalMap(CImage2D* AImage, unsigned int ALevel = 2);

    //@}

    /**
     *  Supprime le sommet incident à ADart en "recollant" les deux arêtes
     *  autour de ce sommet. Les brins sont directement supprimés.
     *  Cette méthode fonctionne uniquement si le sommet est de degré 2, cad
     *  incident uniquement à deux arêtes.
     *  Attention : Ne met pas à jour les brins représentants
     *
     *  @precondition Le sommet incident à ADart est de degré 2
     *  @param  ADart Un brin de la carte
     *  @param  AMarkNumber Un numéro de marque pour marquer les brins
     *          à supprimer. Par défaut -1 => les brins sont détruits.
     */
    void basicVertexRemoval(CDart* ADart, int AMarkNumber = -1);

    /**
     * Supprime le sommet en utilisant la méthode basicVertexRemoval et en
     * ajoutant la mise à jour des brins représentants si besoin
     */
    void vertexRemoval(CDart* ADart, int AMarkNumber = -1);

    /**
     *  Supprime l'arête incidente à ADart en "recollant" les deux faces
     *  autour de cette arête. Seul la topologie (les brins) sont supprimés,
     *  la matrice d'éléments interpixel n'est pas mise à jour (i.e. les linels)
     *
     *  @param  ADart       Un brin de la carte
     */
    void topoEdgeRemoval(CDart* ADart);

    /**
     * Supprime topologiquement l'arête incidente à ADart (cf. topoEdgeRemoval)
     * en effaçant en plus tout les linels de l'arête dans la matrice d'éléments
     * interpixel.
     *
     *  @param  ADart       Un brin de la carte
     */
    void edgeRemoval(CDart* ADart);

    /**
     *  Calcule l'arbre d'inclusion des régions. La liste AList est la liste
     *  des régions rencontrées dans l'ordre du balayage
     *  (haut-bas, derrière-devant et gauche-droite).
     *   La liste est désignée par :
     * - 1er élément :   région infinie
     * - getFirstSon() : région suivante
     * - getBrother() :  région précédente
     * - le brother du 1er élément est la dernière région
     * - le son du dernier élément est NULL
     */
    void computeInclusionTree();

private:
    /**
     *  Boucle principale pour l'extraction de la carte topologique.
     *
     *  @param AImage un pointeur vers l'image à extraire.
     *  @param ALast  le dernier brin, résultat de l'appel précédent à
     *                extractTopologicalMapOneStep
     *  @param ALevel pour donner le niveau de la carte à extraire
     *         0 = carte complète (tout les pixels)
     *         1 = carte lignel (tout les lignels frontières)
     *         2 = carte topologique
     */
    CDart* extractTopologicalMapMainLoop(CImage2D* AImage, CDart* ALast,
                                         unsigned int ALevel = 2);

    /**
     * Fonction relabélisant les brins après l'extraction pour qu'ils
     * appartiennent tous à la région mère dans les arbres union-find
     * utilisés de manière temporaire durant l'extraction.
     */
    void relabelDarts();

    ///@name Création d'objet de base.
    //@{

    /**
     *  Crée une face carré plongée (chaque brin correspond à un
     *    doublet dans la matrice).
     *
     *  @param  Deux CDart* pour récuperer les brins up et last à coudre
     *          a la nouvelle face.
     *  @param  ADoublet le doublet du premier brin de la face.
     *  @param  ARegion  la région d'appartenance de la face.
     *  @return Le premier brin de la face.
     */
    CDart* createSquareFace(CDart* ALast, CDart* AUp,
                            const CDoublet& ADoublet, CRegion* ARegion);

    //@}

    ///@name Méthodes privées pour l'extraction d'images.
    //@{

    /**
     *  Crée le bord initial supérieur avant l'extraction d'une image 2D.
     *
     *  @param ALargX largeur en X de l'image
     *  @param ALargY largeur en Y de l'image
     *  @return Le brin en haut à gauche incident au premier pointel
     *          (cf. thèse page 166).
     */
    CDart* makeBorder(int ALargX, int ALargY);

   /**
    * Effectue la simplification complète de la carte pour supprimer tout les
    * sommets de degré deux.
    */
    void simplifyMap();
    //@}

    ///@name Méthodes protégées pour l'extraction d'images.
    //@{

protected:
    /**
     *  Calcule le brin "up" à partir du brin "last".
     *
     *  @param  ADart le brin "last".
     *  @return Le brin "up".
     */
    CDart* computeUpFromLast(CDart* ADart);

    /**
     *  Détruit le bord initial supérieur après l'extraction d'une image 2D.
     *  (ce bord est la surface incidente à ADart).
     *
     *  @param ADart le dernier last qui appartient au bord à détruire.
     */
    void destroyBorder(CDart* ADart);
    //@}
