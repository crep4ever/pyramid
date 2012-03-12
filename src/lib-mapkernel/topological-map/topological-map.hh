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
#ifndef TOPOLOGICAL_MAP_HH
#define TOPOLOGICAL_MAP_HH
//******************************************************************************
#include <map>
#include "map-generic.hh"
#include "image2D.hh"
#include "doublet.hh"
#include "coordinate.hh"
#include "vertex.hh"
#include "khalimsky.hh"
#include "limited-stack.hh"
//******************************************************************************

/** Définit la classe CTopologicalMap qui représente une carte topologique
 *  2D associée à une géométrie de type "matrice de Khalimsky".
 *  Les opérations implémentées dans cette classe sont
 *  topologiques et géométriques.
 *
 * @author Frédéric Vidil, Guillaume Damiand,
 */

namespace Map2d
{
class CTopologicalDart;
class CRegion;

class CTopologicalMap: public CMapGeneric
{
public:
    /// @name Constructeur et destructeur
    //@{

    /**
     *  Constructeur par défaut, qui ne crée aucune structure cad que tout
     *  les pointeurs sont vides. *.
     *  @param ASizeDartArray La taille de chaque tableau de brins
     */
    CTopologicalMap(unsigned int ASizeDartArray = 5000);

    /**
     *  Constructeur avec une image en paramètre.
     *  La matrice de Khalimsky associée est créé à la taille de l'image,
     *  puis l'image est balayée afin de créer la carte et d'initialiser
     *  la matrice.
     *
     *  @param AImage un pointeur vers l'image à extraire.
     *  @param ALevel pour donner le niveau de la carte à extraire
     *         0 = carte complète (tout les pixels)
     *         1 = carte lignel (tout les lignels frontières)
     *         2 = carte topologique
     *  @param ASizeDartArray La taille de chaque tableau de brins
     */
    CTopologicalMap(CImage2D* AImage, unsigned int ALevel = 2,
                    unsigned int ASizeDartArray = 5000);

    /**
     *  Destruction de l'instance.
     *  Tous les brins de la carte ainsi que leurs plongements sont détruits.
     *  L'image, la matrice de Khalimsky et l'arbre d'inclusion également.
     */
    virtual ~CTopologicalMap();

    //@}

    /// @name Création/Suppression de brins
    //@{

    /**
     *  Crée un tableau de FSizeDartArray CDart.
     *  Cette méthode doit être redéfinie à chaque fois que l'on
     *  surcharge la classe brin.
     */
    virtual CDart* newTabDarts();

    /**
     *  Détruit le tableau de FSizeDartArray CDart.
     *  Cette méthode doit être redéfinie à chaque fois que l'on
     *  surcharge la classe brin.
     */
    virtual void deleteTabDarts(CDart* ATabDart);

    /// Récupère le Aj ème brin du Ai ème tableau
    virtual CDart* getDart(unsigned int Ai, unsigned int Aj) const;

     /**
     *  Crée une nouvelle instance de CTopologicalDart sans plongement.
     *  @return Un nouveau brin de type CTopologicalDart
      */
    virtual CDart* addMapDart();

   /**
     *  Crée une nouvelle instance de CTopologicalDart, dont le plongement est
     *  donné par ADoublet.
     *
     *  @param  ADoublet Un sommet
     *  @param  ARegion Un pointeur vers une région
     *  @return Un nouveau brin de type CTopologicalDart
     */
    virtual CTopologicalDart* addMapDart(const CDoublet & ADoublet,
                                         CRegion* ARegion);

    // Copie le contenu du brin ADartSrc dans le brin ADartDest
    virtual void copyDart(CDart* ADartDest, CDart* ADartSrc);

    /**
     *  Méthode permettant de vider la carte ainsi que de détruire les
     *  structures annexes. La méthode removeAllDarts est appelée (pour
     *  garder les marques utilisées). Puis l'image, la matrice de
     *  Khalimsky et l'arbre d'inclusion sont détruit.
     */
    virtual void empty();

