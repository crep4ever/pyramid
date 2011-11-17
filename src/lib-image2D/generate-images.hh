/*
 * libImage2D : Librairie de manipulation d'une image 2D.
 * Copyright (C) 2008, Guillaume Damiand, guillaume.damiand@liris.cnrs.fr
 *               http://liris.cnrs.fr
 *
 * This file is part of libImage2D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//******************************************************************************
#ifndef GENERATE_IMAGES_HH
#define GENERATE_IMAGES_HH
//******************************************************************************
#include "image2D.hh"
//******************************************************************************

/// Génère une image carré
/// Tout les pixels d'un même carréont la même étiquette, chaque carré
/// (excepté éventuellement ceux touchant les bords) sont de taille AWidth.
void generateSquares( CImage2D& AImage, unsigned int AWidth );

/// Génère une image ligne
/// Tout les pixels d'une même ligne ont la même étiquette, chaque ligne 
/// (excepté éventuellement la dernière) est de hauteur AWidth.
void generateLines( CImage2D& AImage, unsigned int AWidth );

//******************************************************************************
#endif // GENERATE_IMAGES_HH
//******************************************************************************
