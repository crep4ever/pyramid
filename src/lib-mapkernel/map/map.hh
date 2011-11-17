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
#ifndef MAP_HH
#define MAP_HH
//******************************************************************************
#include "map-basic.hh"
#include "inline-macro.hh"
// Includes facultatifs :
#include "dart.hh"
#include "coverage.hh"
#include "dynamic-coverage.hh"
#include "static-coverage.hh"
//******************************************************************************
/** @author Guillaume Damiand, Frédéric Vidil
 */

/** La classe CMap est la première surcouche de CMapBasic. Elle définit les
 *  méthodes ayant besoin de parcours :
 *  - les coutures et décousures (topologiques et géométrique)
 */

//******************************************************************************
namespace Map2d
{
  class CMap : public CMapBasic
  {
  public:
    /** @name Constructeur et destructeur
     */

    //@{

    /** Constructeur
     */
    CMap(unsigned int ASizeDartArray = 1000);
  
    /** Destructeur
     */
    virtual ~CMap();

    //@}

  
    /**  @name Méthodes "sûres" pour la couture et décousure.
     */

    //@{
  
    /** canSew[i]
     *
     *  @param ADart1 et ADart2 Deux brins
     *  @return vrai si la [i]-couture de ces deux brins est possible,
     *          faux sinon.
     */
    bool canSew0(CDart* ADart1, CDart* ADart2);
    bool canSew1(CDart* ADart1, CDart* ADart2);
    bool canSew2(CDart* ADart1, CDart* ADart2);
   
    /** canSew
     *
     *  @param ADart1 et ADart2 Deux brins
     *  @param ADimension Une dimension
     *  @return si la [ADimension]-couture de ces deux brins est possible.
     */  
    bool canSew(CDart* ADart1, CDart* ADart2, int ADimension);

    /** sew[i]
     *
     *  @param ADart1 et ADart2 Deux brins
     *  @pre canSew[i](ADart1, ADart2)
     *
     *  [i]-couture géométrique des 2 brins. Les attributs sont mis à jour
     *  lorsque c'est nécessaire (quand deux orbites sont fusionnées et que
     *  chacune avait un attribut). De plus, la couture est effectuée sur
     *  l'orbite concernée afin de garantir la validité de la carte obtenue.
     */
    void sew0(CDart* ADart1, CDart* ADart2);
    void sew1(CDart* ADart1, CDart* ADart2);
    void sew2(CDart* ADart1, CDart* ADart2);
   
  
    /** sew
     *
     *  @param ADart1 et ADart2 Deux brins
     *  @param ADimension Une dimension
     *  @pre canSew(ADart1, ADart2, ADimension)
     *
     *  Couture géométrique des deux brins pour la dimension donnée.
     *  (cf. sew[i])
     */
    void sew(CDart* ADart1, CDart* ADart2, int ADimension);

    /** unsew[i]
     *
     *  @param ADart Un brin
     *
     *  [i]-décousure géométrique. Les attributs sont mis à jour lorsque c'est
     *  nécessaire (quand on coupe une orbites en deux). De plus, la décousure
     *  est effectuée sur l'orbite concernée afin de garantir la validité
     *  de la carte obtenue.
     */
    void unsew0(CDart* ADart);
    void unsew1(CDart* ADart);
    void unsew2(CDart* ADart);
   
    /** unsew
     *
     *  @param ADart Un brin
     *  @param ADimension Une dimension
     *
     *  Décousure géométrique du brin pour la dimension donnée. (cf. unsew[i])
     */  
    void unsew (CDart* ADart, int ADimension);

    //@}

  
    /**  @name Méthodes pour récupérer des parcours étant donné un brin et une
     *         orbite. Voir les différents types de parcours dans
     *         dynamic-coverage.hh et static-coverage.hh.
     */
    //@{
    CCoverage* getDynamicCoverage(const CDart* ADart, TOrbit AOrbit);
    CCoverage* getStaticCoverage (const CDart* ADart, TOrbit AOrbit);