    /**
     *  Méthode permettant de vérifier si la carte topologique est valide.
     *  Pour cela on commence par appeler checkTopology puis par vérifier les
     *  propriétés supplémentaires de la carte topologique sur les degrés,
     *  et enfin le plongement des brins.
     *
     *  @return vrai SSI la carte est valide.
     */
    bool isMapOk();

    /**
     * Secoue l'ordre des brins pour avoir un ordre aléatoire.
     * N'a pas d'effet sur le contenue de la carte mais juste sur l'ordre
     * dans lequel seront parcouru les brins.
     */
    void randomizeDarts();

    //@}

    /// Méthode permettant de récupérer la racine de l'arbre d'inclusion.
    CRegion* getInclusionTreeRoot() const;

    //@}

    /// @name Degré et fusions
    //@{

    /**
     *  @param  ADart Un brin de la carte
     *  @return Vrai ssi l'arête incidente à ADart est une boucle
     *          (c'est à dire incidente 2 fois au même sommet).
     */
    bool isEdgeLoop(CDart* ADart);

    /**
     * Calcule le barycentre d'un ensemble de sommets déterminé par un brin
     *   ADart et une orbite AOrbit.
     *
     * @param ADart Un brin de la carte
     * @param AOrbit Une orbite quelconque
     */
    CVertex barycenter(CDart * ADart, TOrbit AOrbit);
    //@}

    /// @return la marque utilisée pour les arêtes fictives
    //int getFictiveMark() const;

    /// @return la taille de l'image en X.
    TNatural getSizeX() const;

    /// @return la taille de l'image en Y.
    TNatural getSizeY() const;

    /// @return la taille de la matrice de Khalimsky en X et Y
    TNatural getKhalimskySizeX() const;
    TNatural getKhalimskySizeY() const;

    /// @return Le nombre de régions de l'arbre d'inclusion
    ///         (sans compter la région infinie).
    unsigned int getNbRegions() const;

    /// @return le nombre d'octets utilisé par la carte.
    unsigned long int getMemoryForMap() const;

    /// @return le nombre d'octets utilisé par l'arbre d'inclusion.
    unsigned long int getMemoryForInclusionTree() const;

    /// @return le nombre d'octets utilisé par la matrice de Khalimsky.
    unsigned long int getMemoryForKhalimsky() const;

    /// @return le nombre d'octets utilisé par l'image.
    unsigned long int getMemoryForImage() const;

    /// Affiche les caractéristiques de la carte (nombre d'éléments et
    /// espace mémoire occupé.
    void displayMapCharacteristics();

    /// Retourne le plus grand niveau de gris de l'image2D.
    TRegionId getMaxGreyLevel() const;

    /**
     *  Retourne le doublet associé au brin.
     *  @param ADart le brin
     *  @return Le doublet.
     */
    CDoublet getDoublet(CDart* ADart) const;

    /**
     *  Retourne le second doublet associé au même linel que le doublet
     * associé au brin.
     *  @param ADart le brin
     *  @return Le doublet.
     */
    CDoublet getOtherDoublet(CDart* ADart) const;

    /**
     *  Affecte le doublet associé au brin.
     *  @param ADart    le brin
     *  @param ADoublet le nouveau doublet
     */
    void setDoublet(CDart* ADart, const CDoublet& ADoublet);

    /**
     *  Retourne la région d'appartenance du brin.
     *  @param ADart le brin
     *  @return La région doublet.
     */
    CRegion* getRegion(CDart* ADart);

    /**
     *  Test pour savoir si un brin est représentant de sa région.
     *  @param ADart le brin
     *  @return Vrai ou faux selon si le brin est représentant ou non.
     */
    bool isRepresentativeDart(CDart* ADart);

    /**
     * Récupère le brin représentant de la région (ou la région mère
     * si on est en cours d'extraction).
     */
    CDart* getRepresentativeDart(CRegion* ARegion);

    /**
     * Fixe le brin représentant de la région (ou la région mère
     * si on est en cours d'extraction).
     */
    void setRepresentativeDart(CRegion* ARegion, CDart* ADart);

