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
#ifndef MAP_BASIC_HH
#define MAP_BASIC_HH
//******************************************************************************
#include <bitset>
#include <vector>
#include "kernel-types.hh"
#include "inline-macro.hh"
//******************************************************************************
/** @author Guillaume Damiand, Frédéric Vidil
 */

namespace Map2d
{
  class CDart;

  /** La classe CMapBasic regroupe uniquement des méthodes de base :
   *  - des méthodes de gestion des marques booléennes,
   *  - des méthodes de gestion des champs direct-info,
   *  - des méthodes pour lier/délier deux brins entre eux, sans vérification
   *    des contraintes d'intégrité.
   *
   *  Les méthodes permettant d'explorer la carte sont définies dans
   *  la classe CMap.
   */

  class CMapBasic
  {
    friend class CDynamicCoverageAll;

  public:
    /// @name Constructeur et destructeur
    //@{

    /// Constructeur
    CMapBasic(unsigned int ASizeDartArray = 1000);

    /// Destructeur
    virtual ~CMapBasic();

    //@}

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

    /// Initialise un brin donné (nécessaire car on n'utilise plus le
    /// constructeur des brins avec paramètres).
    void initDart(CDart* ADart);

    /**
     *  Crée une nouvelle instance de CDart.
     *  @return Un nouveau brin de type CDart
     */
    virtual CDart* addMapDart();

    /// Copie le contenu du brin ADartSrc dans le brin ADartDest
    virtual void copyDart(CDart* ADartDest, CDart* ADartSrc);

    /**
     * "Supprime" le brin passé en paramètre (de type CTopologicalDart)
     * Le brin n'est supprimé mais marqué supprimé et peut désormais être
     * ré-utilisé lors d'un addMapDart. ATTENTION: modifie le champ beta0
     * de ADart.
     *
     *  @param ADart Le brin à supprimer
     */
    virtual void delMapDart(CDart* ADart);

   /**
    * @retur vrai ssi ADart est marqué supprimé.
    */
   bool isMarkedDeleted(CDart* ADart) const;

    /**
     *  Méthode permettant de vider la carte.
     *  Tous les brins sont détruits.
     */
    void removeAllDarts();

    /**
     *  Méthode permettant de réinitialiser la carte.
     *  La méthode removeAllDarts est appelée, puis toutes les marques, champs
     *  direc-info, etc, sont réinitialisés.
     */
    virtual void empty();

    /// @return Le nombre de brins présent dans la carte.
    unsigned int getNbDarts() const;

    /// @return Le nombre de brins dans les tableaux
    unsigned int getNbDartsInTabs() const;

    /// @return La taille de chaque tableau de brins.
    unsigned int getSizeDartArray() const;

    /// @return Le nombre de tableaux de brins.
    unsigned int getNbTabsDarts() const;

    /// @return  Le nombre de brins dans le dernier tableau.
    unsigned int getNbDartsInLastTab() const;

    /// @return La marque des brins à détruire.
    int getMarkToDelete() const;

    /// @name Gestion des champs direct-info
    //@{

    /**
     *  Permet de réserver un champ direct info libre.
     *
     *  @return L'indice du champ direct-info réservé.
     */
    int  getNewDirectInfo();

    /**
     *  Permet de libérer un champ direct-info préalablement réservé avec la
     *  méthode getNewDirectInfo.
     *
     *  @param ADirectIndex Un indice de champ direct-info.
     */
    void freeDirectInfo(int ADirectIndex);

    /**
     *  Retourne la valeur du champ direct-info[ADirectIndex] du brin ADart.
     *
     *  @param ADart Un brin de la carte
     *  @param ADirectIndex Un indice de champ direct-info
     *  @return Un pointeur
     */
    void* getDirectInfo(CDart* ADart, int ADirectIndex) const;

    /**
     *  Affecte la valeur APointer au champ direct-info[ADirectIndex] du brin
     *  ADart.
     *
     *  @param ADart Un brin de la carte
     *  @param ADirectIndex Un indice de champ direct-info
     *  @param APointer La valeur à affecter
     */
    void setDirectInfo(CDart* ADart, int ADirectIndex, void* APointer);

    //@}

    /// @name Gestion des marques
    //@{

    /**
     *  Réserve une marque inutilisée de la carte.
     *
     *  @return Un numéro de marque
     */
    int  getNewMark();