    /** Le getBasicDynamicCoverage donne pour les 2 parcours utilisant des
     *  marques (cas ORBIT_12, ORBIT_VERTEX) le parcours basic.
     */
    CCoverage* getBasicDynamicCoverage(const CDart* ADart, TOrbit AOrbit,
				       int AMark, int ADirectIndex = -1);
    //@}  
  
    /** isSameOrbit
     *
     *  @param ADart1 et ADart2 Deux brins
     *  @param AOrbit Une orbite 
     *
     *  @return vrai si ADart1 et ADart2 appartiennent à la même orbite <AOrbit>
     *          faux sinon
     */
    bool isSameOrbit(CDart* ADart1, CDart* ADart2, TOrbit AOrbit);  

    /**  @name Méthodes "non-sûres" pour la coutures. Le résultat de ces
     *   méthodes entraîne l'obtention d'un objet éventuellement non valide
     *   (une 2-carte ne vérifiant pas les contraintes sur les beta, ou
     *   plusieurs attributs sur une même orbite...). La responsabilité de la
     *   validité est laissé à l'appelant. Ces méthodes servent malgré tout pour
     *   optimiser des traitements particulier. A UTILISER AVEC ATTENTION.
     */

    /// @name Les méthodes de coutures / décousures
    //@{
    /** dartSew[i]
     *
     *  @param ADart1 et ADart2 Deux brins
     *
     *  [i]-couture géométrique des 2 brins uniquement (et pas de toute l'orbite
     *  contrairement à sew[i]). Les attributs sont mis à jour lorsque c'est
     *  nécessaire (quand deux orbites sont fusionnées et que
     *  chacune avait un attribut). 
     */
    void dartSew0(CDart*, CDart* ADart2);
    void dartSew1(CDart*, CDart* ADart2);
    void dartSew2(CDart*, CDart* ADart2);
   
    void dartSew (CDart*, CDart* ADart2, int ADimension);

    /** unsew[i]
     *
     *  @param ADart Un brin
     *
     *  [i]-décousure géométrique uniquement du brin (et pas de toute l'orbite
     *  contrairement à unsew[i]). Les attributs sont mis à jour lorsque c'est
     *  nécessaire (quand on coupe une orbites en deux). 
     */
    void dartUnsew0(CDart* ADart);
    void dartUnsew1(CDart* ADart);
    void dartUnsew2(CDart* ADart);
   
    void dartUnsew (CDart* ADart, int ADimension);


    /** topoSew[i]
     *
     *  @param ADart1 et ADart2 Deux brins
     *  @pre canSew[i](ADart1, ADart2)
     *
     *  [i]-couture topologique des 2 brins (les attributs ne sont pas mis à
     *  jour contrairement à sew[i]). La couture est effectuée sur l'orbite
     *  concernée afin de garantir la validité de la carte obtenue.
     */
    void topoSew0(CDart* ADart1, CDart* ADart2);
    void topoSew1(CDart* ADart1, CDart* ADart2);
    void topoSew2(CDart* ADart1, CDart* ADart2);

    void topoSew (CDart* ADart1, CDart* ADart2, int ADimension);

    /** topoUnsew[i]
     *
     *  @param ADart Un brin
     *
     *  [i]-décousure topologique (les attributs ne sont pas mis à jour
     *  contrairement à unsew[i]). La décousure est effectuée sur l'orbite
     *  concernée afin de garantir la validité de la carte obtenue.
     */
    void topoUnsew0(CDart* ADart);
    void topoUnsew1(CDart* ADart);
    void topoUnsew2(CDart* ADart);
    
    void topoUnsew (CDart* ADart, int ADimension);
    
    //@}
  };
  
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("map.icc")
//******************************************************************************
#endif // MAP_HH
//******************************************************************************