    /**
      * Accesseur sur les pixels de l'image.
      */
    TRegionId getPixel(unsigned int x, unsigned int y) const;

    /** Accesseur sur le label d'une image
     */
    TRegionId getPixel( const CCoordinate & ACoord ) const;

    /// Accesseur sur les marques des pixels
    bool isPixelMarked(unsigned int Ax, unsigned int Ay) const;
    void setPixelMark(unsigned int Ax, unsigned int Ay, bool AOn);
    void markPixel(unsigned int Ax, unsigned int Ay);
    void unmarkPixel(unsigned int Ax, unsigned int Ay);
    void unmarkAllPixels();

    /**
     * Donne le pixel interne à la région délimité par le doublet ADoublet.
     * @param ADoublet appartenant à la frontière d'une région.
     * @return Le pixel qui est à l'intérieur.
     */
    CCoordinate getPixelFromDoublet(const CDoublet & ADoublet) const;

    /** Donne le pixel à l'intérieur de la région délimité par le doublet.
     *
     * @see getPixelFromDoublet()
     * @param ADoublet appartenant à la frontière d'une région.
     * @return Le pixel qui est à l'intérieur.
     */
    CCoordinate getPixelIn( const CDoublet & ADoublet ) const;

    /** Donne le pixel à l'extérieur de la région délimité par le doublet.
     *
     * @param ADoublet appartenant à la frontière d'une région.
     * @return Le pixel qui est à l'intérieur.
     */
    CCoordinate getPixelOut( const CDoublet & ADoublet ) const;

    /**
     * Donne le premier pixel d'une région, la graine. C'est le pixel provenant
     * du doublet associé au brin représentant de la région.
     * @return Pixel graine pour la région parcourue.
     */
    CCoordinate getFirstPixel(CRegion* ARegion);

    /// @name Accesseurs sur les cellules
    //@{
    bool isPCell(const CDoublet& ADoublet) const;
    bool isLCell(const CDoublet& ADoublet) const;

    void setPCell(const CDoublet& ADoublet, bool AOn = true);
    void setLCell(const CDoublet& ADoublet, bool AOn = true);

    CDoublet normaliseDoubletPointel(const CDoublet& ADoublet) const;
    CDoublet normaliseDoubletLinel(const CDoublet& ADoublet) const;

    //@}

    /// @name Méthodes pour explorer la géométrie.
    //@{
     /// @return le degré du pointel ADoublet
    ///   (cad incident le nombre de lignels incident).
    unsigned int getPointelDegree(const CDoublet& Adoublet) const;

   /// @return vrai ssi le pointel de ADoublet est de degré 2
    ///   (cad incident exactement à 2 lignels).
    bool isDegreeTwoPointel(const CDoublet& Adoublet) const;

    /**
     *  Retourne l'autre lignel incident au pointel de ADoublet en tournant
     * dans le sens anti-trigonométrique, ou ADoublet si un tel pointel
     * n'existe pas.
     *
     *  @return l'autre lignel désigné par un CDoublet.
     */
    CDoublet getOtherLinel(const CDoublet& ADoublet) const;

    //@}

  /** Charge une nouvelle image comme donnée pour la carte topologique.
   *
   * @param AImage nouvelles données images
   * @pre La taille de l'image doit être égale à la taille de la carte
   * @return true si le changement est un succès et false sinon.
   */
  bool changeImageData( CImage2D * AImage );

  /** Retourne un pointeur sur l'image actuelle.
   *
   * @return pointeur sur les données images de la carte topologique.
   */
  CImage2D * getImage();

    /**
     * Marque les faces sauf la face infinie et les faces adjacentes
     */
    void markInteriorWithoutInfiniteFace(unsigned int AMark);

    /**
     * Marque les faces de la plus grande composante connexe de faces
     * sauf la face infinie et les faces adjacentes.
     */
    void markBiggestCC(unsigned int AMark);

