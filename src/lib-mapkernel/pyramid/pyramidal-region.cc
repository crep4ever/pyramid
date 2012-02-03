/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
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

#include "inline-macro.hh"
#include "pyramidal-region.hh"
#include INCLUDE_NON_INLINE("pyramidal-region.icc")

using namespace Map2d;

CInfinitePyramidalRegion::~CInfinitePyramidalRegion()
{
  //std::cout<<" [start] CInfinitePyramidalRegion::destructeur() "<<std::endl;
  setUp(NULL);
  setDown(NULL);
}

void CPyramidalRegion::addPixels( CRegion* ARegion )
{
  assert(false);
  incNbPixels( ARegion->getNbPixels() );
  addGrey(static_cast<CPyramidalRegion*>(ARegion)->getGreySum());
  // Mise à jour de la somme des carrés des niveaux de gris
  //setGreySquareSum(getGreySquareSum()+ ARegion->getGreySquareSum());

  TRegionId min = static_cast<CPyramidalRegion*>(ARegion)->getGreyMin();
  TRegionId max = static_cast<CPyramidalRegion*>(ARegion)->getGreyMax();

  if( min  < getGreyMin() )
    setGreyMin(min);

  if( max  > getGreyMax() )
    setGreyMax(max);

  // Mise à jour de l'écart-type
  // TODO : à vérifier
  /*
    float deviation1 = getDeviation();
    float deviation2 = static_cast<CPyramidalRegion*>(ARegion)->getDeviation();
    unsigned int nbPix1 = getNbPixels();
    unsigned int nbPix2 = static_cast<CPyramidalRegion*>(ARegion)->getNbPixels();

    setDeviation( sqrt( (nbPix1*(deviation1)*(deviation1) + nbPix2*(deviation2)*(deviation2))/(nbPix1 + nbPix2) ) );
  */
}

//******************************************************************************
// Méthodes de contrôle et de vérification
//******************************************************************************

void CPyramidalRegion::print()
{
#ifdef DEBUG_PYRAMID
  //assert(isOk());
  std::string sVoid = "-";
  std::cout << std::endl;

  if(isInfiniteRegion())
    {
      std::cout << " Région Infinie:   Id: " << getId() << std::endl;

      std::cout << "  Représentant: "
	        << static_cast<CPyramidalDart*>(getRepresentativeDart())->doublet()
	        << "   Region Up: ";
      if(existUp())
	std::cout << up()->getId();
      else
	std::cout << sVoid;
      std::cout << "   Region Down: ";
      if(existDown())
	std::cout  <<  down()->getId();
      else
	std::cout << sVoid;
      std::cout << std::endl;

      std::cout << "  Father: ";
      if(existFather())
	std::cout << getFather()->getId();
      else
	std::cout << sVoid;
      std::cout << "   First Son: ";
      if(existSon())
	std::cout << getFirstSon()->getId();
      else
	std::cout << sVoid;
      std::cout << "   Brother: ";
      if(existBrother())
	std::cout << getBrother()->getId();
      else
	std::cout << sVoid;
      std::cout << "   Next Same CC: ";
      if(existNextSameCC())
	std::cout << getNextSameCC()->getId();
      else
	std::cout << sVoid;
      std::cout << std::endl;

    }
  else
    {
      std::cout << " Région pyramidale:   Id: " << getId() << std::endl;

      std::cout << "  Paramètres: ";
      std::cout << "   First Pixel: "   << getFirstPixel();
      std::cout << "   Profondeur: "    << getProfondeur();
      std::cout << "   Nb pixels: "     << getNbPixels();
      std::cout << std::endl;

      std::cout << "  Critères:   ";
      //std::cout << "   Merge: "      << getMerge();
      //std::cout << "   Split: "      << getSplit();
      //std::cout << "   Ecart-type: " << getDeviation();
      std::cout << "   Sum: "      << getGreySquareSum();
      std::cout << std::endl;

      std::cout << "  Couleurs:   ";
      //std::cout << "   Moyenne: "  << getAverageGrey();
      std::cout << "   Somme: "    << getGreySum();
      std::cout << "   Min: "      << getGreyMin();
      std::cout << "   Max: "      << getGreyMax();
      std::cout << std::endl;

      std::cout << "  Représentant: " << static_cast<CPyramidalDart*>(getRepresentativeDart())->doublet();
      std::cout << "   Region Up: ";
      if(existUp())
	std::cout << up()->getId();
      else
	std::cout << sVoid;
      std::cout << "   Region Down: ";
      if(existDown())
	std::cout << down()->getId();
      else
	std::cout << sVoid;
      std::cout << std::endl;

      std::cout << "  Father: ";
      if(existFather())
	std::cout << getFather()->getId();
      else
	std::cout << sVoid;
      std::cout << "   First Son: ";
      if(existSon())
	std::cout << getFirstSon()->getId();
      else
	std::cout << sVoid;
      std::cout << "   Brother: ";
      if(existBrother())
	std::cout << getBrother()->getId();
      else
	std::cout << sVoid;
      std::cout << "   Next Same CC: ";
      if(existNextSameCC())
	std::cout << getNextSameCC()->getId();
      else
	std::cout << sVoid;
      std::cout << std::endl;

      std::cout << "   Histological Label: ";
      switch(label())
	{
	case Background:
	  std::cout << "Background";
	  break;
	case Tissue:
	  std::cout << "Tissue";
	  break;
	case DarkTissue:
	  std::cout << "DarkTissue";
	  break;
	case BrightTissue:
	  std::cout << "BrightTissue";
	  break;
	case Lesion:
	  std::cout << "Lesion";
	  break;
	case Cancer:
	  std::cout << "Cancer";
	  break;
	case Stroma:
	  std::cout << "Stroma";
	  break;
	case Mitosis:
	  std::cout << "Mitosis";
	  break;
	case InSitu:
	  std::cout << "InSitu";
	  break;
	case Invalid:
	  std::cout << "Invalid";
	  break;
	default:
	  std::cout << sVoid;
	  break;
	}
      std::cout << std::endl;
    }
#endif
}

bool CPyramidalRegion::isOk()
{
  if( !getRepresentativeDart() )
    {
      std::cout << "Representative dart is NULL for region " << id() << std::endl;
      return false;
    }
  return true;
}
