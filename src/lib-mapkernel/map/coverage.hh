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
// Generic coverage class following different orbits in maps.
//******************************************************************************
#ifndef COVERAGE_HH
#define COVERAGE_HH
//******************************************************************************
#include "kernel-types.hh"
#include "inline-macro.hh"
//******************************************************************************
/** Classe CCoverage : Parcours (CCoverage) (Class Abstraite)
 *  Cette classe définit un itérateur sur l'ensemble des brins d'une orbite
 *  donnée. Les classes CStaticCoverage et CDynamicCoverage implémentent cette
 *  interface.
 *
 *  @author Guillaume Damiand, Frédéric Vidil
 */

#define COVERAGE_SELF          	(0)
#define COVERAGE1              	(1)   /*=COVERAGE_BETA2 en 2d, variable non changée*/
#define COVERAGE_BETA0         	(2)
#define COVERAGE_BETA1         	(3)
#define BASIC_COVERAGE_BETA12  	(4)
#define COVERAGE_BETA12        	(5)
#define BASIC_COVERAGE_VERTEX  	(6)
#define COVERAGE_VERTEX        	(7)
#define COVERAGE_ALL            (8)

namespace Map2d
{
  class CDart;
  class CMapBasic;

  class CCoverage
  {
  public:
    //@name Constructeur et destructeur.

    //@{
    /// Constructeur.
    CCoverage(CMapBasic*);

    /// Destructeur virtuel.
    virtual ~CCoverage();

    //@}

    /// Accesseur à l'objet Carte.
    CMapBasic* getMap() const;

    //@name Méthodes virtuelles pures.
    //@{

    /// Réinitialisation du parcours.
    virtual void reinit() = 0;

    /// Itérateur préfixe.
    virtual void operator++() = 0;

    /// Itérateur postfixe
    virtual CDart* operator++(int) = 0;

    /// Drapeau d'arrêt du parcours.
    virtual bool cont() = 0;

    /// Déreférencement du pointeur de parcours en pointeur de brin.
    virtual CDart* operator*() = 0;

    /// Type du dernier appel à ++.
    virtual TOperationState prevOperationType();

    /// Identificateur du type de parcours.
    virtual int type() const = 0;

  protected:
    /// La Carte sur laquelle s'effectue le parcours.
    CMapBasic* FMap;
  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("coverage.icc")
//******************************************************************************
#endif // COVERAGE_HH
//******************************************************************************
