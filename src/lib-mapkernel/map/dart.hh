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
#ifndef DART_HH
#define DART_HH
//******************************************************************************
#include <bitset>
#include <ostream>
#include "kernel-types.hh"
#include "edge-attribute.hh"
#include "inline-macro.hh"
//******************************************************************************

namespace Map2d
{
  /** Classe CDart : base pour un brin d'une 2-Carte.
   *
   *  Cette classe contient entre autres les fonctions de manipulation bas
   *  niveau des betas.
   *  Ses méthodes ne doivent pas être directement appelées par un utilisateur
   *  lambda : la manipulation des brins se fait toujours à l'aide de la classe
   *  de la carte (CMapBasic/CMap).
   *
   *  @author Guillaume Damiand, Frédéric Vidil
   */

  class CDart
  {
  public:
    /** @name Constructeurs et destructeur
     */
    //@{

    /** Constructeur par défaut de la classe CDart
     */
    CDart();

    /// Initialise le brin comme s'il venait d'être construit.
    void init(const std::bitset<NB_MARKS> & AMarks);

    /// Copy les informations du brin ADart
    void copy( CDart* ADart );
    
    /** Destructeur
     *
     *  Détruit une instance de la classe CDart.
     */
    virtual ~CDart();

    //@}

    /** @name Utilisation des informations directes (champs direct-info)
     *
     *  Une information directe est un pointeur vers une structure de données
     *  quelconque. Le nombre d'informations par brin est fixé par la macro
     *  NB_DIRECT_INFO (voir fichier kernel-types.hh).
     */

    //@{

    /** getDirectInfo
     *
     *  @param AIndice Le numéro de l'information directe à récupérer.
     *  @return Un pointeur sur l'information recherchée.
     *
     *  @pre 0 <= AIndice < NB_DIRECT_INFO
     */
    void* getDirectInfo(int ADirectIndex) const;

    /** setDirectInfo
     *
     *  @param AIndice Le numéro de champ à modifier
     *  @param APointer La valeur à affecter
     *
     *  @pre 0 <= AIndice < NB_DIRECT_INFO
     */
    void setDirectInfo(int ADirectIndex, void* AValue);

    //@}

    /** @name Accès aux involutions
     */

    //@{

    /** Accesseur de l'involution beta0
     *
     *  @return le brin cousu par beta0.
     */
    CDart* getBeta0() const;

    /** Accesseur de l'involution beta1
     *
     *  @return le brin cousu par beta1.
     */
    CDart* getBeta1() const;

    /** Accesseur de l'involution beta2
     *
     *  @return le brin cousu par beta2.
     */
    CDart* getBeta2() const;


    /** Accesseur de l'involution betaI
     *
     *  @param ADimension le numéro de la liaison à regarder (entre 0 et 2).
     *  @return le brin cousu par betaI.
     */
    CDart* getBeta(int AIndex) const;

    /** Définition de l'involution beta0
     *
     *  @param le brin à lier par beta0 à l'instance.
     */
    void setBeta0(CDart* ADart);

    /** Définition de l'involution beta1
     *
     *  @param D le brin à lier par beta1 à l'instance.
     */
    void setBeta1(CDart* ADart);

    /** Définition de l'involution beta2
     *
     *  @param D le brin à lier par beta2 à l'instance.
     */
    void setBeta2(CDart* ADart);

 
    /** Définition de l'involution betaI
     *
     *  @param ADart le brin à lier par betaI à l'instance.
     *  @param ADimension le dimension de la liaison beta à considérer.
     */
    void setBeta (CDart* ADart, int AIndex);

 
  
 /** Test permettant de savoir si un brin est libre (non cousu) par beta0.
     *
     *  @return un booléen signifiant si l'instance est libre par beta0.
     */
    bool isFree0() const;

    /** Test permettant de savoir si un brin est libre (non cousu) par beta1.
     *
     *  @return un booléen signifiant si le brin instancié est libre par beta1.
     */
    bool isFree1() const;

