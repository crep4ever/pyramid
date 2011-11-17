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
/** Vérifie que le pixel induit par le double est simple pour le label.
 *
 * @param ALinel doublet désignant un pixel.
 * @param ALabel label par rapport auquel nous vérifions la simplicité.
 * @return true si le pixel est simple et false sinon.
 *
 * @pre le lignel sépare le pixel courant de la région vers laquelle nous
 * souhaitons le basculer.
 */
bool deformationIsSimple( const CDoublet & ALinel ) const;

/** Bascule le pixel et modifie le plongement de l'arête déformée
 *
 * @param ADart brin de l'arête déformée
 * @param ALinel doublet désignant un pixel.
 *
 * @pre le pixel désigné est IM-Simple
 * @pre l'arête sépare la région du pixel et la région ALabel
 */
void deformationApply( CDart * ADart, const CDoublet & ALinel );

/** Vérification de la propriété 1 pour un pixel.
 *
 * Le degré de chaque pointel est égal à 0, 2 ou 3.
 *
 * @param ALinel doublet désignant un pixel.
 * @param AMaxDegree valeur maximum du degré des pointels.
 * @return true si la propriété est vraie et false sinon.
 */
bool deformationCheckPointelDegree( const CDoublet & ALinel,
				    uint32_t * AMaxDegree ) const;

/** Vérification de la propriété 2 pour un pixel.
 *
 * L'ensemble des lignels frontière lf( x ) est homéomorphe à un
 * 1-disque.
 *
 * @param ALinel doublet désignant un pixel.
 * @return true si la propriété est vraie et false sinon.
 */
bool deformationCheckLinelBody( const CDoublet & ALinel ) const;

/** Vérification de la propriété 3 pour un pixel.
 *
 * Si le degré interne du pointel est 0 alors le degré externe est 0.
 *
 * @param ALinel doublet désignant un pixel.
 * @return true si la propriété est vraie et false sinon.
 */
bool deformationCheckPointelDegreeExt( const CDoublet & ALinel ) const;

/** Vérification de la propriété 4 pour un pixel.
 *
 * Si le pointel n'est pas incident au chemin clf( ALinel ) alors son
 * degré doit être inférieur à 2.
 *
 * @param ALinel doublet désignant un pixel.
 * @param ALabel label dans lequel nous basculons le pixel.
 * @return true si la propriété est vraie et false sinon.
 *
 * @pre le lignel sépare le pixel courant de la région vers laquelle nous
 * souhaitons le basculer.
 */
bool deformationCheckPointelDegreeOther( const CDoublet & ALinel ) const;
