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
     * Compte le nombre de fois que la carte *this est un motif de la carte AMap.
     * AMarkToTest est un numéro de marque, seul les brins de *this marqués
     * sont testés.
     * @param ANbMatched si !=NULL, parametre retour qui compte le
     *        plus grand nombre de brins avant de sortir.
     * @param AMirror vrai pour tester l'isomorphisme mirroir
     * @return le nombre de fois que le motif est trouvé.
     */
    unsigned int countNumberOfMotifs(unsigned int AMarkToTest, 
				     CTopologicalMap* AMap,
				     unsigned int* ANbMatched, 
				     bool AMirror = false);

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
		       unsigned int AMarkToTest2,
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

