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
#include "point2d.hh"
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
     * Supprime le sommet en utilisant la méthode basicVertexRemoval et en
     * ajoutant la mise à jour des brins représentants si besoin, mais sachant
     * que les régions sont éventuellement fusionnées dans des arbres UF.
     */ 
    void vertexRemovalWithUnionFindRegions(CDart* ADart, int AMarkNumber = -1);
   
    /**
     *  Supprime l'arête incidente à ADart en "recollant" les deux faces
     *  autour de cette arête. Seul la topologie (les brins) sont supprimés, 
     *  la matrice d'éléments interpixel n'est pas mise à jour (i.e. les linels)
     *
     *  @param  ADart       Un brin de la carte
     */
    void topoEdgeRemoval(CDart* ADart);

    /**
     *  Supprime topologiquement l'arête incidente à ADart (cf. topoEdgeRemoval)
     * en effaçant en plus tout les linels de l'arête dans la matrice d'éléments
     * interpixel.
     *
     *  @param  ADart       Un brin de la carte
     */
    void edgeRemoval(CDart* ADart);

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
    TRegionId getPixel( const CPoint2D & ACoord ) const;

    /// Accesseur sur les marques des pixels
    virtual bool isPixelMarked(unsigned int Ax, unsigned int Ay) const;
    virtual void setPixelMark(unsigned int Ax, unsigned int Ay, bool AOn);
    virtual void markPixel(unsigned int Ax, unsigned int Ay);
    virtual void unmarkPixel(unsigned int Ax, unsigned int Ay);
    virtual void unmarkAllPixels();

    /**
     * Donne le pixel interne à la région délimité par le doublet ADoublet.
     * @param ADoublet appartenant à la frontière d'une région.
     * @return Le pixel qui est à l'intérieur.
     */
    CPoint2D getPixelFromDoublet(const CDoublet & ADoublet) const;

    /** Donne le pixel à l'intérieur de la région délimité par le doublet.
     *
     * @see getPixelFromDoublet()
     * @param ADoublet appartenant à la frontière d'une région.
     * @return Le pixel qui est à l'intérieur.
     */
    CPoint2D getPixelIn( const CDoublet & ADoublet ) const;

    /** Donne le pixel à l'extérieur de la région délimité par le doublet.
     *
     * @param ADoublet appartenant à la frontière d'une région.
     * @return Le pixel qui est à l'intérieur.
     */
    CPoint2D getPixelOut( const CDoublet & ADoublet ) const;

    /**
     * Donne le premier pixel d'une région, la graine. C'est le pixel provenant
     * du doublet associé au brin représentant de la région.
     * @return Pixel graine pour la région parcourue.
     */
    CPoint2D getFirstPixel(CRegion* ARegion);

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
     * Cherche si la carte *this est un motif de la carte AMap.
     * AMarkToTest est un numéro de marque, seul les brins de *this marqués
     * sont testés.
     * @param ANbMatched si !=NULL, parametre retour qui compte le
     *        plus grand nombre de brins avant de sortir.
     * @param AMirror vrai pour tester l'isomorphisme mirroir
     * @return -1 si motif non trouvé, le numéro du direct info contenant
     * l'injection sinon (les autres ont l'injection à NULL).
     */
    int findMotif(unsigned int AMarkToTest, CTopologicalMap* AMap,
                  unsigned int* ANbMatched, bool AMirror = false);

    /**
     * Marque les faces sauf la face infinie et les faces adjacentes
     */
    void markInteriorWithoutInfiniteFace(unsigned int AMark);

    /**
     * Marque les faces de la plus grande composante connexe de faces
     * sauf la face infinie et les faces adjacentes.
     */
    void markBiggestCC(unsigned int AMark);  
  
    /**
     * Cherche si la carte *this est un motif de la carte AMap à partir du
     * brin AFromDart dans la carte this et ADestDart dans la carte AMap.
     * AIndex est le numéro du direct info utilisé pour l'injection.
     * AMarkToTest est un numéro de marque, seul les brins de *this marqués
     * sont testés.
     * AMarkTreated est le numéro de marque utilisé pour marquer les brins
     * déjà traités.
     * @param ANbMatched si !=NULL, parametre retour qui compte le
     *        plus grand nombre de brins avant de sortir.
     * @param AMirror vrai pour tester l'isomorphisme mirroir
     * @return vrai si motif est trouvé, dans ce cas, le direct info contient
     * l'injection.
     */
    bool findMotifFrom(CDart* AFromDart, unsigned int AIndex,
                       unsigned int AMarkToTest,
                       unsigned int AMarkTreated,
                       CTopologicalMap* AMap, CDart* ADestDart,
                       unsigned int AMarkTreated2,
                       unsigned int* ANbMatched=NULL,
		       bool AMirror = false);

    /**
     * Teste si la face AFromDart matche avec la face ADestDart, c'est à dire
     * si les faces ont le même nombre de brins, et si les voisins par beta2
     * sont compatible (i.e. soit non affecté, soit en relation par
     * l'injection se trouvant dans le directInfo[AIndex]).
     * L'injection des brins de la face est mise à Null et les brins de la
     * face sont marqués traités.
     */
    bool isFacesMatch(CDart* AFromDart, int AIndex,
                      unsigned int AMarkTreated,
                      CTopologicalMap* AMap, CDart* ADestDart);

    /**
     * Cherche le motif maximal à partir des brins AFromDart et ADestDart.
     * Retourne le nombre de brins du motif matché. les brins du motif ont
     * leurs champs directInfo[AIndex] non null désignant l'injection.
     * @pre le champ direct info de tout les brins doit être null
     */
    unsigned int findMaximalMotifFrom(CDart* AFromDart,
                                      unsigned int AMarkTreated,
                                      int AIndex,
                                      CTopologicalMap* AMap,
                                      CDart* ADestDart);

    /**
     * Cherche le motif maximal à partir des brins AFromDart et
     * ADestDart V2.
     * Retourne le nombre de brins du motif matché. les brins du motif ont
     * leurs champs directInfo[AIndex] non null désignant l'injection.
     * @pre le champ direct info de tout les brins doit être null
     */
    unsigned int findMaximalMotif2From(CDart* AFromDart,
                                       unsigned int AMarkTreated,
                                       int AIndex,
                                       CTopologicalMap* AMap,
                                       CDart* ADestDart,
                                       unsigned int AMarkTreated2);

    /**
    * Cherche le motif maximal entre les 2 cartes this et AMap.
    * Retourne le nombre de brins du motif matché. les brins du motif ont
    * leur champ directInfo[AIndex] non null et désignant l'injection.
    */
    unsigned int findMaximalMotif(int AIndex, CTopologicalMap* AMap);

    /// Met le champ directinfo du motif d'origine ADart à Null.
    /// Le parcours du motif utilise la marque AMark.
    /// Si AIndex!=-1, met à Null le champ directInfo[AIndex].
    /// Si AIndexCopy!=-1, recopie directInfo[AIndex] dans
    ///    directInfo[AIndexCopy].
    /// Si AMark2!=-1 démarque aussi les brins de AMap.
    void unmarkMotifMark(CDart* ADart, int AMark, int AIndex = -1,
                         int AIndexCopy = -1, CTopologicalMap* AMap = NULL,
                         CDart* ADart2 = NULL, int AMark2 = -1,
                         bool AMirror = false);

    /// Met le champ directinfo du motif d'origine ADart à Null. Le parcours
    /// du motif utilise le champ directinfo.
    void unmarkMotifIndex(CDart* ADart, int AIndex);

    ///  Calcule la distance maximale entre tout les pointels de l'arête
    /// incidente à ADart et le segment [AV1, AV2]
    TCoordinate distanceToCurve(CDart* ADart, const CVertex & AV1,
                                const CVertex& AV2);

    ///  Calcule la distance maximale entre tout les pointels de l'arête
    /// incidente à ADart et le segment [AV1, AV2], en restant sous le seuil
    /// AThreshold. Dès qu'une distance est >= à AThreshold, s'arrête.
    TCoordinate distanceToCurve(CDart* ADart, const CVertex & AV1,
                                const CVertex& AV2, TCoordinate AThreshold);
    
    /**
     * Parcours toute la carte en fusionnant les couples r1,r2 de régions 
     * voisines vérifiant r1->AMethod(r2,AThreshold). 
     * @return le nombre de régions fusionnées
     */
    unsigned int mergeRegions(bool (CRegion::*AMethod)(CRegion*,int)const, 
                              int AThreshold);
    
    /// Segmente la carte en fusionnant les régions proche selon AThreshold.
    /// @return le nombre de régions fusionnées
    unsigned int segment(unsigned int AThreshold);
    
    /// Supprime les petites régions (de taille < à AThreshold) en les 
    /// fusionnants avec leur région voisine la plus proche en terme de couleur.
    /// @return le nombre de petites régions supprimées
    unsigned int removeSmallRegions(unsigned int AThreshold);
   
    /// Sauve l'image représentée par la partition courante.
    /// La valeur de chaque pixel d'une même région vaut la moyenne des
    /// couleurs de la région
    /// @param AFilename le nom de l'image à sauver
    bool savePartition(const char* AFilename);
    