    /** Test permettant de savoir si un brin est libre (non cousu) par beta2.
     *
     *  @return un booléen signifiant si le brin instancié est libre par beta2.
     */
    bool isFree2() const;

   
    /** Test permettant de savoir si un brin est libre (non cousu) par betaI.
     *
     *  @param ADimension la dimension de la liaison à tester.
     *  @return un booléen signifiant si l'instance est libre par beta3.
     */
    bool isFree (int AIndex) const;

    /** Libèration de l'involution beta0 de l'instance.
     */
    void setFree0();

    /** Libèration de l'involution beta1 de l'instance.
     */
    void setFree1();

    /** Libèration de l'involution beta2 de l'instance.
     */
    void setFree2();

   
    /** Libèration pour le brin instancié de son involution betaI
     */
    void setFree(int AIndex);

    //@}

    /** @name Gestion des marques
     */
    //@{

    /** Test permettant de savoir si un brin est marqué.
     *
     *  @param i Le numéro de la marque à tester (entre 0 et NB_MARKS-1).
     *  @return booléen signifiant si le brin est marqué.
     *  @pre 0 <= i < NB_MARKS
     */
    bool getMark(int AMarkNumber) const;

    /** Méthode définissant si un brin est marqué ou non.
     *
     *  @param i Le numéro de la marque à changer (entre 0 et NB_MARKS-1).
     *  @param value La valeur booléenne de la marque.
     *  @pre 0 <= i < NB_MARKS
     */
    void setMark(int AMarkNumber, bool AValue);

    /** Méthode d'accès à l'ensemble des marques d'un brin.
     *
     *  @return l'ensemble de marques (de type bitset)
     */
    std::bitset<NB_MARKS> getMarks() const;

    /** Méthode définissant en une seule fois l'ensemble des marques d'un brin.
     *
     *  @param marks l'ensemble des valeurs à prendre par les marques du brin
     */
    void setMarks(const std::bitset<NB_MARKS> & AMarks);

#ifdef CONTOUR_SIMPLIFICATION
    /**
     * Pour récupérer l'attribut de l'arête incidente au brin this.
     * Cet attribut peut-être soit sur this, soit sur beta2(this).
     *
     * @return un pointeur vers l'attribut retrouvé
     *        (NULL si l'arête n'a pas d'attribut)
     */
    CEdgeAttribute* getEdgeAttribute() const;

    /**
     * Pour fixer l'attribut de l'arête incidente à this.
     *
     * @pre L'arête incidente à this n'a pas d'attribut arête.
     * @param AAttribute un pointeur sur l'attribut à affecter
     */
    void setEdgeAttribute( CEdgeAttribute* AAttribute );

    /**
     * Enlève l'attribut de l'arête incidente à this.
     * Ne fait rien et retourne NULL si cette arête n'avait pas d'attribut.
     * @return un pointeur vers l'attribut enlevé
     *        (NULL si l'arête n'avait pas d'attribut)
     */
    CEdgeAttribute* removeEdgeAttribute();

    /**
     *  Oriente les attributs d'arêtes afin que les deux brins this et ADart
     *  soient tous deux porteur de l"attributs.
     */
    void orientEdgesAttribute( CDart* ADart );
    
#endif // CONTOUR_SIMPLIFICATION

    /// Affichage d'un brin.
    void print( std::ostream& AOs ) const;    
    friend std::ostream& operator<<(std::ostream&, const CDart &);
    
  private:
    /// Tableau des liaisons beta (suite de pointeurs sur un brin)
    CDart* FBeta[3];

    /// Valeur des marques booléennes du brin
    std::bitset<NB_MARKS> FMarks;

    /// Tableau des informations directes
    void* FDirectInfo[NB_DIRECT_INFO];

    /// Pointeur vers un attribut sur les arêtes
#ifdef CONTOUR_SIMPLIFICATION
    public: CEdgeAttribute* FEdgeAttribute;
#endif // CONTOUR_SIMPLIFICATION
    
  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("dart.icc")
//******************************************************************************
#endif // DART_HH
//******************************************************************************
