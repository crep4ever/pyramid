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
#ifndef ORDERS_HH
#define ORDERS_HH
//******************************************************************************
#include "region.hh"
#include "topological-dart.hh"
//******************************************************************************
/**
 * Ordre sur les régions utilisant les ID. Ces ID doivent vérifier la propriété
 * suivante:
 *  r1->getId() < r2->getID() ssi r1 est trouvé avant r2 lors d'un balayage de 
 * l'image de gauche à droite et de haut en bas.
 */
class orderRegionId:
      public std::binary_function<Map2d::CRegion*, Map2d::CRegion*, bool>
{
   public:
      bool operator()(Map2d::CRegion* ARegion1, Map2d::CRegion* ARegion2)
      { return ARegion1->getId() < ARegion2->getId(); }
};
//******************************************************************************
/**
 * Ordre sur les brins utilisant les intervalles de valeurs de leurs régions.
 * Un brin est plus petit qu'un autre si la différence entre les intervalles de
 * sa région et de la région accessible par beta2 est plus petit que la même
 * différence pour l'autre brin.
 */
class orderEdgesInterval:
      public std::binary_function<Map2d::CTopologicalDart*, 
      Map2d::CTopologicalDart*, bool>
{
   public:
      bool operator()(Map2d::CTopologicalDart* ADart1, 
                    Map2d::CTopologicalDart* ADart2)
      {
         return
               ADart1->getRegion()->distanceWith
               (static_cast<Map2d::CTopologicalDart*>(ADart1->getBeta2())
               ->getRegion()) <
               ADart2->getRegion()->distanceWith
               (static_cast<Map2d::CTopologicalDart*>(ADart2->getBeta2())
               ->getRegion());
      }
};
//******************************************************************************
#endif // ORDERS_HH
//******************************************************************************