public:
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

protected:
    /**
     * Fonction relabélisant les brins après l'extraction pour qu'ils
     * appartiennent tous à la région mère dans les arbres union-find
     * utilisés de manière temporaire durant l'extraction.
     */
    void relabelDarts();

public:
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

protected:
    ///@name Méthodes privées pour l'extraction d'images.
    //@{

    /**
     *  Calcule le brin "up" à partir du brin "last".
     *
     *  @param  ADart le brin "last".
     *  @return Le brin "up".
     */
    CDart* computeUpFromLast(CDart* ADart);

    /**
     * Initialise un brin à une région
     */
    void setRegion(CDart* ADart, CRegion* ARegion) const;

    /**
     * Retrouve la  racine de l'arbre correspondant à la région passée en
     * paramètre
     * rmq: On utilise le pointeur FNextSameCC pour construire l'arbre
     * d'une composante connexe.
     */
    CRegion* findRegionRoot(CRegion* Region);

    /**
     * Réalise l'union entre une région et sa racine 
     * si la région est déjà la racine de l'arbre on n'effectue rien...
     * Comme précédemment, on utilise le pointeur FNextSameCC pour construire 
     * l'arbre d'une composante connexe.
     * @return 0 si les regions racines de last et up sont égales 
     * et 1 si l'opération de fusion est réalisée.
     */
    int unionRegionRoot(CRegion* Region1, CRegion* Region2);

