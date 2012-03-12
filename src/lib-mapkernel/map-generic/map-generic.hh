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
#ifndef MAP_GENERIC_HH
#define MAP_GENERIC_HH
//******************************************************************************
#include "map.hh"
//******************************************************************************

/** Définit la classe CMapGeneric qui représente une 2-carte.
 *  Les opérations implémentées dans cette classe sont
 *  toutes topologiques.
 *  Celles qui modifient la carte ne mettent pas à jour les
 *  plongements (exemple: triangulation, qui ne duplique pas
 *  les plongements 'face' lors de la triangulation d'une face).
 *  La mise à jour des plongements doit donc être faite dans les
 *  méthodes des classes qui dérivent de CMapGeneric.
 *
 * @author Frédéric Vidil, Guillaume Damiand,
 */

namespace Map2d
{
  class CMapGeneric: public CMap
  {
  public:
    /** @name Constructeur et destructeur
     */

    //@{

    /** Constructeur par défaut
     *
     *  @return Une nouvelle instance de la classe
     *
     *  Construction d'une nouvelle instance de la classe.
     *  La carte créée est vide.
     */
    CMapGeneric(unsigned int ASizeDartArray = 1000);

    /** Destructeur
     *
     *  Destruction de l'instance.
     *  Tous les brins de la carte ainsi que leurs plongements sont détruits.
     */
    virtual ~CMapGeneric();

    //@}

    /** @name Accesseurs
     */

    //@{

    /** beta----
     *
     *  @param ADart Un pointeur sur un brin quelconque
     *  @return Un pointeur sur un brin
     *
     *  Ces méthodes permettent de composer les involutions pour accéder
     *  facilement aux brins de la carte. Les indices sont donnés selon
     *  l'ordre dans lequel ils sont appliqués.
     *  Exemple: beta12(d) signifie beta2(beta1(d))).
     */

    CDart* beta00   (CDart* ADart) const;
    CDart* beta02   (CDart* ADart) const;
    CDart* beta020  (CDart* ADart) const;
    CDart* beta0202 (CDart* ADart) const;
    CDart* beta02020(CDart* ADart) const;
    CDart* beta0211 (CDart* ADart) const;
    CDart* beta11   (CDart* ADart) const;
    CDart* beta1111 (CDart* ADart) const;
    CDart* beta112  (CDart* ADart) const;
    CDart* beta1120 (CDart* ADart) const;
    CDart* beta12   (CDart* ADart) const;
    CDart* beta121  (CDart* ADart) const;
    CDart* beta20   (CDart* ADart) const;
    CDart* beta21   (CDart* ADart) const;
    CDart* beta212  (CDart* ADart) const;


    /** getDirectInfoAsDart
     *
     *  @param ADart Un brin de la carte
     *  @param ADirectInfoIndex Un indice de champ directInfo
     *  @return Un brin de la carte
     *
     *  Retourne le champ directInfo demandé du brin ADart après l'avoir
     *  retypé en pointeur sur un brin.
     */
    CDart* getDirectInfoAsDart(CDart* ADart, int ADirectInfoIndex);

    /** @name Méthodes de marquage des brins
     */

    //@{

    /** markOrbit
     *
     *  @param ADart Un brin quelconque
     *  @param AOrbit Une orbite
     *  @param AMarkNumber Un numéro de marque
     *
     *  Marque tous les brins de l'orbite AOrbit incidente à ADart en utilisant
     *  la marque numéro AMarkNumber.
     */
    void markOrbit(CDart* ADart, TOrbit AOrbit, int AMarkNumber);

    /** unmarkOrbit
     *
     *  @param ADart Un brin quelconque
     *  @param AOrbit Une orbite
     *  @param AMarkNumber Un numéro de marque
     *
     *  Démarque tous les brins de l'orbite AOrbit incidente à ADart pour la
     *  marque numéro AMarkNumber.
     */
    void unmarkOrbit(CDart* ADart, TOrbit AOrbit, int AMarkNumber);

    /**
     * Marque un brin sur deux de l'orbite AOrbit incidente à ADart en
     * utilisant la marque numéro AMarkNumber.
     * Attention: Cette méthode ne fonctionne pas si l'orbite est une orbite de
     * bord !
     *
     * @param ADart Un brin quelconque
     * @param AOrbit Une orbite
     * @param AMarkNumber Un numéro de marque
     *
     * @precondition AOrbit n'est pas une orbite de bord
     */
    void halfMarkOrbit(CDart* ADart, TOrbit AOrbit, int AMarkNumber);

    /**
     * Démarque un brin sur deux de l'orbite AOrbit incidente à ADart en
     * utilisant la marque numéro AMarkNumber.
     * Attention: Cette méthode ne fonctionne pas si l'orbite est une orbite de
     * bord !
     *
     * @param ADart Un brin quelconque
     * @param AOrbit Une orbite
     * @param AMarkNumber Un numéro de marque
     *
     * @precondition AOrbit n'est pas une orbite de bord
     */
    void halfUnmarkOrbit(CDart* ADart, TOrbit AOrbit, int AMarkNumber);

