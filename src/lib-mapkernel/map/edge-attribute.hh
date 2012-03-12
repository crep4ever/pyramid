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
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#ifndef EDGE_ATTRIBUTE_HH
#define EDGE_ATTRIBUTE_HH
//******************************************************************************
#include "kernel-types.hh"
#include "inline-macro.hh"
#include <errno.h>
#include "dss.hh"

#ifdef TBB_ACTIVE
#include "tbb/spin_mutex.h"
//#include <pthread.h>
#endif // TBB_ACTIVE
//******************************************************************************

namespace Map2d
{
  /** Classe CEdgeAttribute : pour associer des attributs aux arêtes.
   *
   *  @author Guillaume Damiand
   */

  class CEdgeAttribute
  {
  public:
    /** @name Constructeurs et destructeur
     */
    //@{

    /// Constructeur par défaut de la classe CEdgeAttribute
    CEdgeAttribute();

    /// Constructeur par défaut de la classe CEdgeAttribute
    CEdgeAttribute( int, int, int, int );

    /// Constructeur par recopie
    CEdgeAttribute( const CEdgeAttribute & );

    /// Destructeur
    ~CEdgeAttribute();

    //@}

    /** @name Accesseur aux informations, en lecture et en modification.
     */

    //@{
    /// Pour initialiser la DSS la première fois.
    void initDSS( int, int, int, int );

    /** @name Fonction d'accès au mutex afin de protéger l'arête lors
     *        d'accès concurents.
     */
    //@{

#ifdef TBB_ACTIVE
    /// Un mutex pour protéger l'arête des accès concurents.
    tbb::spin_mutex FMutex;
//    pthread_mutex_t FMutex;
#endif // TBB_ACTIVE

    /// Paramètres de la droite discrète.
  public:  CDss FDss;
  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("edge-attribute.icc")
//******************************************************************************
#endif // EDGE_ATTRIBUTE_HH
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