    ///  Calcule la distance maximale entre tout les pointels de l'arête
    /// incidente à ADart et le segment [AV1, AV2]
    TCoordinate distanceToCurve(CDart* ADart, const CVertex & AV1,
                                const CVertex& AV2);

    ///  Calcule la distance maximale entre tout les pointels de l'arête
    /// incidente à ADart et le segment [AV1, AV2], en restant sous le seuil
    /// AThreshold. Dès qu'une distance est >= à AThreshold, s'arrête.
    TCoordinate distanceToCurve(CDart* ADart, const CVertex & AV1,
                                const CVertex& AV2, TCoordinate AThreshold);

    /// Sauve l'image représentée par la partition courante.
    /// La valeur de chaque pixel d'une même région vaut la moyenne des
    /// couleurs de la région
    /// @param AFilename le nom de l'image à sauver
    bool saveImage(const char* AFilename);

    /// Sauve l'image de label représentée par la partition courante.
    /// Chaque région est associé à un label (un nombre entre 1 et nbrégions)
    /// La valeur de chaque pixel d'une même région vaut ce label.
    /// @param AFilename le nom de l'image à sauver
    bool savePartition(const char* AFilename);

    /// Export the topomap in a graph format.
    /// if amark!=-1, only vertices and edges having one dart marked are exported.
    /// if amark==-1, the whole map is exported.

  void exportGraph(std::ostream &os, int amark=-1);

private:
     /**
     * @return vrai ssi ARegion est une racine d'un arbre uf.
     * rmq: On utilise le pointeur FFather pour construire l'arbre
     * d'une composante connexe.
     * @pre FUFTreeMode
     */
    bool isRegionRoot(CRegion* ARegion) const;

  /**
     * @return vrai ssi la région de ADart est une racine d'un arbre uf.
     * rmq: On utilise le pointeur FFather pour construire l'arbre
     * d'une composante connexe.
     * @pre FUFTreeMode
     */
    bool isRegionRoot(CDart* ADart) const;

    /**
     * Réalise l'union entre une région et sa racine
     * si la région est déjà la racine de l'arbre on n'effectue rien...
     * Comme précédemment, on utilise le pointeur FFather pour construire
     * l'arbre d'une composante connexe.
     * @return false si les regions racines de last et up étaient égales
     * et true si l'opération de fusion est réalisée.
     * @pre FUFTreeMode
     */
    bool unionRegionRoot(CRegion* ARegion1, CRegion* ARegion2);

protected:
    /// Initialise un brin donné (nécessaire car on n'utilise plus le
    /// constructeur des brins avex paramètres).
    void initDart(CDart* ADart, const CDoublet & ADoublet, CRegion* ARegion);

    /**
     * Retrouve la  racine de l'arbre correspondant à la région passée en
     * paramètre
     * rmq: On utilise le pointeur FFather pour construire l'arbre
     * d'une composante connexe.
     * @pre FUFTreeMode
     */
    CRegion* findRegionRoot(CRegion* ARegion);

    /**
     * Initialise un brin à une région
     */
    void setRegion(CDart* ADart, CRegion* ARegion) const;

protected:
    /// @name Champs privés
    //@{
    CImage2D*   FImage;
    CKhalimsky* FKhalimsky;
    CRegion*    FInclusionTreeRoot;
    /// Compteurs pour le nombre de régions
    unsigned int FNbRegions;

private:
    /// vrai si on est en mode uftree: dans ce cas, le champ Father de chaque
    /// région désigne le père dans l'arbre uf, null pour les racines.
    bool FUFTreeMode;

  unsigned int FLastCavityCount;
    //@}

public:
    unsigned int FMaxLg;

  /// @name Fichiers header annexes
  //@{
#include "tm-extract-images.hh"
#include "tm-segmentation.hh"
#include "tm-deformation.hh"
#include "tm-isomorphism.hh"
  //@}
};
//******************************************************************************
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("topological-map.icc")
#include INCLUDE_INLINE("tm-extract-images.icc")
//******************************************************************************
#endif // TOPOLOGICAL_MAP_HH
//******************************************************************************