    /** unmarkAll
     *
     * @param AMarkNumber Un numéro de marque
     *
     * Démarque tous les brins de la carte pour la marque numéro AMarkNumber.
     */
    void unmarkAll(int AMarkNumber);

    /** markIncidentCells
     *
     *  @param AOrbit Une orbite quelconque
     *  @param AMarkNumberSrce Un numéro de marque
     *  @param AMarkNumberDest Un numéro de marque
     *  @return Le nombre de cellules marquées
     *
     *  @pre AMarkNumberSrce!=AMarkNumberDest
     *
     *  Marque avec la marque AMarkNumberDest toutes les cellules correspondant
     *  à l'orbite AOrbit dont un brin au moins est marqué avec la marque
     *  AMarkNumberSrce.
     */
    int markIncidentCells(TOrbit AOrbit, int AMarkNumberSrce,
			  int AMarkNumberDest);

    /** markIncidentCells
     *
     *  @param AOrbit Une orbite quelconque
     *  @param AMarkNumber Un numéro de marque
     *  @return Le nombre de cellules marquées
     *
     *  Marque entièrement les cellules correspondant à l'orbite AOrbit dont un
     *  brin au moins est marqué.
     */
    int markIncidentCells(TOrbit AOrbit, int AMarkNumber);

    /** markCopy
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *  @param ADart Un brin de la carte
     *  @param AOrbit Une orbite quelconque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de l'orbite déterminée par ADart et AOrbit,
     *  positionne la marque AMarkNumberTo sur le même état que la marque
     *  AMarkNumberFrom.
     */
    void markCopy(int AMarkNumberFrom, int AMarkNumberTo,
		  CDart* ADart, TOrbit AOrbit);

    /** markCopy
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de la carte,
     *  positionne la marque AMarkNumberTo sur le même état que la marque
     *  AMarkNumberFrom.
     */
    void markCopy(int AMarkNumberFrom, int AMarkNumberTo);

    /** markAdd
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de la carte,
     *  positionne la marque AMarkNumberTo à vrai si la marque AMarkNumberFrom
     *  est positionnée à vrai.
     */
    void markAdd(int AMarkNumberFrom, int AMarkNumberTo);

    /** markAdd
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *  @param ADart Un brin de la carte
     *  @param AOrbit Une orbite quelconque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de l'orbite déterminée par ADart et AOrbit,
     *  positionne la marque AMarkNumberTo à vrai si la marque AMarkNumberFrom
     *  est positionnée à vrai.
     */
    void markAdd(int AMarkNumberFrom, int AMarkNumberTo,
		 CDart* ADart, TOrbit AOrbit);

    /** markAnd
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de la carte,
     *  positionne la marque AMarkNumberTo à faux si la marque AMarkNumberFrom
     *  est positionnée à faux.
     */
    void markAnd(int AMarkNumberFrom, int AMarkNumberTo);

    /** markAnd
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *  @param ADart Un brin de la carte
     *  @param AOrbit Une orbite quelconque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de l'orbite déterminée par ADart et AOrbit,
     *  positionne la marque AMarkNumberTo à faux si la marque AMarkNumberFrom
     *  est positionnée à faux.
     */
    void markAnd(int AMarkNumberFrom, int AMarkNumberTo,
		 CDart* ADart, TOrbit AOrbit);

    /** markSub
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de la carte,
     *  positionne la marque AMarkNumberTo à faux si la marque AMarkNumberFrom
     *  est positionnée à vrai.
     */
    void markSub(int AMarkNumberFrom, int AMarkNumberTo);

    /** markSub
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *  @param ADart Un brin de la carte
     *  @param AOrbit Une orbite quelconque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de l'orbite déterminée par ADart et AOrbit,
     *  positionne la marque AMarkNumberTo à faux si la marque AMarkNumberFrom
     *  est positionnée à vrai.
     */
    void markSub(int AMarkNumberFrom, int AMarkNumberTo,
		 CDart* ADart, TOrbit AOrbit);

    /** markMove
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de la carte,
     *  positionne la marque AMarkNumberTo à vrai si la marque AMarkNumberFrom
     *  est positionnée à vrai, puis positionne la marque AMarkNumberFrom à
     *  faux.
     */
    void markMove(int AMarkNumberFrom, int AMarkNumberTo);

    /** markMove
     *
     *  @param AMarkNumberFrom Un numéro de marque
     *  @param AMarkNumberTo Un numéro de marque
     *  @param ADart Un brin de la carte
     *  @param AOrbit Une orbite quelconque
     *
     *  @pre AMarkNumberFrom!=AMarkNumberTo
     *
     *  Pour chacun des brins de l'orbite déterminée par ADart et AOrbit,
     *  positionne la marque AMarkNumberTo à vrai si la marque AMarkNumberFrom
     *  est positionnée à vrai, puis positionne la marque AMarkNumberFrom à
     *  faux.
     */
    void markMove(int AMarkNumberFrom, int AMarkNumberTo,
		  CDart* ADart, TOrbit AOrbit);

