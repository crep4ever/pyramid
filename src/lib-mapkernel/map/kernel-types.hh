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
#ifndef KERNEL_TYPES_HH
#define KERNEL_TYPES_HH
//******************************************************************************
#include <bitset>
#include <string>
#include "alt-stdint.hh"
//******************************************************************************
/** Définition des types de base du noyau (Orbites, Involutions et Masques)
 *
 * @author Guillaume Damiand, Frédéric Vidil
 */

//******************************************************************************
//@{
/** NB_MARKS
 *
 *  Nombre de marques associées aux brins.
 */
#define NB_MARKS (16)

/** NB_DIRECT_INFO
 *
 *  Nombre de champs direct-info associés aux brins.
 */
#define NB_DIRECT_INFO (2)

//@}

//******************************************************************************
//@name Orbites
//@{
/// Définition du type TOrbit :
typedef int8_t TOrbit;

//@name Orbites de base :
//@{
enum
{
  ORBIT_NONE = -1,
  ORBIT_SELF,
  ORBIT_0,
  ORBIT_1,      // Face  
  ORBIT_2,      // Arête
  ORBIT_VERTEX, // Sommet
  ORBIT_12   	// CC
};
//@}
//******************************************************************************
//@name Types d'attributs
//@{
/// Définition du type TAttributeId :
typedef int TAttributeId;

// Les différents identifiants des attributs sont définit dans le fichier :
// attributes-id.hh
//@}
//******************************************************************************
//@name Orbites ayant une sémantique géométrique.
//@{
/// Orbite arête.
const TOrbit ORBIT_EDGE   = ORBIT_2;
/// Orbite face.
const TOrbit ORBIT_FACE   = ORBIT_1;
/// Orbite composante connexe.
const TOrbit ORBIT_CC     = ORBIT_12;
//@}
//****************************************************************************
//@name Ensembles d'orbites particuliers.
//@{

/// Orbite cellule de dimension l'indice du tableau.
const TOrbit ORBIT_CELL[4] =
{
  ORBIT_VERTEX, ORBIT_EDGE, ORBIT_FACE, ORBIT_CC
};

//@}
//******************************************************************************
//@name Opérations (interne au fonctionnement des parcours).

//@{
/** Ce type permet, lors d'un parcours, de connaître quel était le type de
 *  l'opération précédente
 */
typedef int8_t TOperationState;

enum
{
  /// Le parcours débute et il n'y a pas d'opération précédente (operator++).
  OP_NONE = -1,

  /// L'opération précédente était un operator++ de type beta0.
  OP_BETA0,
  /// L'opération précédente était un operator++ de type beta1.
  OP_BETA1,
  /// L'opération précédente était un operator++ de type beta2.
  OP_BETA2,
 
  /// L'opération précédente était un operator++ de type beta02.
  OP_BETA02,
 
  /// L'opération précédente était un operator++ de type beta21.
  OP_BETA21,
 
  /// Pour les templates de parcours, les beta relatifs utilisés sont betai
  /// pour le premier,
  OP_BETAI,
  /// betaj pour le second.
  OP_BETAJ,
  
  /// Opération de saut (pour les parcours qui utilisent une pile)
  OP_JUMP,
  /// End of op ???....
  OP_END,

  // Lorsqu'on parcourt une liste de brins (static et multidynamic coverages).
  OP_UNKNOWN
};
//@}
//******************************************************************************
//@name Types pour les régions
//@{
/// Définition du type TRegionId :
typedef uint32_t TRegionId;
//@}

//******************************************************************************
//@name Types pour les brins
//@{
/// Définition du type TDartIdId :
typedef uint32_t TDartId;
//@}


//****************************************************************************
#endif // KERNEL_TYPES_HH
//******************************************************************************