    /**
     *  Retourne l'état de la marque.
     *  Initialement, cette valeur vaut false. Elle est modifiée à chaque appel
     *  à la méthode negateMaskMark().
     *
     *  @param AMarkNumber Un numéro de marque
     *  @return Un booléen indiquant l'état de la marque
     */
    bool getMaskMark(int AMarkNumber) const;

    /**
     *  Libère la marque AMarkNumber préalablement réservée avec la méthode
     *  getNewMark().
     *
     *  @param AMarkNumber
     */
    void freeMark(int AMarkNumber);

    /**
     *  Inverse l'état d'une marque. Cette méthode permet de changer l'état de
     *  la marque AMarkNumber de tous les brins de la carte en O(1).
     *  C'est utile par exemple pour démarquer tous les brins à la fin d'un
     *  traitement.
     *
     *  @param AMarkNumber Un numéro de marque
     */
    void negateMaskMark(int AMarkNumber);

    /**
     *  Cette méthode est utilisée (en assertion) par la méthode freeMark.
     *  En effet, lorqu'on libère une marque, il faut nécessairement que tous
     *  les brins de la carte soient démarqués.
     *
     *  @param AMarkNumber Un numéro de marque
     *  @return Un booléen indiquant si tous les brins de la carte sont
     *          démarqués
     */
    bool isWholeMapUnmarked(int AMarkNumber);

    /**
     *  Cette méthode permet de savoir à tout moment combien de marques sont
     *  utilisées simultanément.
     *  Sert au debugging.
     *
     *  @return Le nombre de marques utilisées
     */
    int getNbUsedMarks() const;

    /**
     *  Teste si la marque AMarkNumber du brin ADart est activée.
     *  Cette méthode est redirigée sur la méthode équivalente de CDart.
     *
     *  @param ADart Un brin de la carte
     *  @param AMarkNumber Un numéro de marque
     *  @return Un booléen indiquant l'état de la marque AMarkNumber du brin
     *          ADart.
     */
    bool isMarked(const CDart* ADart, int AMarkNumber) const;

    /**
     *  Permet de modifier l'état de la marque AMarkNumber du brin ADart.
     *  Cette méthode est redirigée sur la méthode équivalente de CDart.
     *
     *  @param ADart Un brin de la carte
     *  @param AMarkNumber Un numéro de marque
     *  @param AState Un booléen indiquant s'il faut activer ou désactiver la
     *         marque
     */
    void setMarkTo(CDart* ADart, int AMarkNumber, bool AState);

    /**
     *  Permet d'activer la marque AMarkNumber du brin ADart.
     *  Cette méthode équivaut à setMarkTo(ADart, AMarkNumber, true).
     *
     *  @param ADart Un brin de la carte
     *  @param AMarkNumber Un numéro de marque
     */
    void setMark(CDart* ADart, int AMarkNumber);

    /**
     *  Permet de désactiver la marque AMarkNumber du brin ADart.
     *  Cette méthode équivaut à setMarkTo(ADart, AMarkNumber, false).
     *
     *  @param ADart Un brin de la carte
     *  @param AMarkNumber Un numéro de marque
     */
    void unsetMark(CDart* ADart, int AMarkNumber);

    /**
     *  Cette méthode permet d'initialiser toutes les marques du brin ADart.
     *  AMarks est modifié pour prendre en comte le masque des marques.
     *
     *  @param ADart Un brin de la carte
     *  @param AMarks un tableau indiquant l'état de toutes les marques
     */
    void setMarks(CDart* ADart, const std::bitset<NB_MARKS> & AMarks) const;

    /**
     *  Retourne l'état des marques du brin, en tenant compte du masque des
     *  marques.
     *
     *  @param ADart Un brin de la carte
     *  @return L'état des marques du brin
     */
    std::bitset<NB_MARKS> getMarks(CDart* ADart) const;

    //@}

    /** @name Accès aux involutions
     *
     *  Attention, les méthodes d'accès en écritures qui suivent ne sont pas
     *  sure en ce sens qu'elles ne vérifient pas forcément les contraintes des
     *  cartes en sortie, et qu'elles ne mettent pas à jour les eventuels
     *  plongement.
     */
    //@{

    /** isFree[i]
     *
     *  @param ADart Un brin de la carte
     *
     *  Teste si le brin ADart est libre par beta-i, c'est-à-dire s'il est sa
     *  propre image par beta-i.
     */
    bool isFree0(const CDart* ADart) const;
    bool isFree1(const CDart* ADart) const;
    bool isFree2(const CDart* ADart) const;