    /** isWholeCellMarked
     *
     *  @param ADart Un brin quelconque de la carte
     *  @param AOrbit Une orbite quelconque
     *  @param AMarkNumber Un numéro de marque
     *  @return Un booléen
     *
     *  Teste si tous les brins de la cellule déterminée par le brin ADart et
     *  l'orbite AOrbit sont marqués avec la marque AMarkNumber.
     */
    bool isWholeCellMarked(CDart* ADart, TOrbit AOrbit, int AMarkNumber);

    /**
     * Calcule le degré d'une cellule de dimension ACellDim, c'est-à-dire le
     * nombre de cellules de dimension ACellDim+1 qui lui sont incidentes.
     *
     * @param ADart Un brin de la carte
     * @param ACellDim Une dimension
     * @return Le degré de la cellule de dimension ACellDim incidente à ADart.
     *
     * @precondition 0 <= ACellDim < 2
     */
    int degree(CDart* ADart, int ACellDim);

    /**
     * Calcule le degré local d'une cellule de dimension ACellDim, c'est-à-dire le
     * nombre de cellules de dimension ACellDim+1 qui lui sont incidentes sans tenir
     * compte du repliement possible des cellules.
     *
     * Par exemple le degré d'un sommet incident à une boucle est 1 car il y a une
     * seule arête qui lui est incidente, alors que son degré local est 2 car on
     * ne tient pas compte de Beta1.
     *
     * @param ADart Un brin de la carte
     * @param ACellDim Une dimension
     * @return Le degré local de la cellule de dimension ACellDim incidente à ADart.
     *
     * @precondition 0 <= ACellDim < 2
     */
    //int localDegree(CDart* ADart, int ACellDim);

    /** countCells
     *
     *  @param AMarkNumber Un numéro de marque
     *
     *  @param ANbi Les nombres de i-cellules
     *
     *  Compte le nombre de cellules incidentes à un brin marqué avec la marque
     *  AMarkNumber. Pour ne compter que certains types de cellules, mettre à
     *  NULL les paramètres ANb- correspondant à une dimension non voulue.
     */
    void countCells(int AMarkNumber,
		    unsigned int* ANb0, unsigned int* ANb1, unsigned int* ANb2,
		    unsigned int* ANb3, unsigned int* ANbDarts = NULL);

    /** countCells
     *
     *  @param ADart Un brin de la carte
     *  @param AOrbit Une orbite quelconque
     *
     *  @param ANbi Les nombres de i-cellules
     *
     *  Compte le nombre de cellules incidentes ou incluses dans l'orbite
     *  (ADart, AOrbit). Pour ne compter que certains types de cellules, mettre
     *  à NULL les paramètres ANb- correspondant à une dimension non voulue.
     */
    void countCells(CDart* ADart, TOrbit AOrbit,
		    unsigned int* ANb0, unsigned int* ANb1, unsigned int* ANb2,
		    unsigned int* ANb3, unsigned int* ANbDarts = NULL);

    /** getGlobalCharacteritics
     *
     *  @param ANbDarts Le nombre de brins de la carte
     *  @param ANb{Vertices,...,CC} Le nombre de cellules de la carte
     *
     *  Calcule les caractéristiques globales de la carte (nombre de brins,
     *  de cellules). Pour ne pas calculer certaines caractéristiques,
     *  mettre à NULL les paramètres correspondants.
     */
    void getGlobalCharacteristics(unsigned int* ANbDarts,
				  unsigned int* ANbVertices,
				  unsigned int* ANbEdges,
				  unsigned int* ANbFaces,
				  unsigned int* ANbCC);

    /** getFaceCharacteritics
     *
     *  @param ADart Un brin d'une face
     *
     *  @param ANbDarts Le nombre de brins de la face
     *  @param ANb{Vertices,Edges} Le nombre de cellules de la face
     *
     *  Calcule les caractéristiques d'une face dont un brin est donné
     *  (nombre de brins, de sommets et d'arêtes). Pour ne pas calculer
     *  certaines caractéristiques, mettre à NULL les paramètres correspondants.
     */
    // void getFaceCharacteristics(CDart* ADart, unsigned int* ANbDarts,
    //				unsigned int* ANbVertices, unsigned int* ANbEdges );

     /** isConnex
     *
     *  @return 'vrai' si la carte est connexe
     *
     *  Teste si la carte ne contient qu'une seule composante connexe.
     *  Si c'est le cas, la valeur 'vrai' est retournée.
     */
    bool isConnex();

    /** checkTopology
     *
     *  @return Un booléen indiquant si la topologie de la carte est cohérente
     *
     *  Vérifie l'intégrité de la carte.
     *  La carte est cohérente si beta1 est une permutation, si beta2 est une
     *  involution.
     */
    bool checkTopology();

  };
//******************************************************************************
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("map-generic.icc")
//******************************************************************************
#endif // MAP_GENERIC_HH
//******************************************************************************