public:      
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

public:
    /**
     * Transforme l'arbre d'inclusion en la liste de régions ordonnées (en 
     * utilisant les champs de la manière expliquée pour computeInclusionTree)
     * C'est la méthode "inverse" de computeInclusionTree.
     */
    void transformInclusionTreeInOrderedList();
    
    /**
     *  Crée le bord initial supérieur avant l'extraction d'une image 2D.
     *
     *  @param ALargX largeur en X de l'image
     *  @param ALargY largeur en Y de l'image
     *  @return Le brin en haut à gauche incident au premier pointel
     *          (cf. thèse page 166).
     */
    CDart* makeBorder(int ALargX, int ALargY);

protected:
    /**
     *  Détruit le bord initial supérieur après l'extraction d'une image 2D.
     *  (ce bord est la surface incidente à ADart).
     *
     *  @param ADart le dernier last qui appartient au bord à détruire.
     */
    void destroyBorder(CDart* ADart);
    
   /**
    * Effectue la simplification complète de la carte pour supprimer tout les
    * sommets de degré deux.
    */
    void simplifyMap();    
    //@}

    /// Initialise un brin donné (nécessaire car on n'utilise plus le
    /// constructeur des brins avex paramètres).
    void initDart(CDart* ADart, const CDoublet & ADoublet, CRegion* ARegion);

protected:
    /// @name Champs privés
    //@{
    CImage2D*   FImage;
    CKhalimsky* FKhalimsky;
    CRegion*    FInclusionTreeRoot;

    /// Compteurs pour le nombre de régions
    unsigned int FNbRegions;
    //@}

  /// @name Fichiers header annexes
  //@{
#include "tm-deformation.hh"
  //@}
};
//******************************************************************************
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("topological-map.icc")
#include INCLUDE_INLINE("tm-extract-images.icc")
#include INCLUDE_INLINE("tm-deformation.icc")
//******************************************************************************
#endif // TOPOLOGICAL_MAP_HH
//******************************************************************************