    /** isFree
     *
     *  @param ADart Un brin de la carte
     *  @param ADimension Une dimension comprise entre 0 et 2 inclus
     *
     *  Teste si le brin ADart est libre en dimension ADimension, c'est-à-dire
     *  s'il est sa propre image par beta-ADimension.
     */
    bool isFree(const CDart* ADart, int ADimension) const;

    /** beta[i]
     *
     *  @param ADart Un brin de la carte
     *  @return L'image par beta-i de ADart
     */

    CDart* beta0(const CDart* ADart) const;
    CDart* beta1(const CDart* ADart) const;
    CDart* beta2(const CDart* ADart) const;

    /** beta[i]
     *
     *  @param ADart Un brin de la carte
     *  @param ADimension Une dimension comprise entre 0 et 2 inclus
     *  @return l'image par beta-ADimension de ADart.
     */
    CDart* beta(const CDart* ADart, int ADimension) const;

    /** linkBeta[i]
     *
     *  @param ADart1 Un brin de la carte
     *  @param ADart2 Un brin de la carte
     *
     *  Lie par beta-[i] les brins ADart1 et ADart2.
     */
    void linkBeta0(CDart* ADart1, CDart* ADart2);
    void linkBeta1(CDart* ADart1, CDart* ADart2);
    void linkBeta2(CDart* ADart1, CDart* ADart2);

    /** linkBeta
     *
     *  @param ADart1 Un brin de la carte
     *  @param ADart2 Un brin de la carte
     *  @param ADimension Une dimension entre 0 et 2 inclus.
     *
     *  Lie par beta-ADimension les brins ADart1 et ADart2.
     */
    void linkBeta(CDart* ADart1, CDart* ADart2, int ADimension);

    /** unlinkBeta[i]
     *
     *  @param ADart Un brin de la carte
     *
     *  Délie par beta-[i] les brins ADart et beta-[i](ADart).
     */
    void unlinkBeta0(CDart* ADart);
    void unlinkBeta1(CDart* ADart);
    void unlinkBeta2(CDart* ADart);

    /** unlinkBeta
     *
     *  @param ADart1 Un brin de la carte
     *  @param ADart2 Un brin de la carte
     *  @param ADimension Une dimension entre 0 et 2 inclus.
     *
     *  Délie par beta-ADimension les brins ADart et beta-ADimension(ADart2).
     */
    void unlinkBeta(CDart* ADart, int ADimension);

    //@}

  protected:
    /// Tableau indiquant quelles sont les marques prises
    std::bitset<NB_MARKS> FUsedMarks;

    /// Masque des marques (modifié par la méthode negateMaskMark())
    std::bitset<NB_MARKS> FMaskMarks;

    /// Liste des marques libres
    int FMarks[NB_MARKS];

    /// Tableau indiquant quels sont les champs direct-info utilisés
    std::bitset<NB_DIRECT_INFO> FUsedDirectInfo;

    /// Liste des champs directInfo libres
    int FDirectInfos[NB_DIRECT_INFO];

    /// Nombre de marques utilisées
    int FNbUsedMarks;

    /// Nombre de champs directInfo utilisés
    int FNbUsedDirectInfos;

#ifndef NDEBUG
    /// Nombre maximum de marques utilisées simultanément
    int FMaxNbUsedMarks;

    /// Nombre maximum de champs directInfo utilisés simultanément
    int FMaxNbUsedDirectInfos;
    bool FMsgDirectInfo; // Pour afficher le message une seule fois.
#endif // NDEBUG

    /// Compteurs pour le nombre de brins
    unsigned int FNbDarts;

    /// Stockage des brins dans un tableau de tableau (et plus dans une liste)
    /// Le tableau de tableau de brins.
    std::vector<CDart*> FTabsDarts;

    /// Le nombre de tableau de brins.
    unsigned int FNbTabsDarts;

    /// Le nombre de brins dans le dernier tableau.
    unsigned int FNbDartsInLastTab;

    /// La taille de chaque tableau de brins.
    unsigned int FSizeDartArray;

    /// Une marque pour les brins à détuire. Le principe est de ne jamais
    /// supprimer les brins directement mais simplement de les marquer.
    /// La liste des brins étant nettoyé à chaque fois que l'on fait un
    /// CDynamicCoverageAll (un peu le principe du garbabe collector...).
    /// Les brins supprimés sont chainés entre eux à partir de FFirstDeleteDart
    /// en utilisant le champ beta[0] comme pointeur next.
    int FMarkToDelete;
    CDart* FFirstDeleteDart;
  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("map-basic.icc")
//******************************************************************************
#endif // MAP_BASIC_HH
//******************************************************************************
