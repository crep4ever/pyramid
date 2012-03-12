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
#include "inline-macro.hh"
#include "topological-map.hh"
#include "coverage-region-pixels.hh"
#include "chrono.hh"
#include INCLUDE_NON_INLINE("topological-map.icc")
#include <iostream>
#include <iomanip>
using namespace std;
using namespace Map2d;
//******************************************************************************
CTopologicalMap::~CTopologicalMap()
{
   empty();

   //  delete FImage;
   delete FKhalimsky;
   delete FInclusionTreeRoot;
}
//******************************************************************************
bool CTopologicalMap::isMapOk()
{
   //  CDart* dart = NULL;

   // En premier on teste la topologie cad les contraintes générales des cartes.
   if (!checkTopology())
   {
      std::cerr << "CTopologicalMap::isMapOk : Problème de topologie."
            << std::endl;
      return false;
   }

   // Puis les caractéristiques spécifiques à CTopologicalMap.
   for (CDynamicCoverageAll it(this); it.cont(); ++it)
   {
      // A) D'abord les caractéristiques topologiques.

      // 1. Aucun brin ne doit être libre pour beta2.
      if (isFree2(*it))
      {
         std::cerr << "CTopologicalMap::isMapOk : Le brin " << *it
         << " est 2-libre." << std::endl;
         return false;
      }

      // 2. Les sommets ne doivent pas être de degré deux (excepté le cas
      //    d'une boucle incidente à une arête)
      //  int r = reelVertexDegree(*it);
//       if ( r==2 && findVertexType(*it, &dart)!=3 )
//  {
//    std::cerr<<"CTopologicalMap::isMapOk : "
//      <<"Problème de degré : lesommet " <<getDoublet(*it)
//      <<" est de degré réel deux."
//      <<std::endl;
//    return false;
//  }
//       // 3. Les sommets ne doivent pas être de degré zéro (excepté le cas
//       //    d'une sphère, et le cas d'un sommet incident uniquement à des boucles)
//       if ( r==0 )
//  {
//    if  ( (beta0(*it)!=beta2(*it) && beta1(*it)==beta2(*it)) ||
//   (beta1(*it)!=beta2(*it) && beta0(*it)==beta2(*it)) )
//      {
//        std::cerr<<"CTopologicalMap::isMapOk : "
//          <<"Problème : L'arête fictive incidente au brin "
//          <<*it<<" est pendante."<<std::endl;
//        return false;
//      }
//    // else if ( (beta0(*it)!=beta2(*it) || beta1(*it)!=beta2(*it)) &&
//    //     findIncidentEdgeNonLoop(*it)!=NULL )
//      else if (beta0(*it)!=beta2(*it))
//      {
//        std::cerr<<"CTopologicalMap::isMapOk : "
//          <<"Problème : le sommet fictif " <<getDoublet(*it)
//          <<" n'est pas incident qu'a des boucles."<<std::endl;
//        return false;
//      }
//  }

//       // 5. Et les arêtes réelles sont forcément de degré local supérieur strict à 2.
//        int e =  localDegree(*it,1);
//        if ( e<=2 )
//        {
//          std::cerr<<"CTopologicalMap::isMapOk : "
//            <<"Problème de degré : l'arête réelle "<<*it
//            <<" est de degré local égal à "<<e<<std::endl;
//          return false;
//        }

//       // B) Puis les caractéristiques géométriques.

//       // 1. Il faut que chaque doublet désigne un SCell.
//       //  if ( !isSCell(getTriplet(*it)) )
//       //{
//       //  std::cerr<<"CTopologicalMap::isMapOk : "
//       //    <<"Le triplet "<<getTriplet(*it)
//       //    <<" ne désigne pas un SCell."<<std::endl;
//       //  return false;
//       //}

//       // 2. Il faut que chaque doublet désigne un LCell.
//       //if ( !isFictiveEdge(*it) && !isLCell(getDoublet(*it)) )
//       if(!isLCell(getDoublet(*it)))
//      {
//        std::cerr<<"CTopologicalMap::isMapOk : "
//      <<"Le doublet "<<getDoublet(*it)
//      <<" ne désigne pas un LCell."<<std::endl;
//        return false;
//      }
//       // Et que chaque triplet fictif désigne un LCellFictif.
// //        if ( isFictiveEdge(*it) && !isFictiveLCell(getTriplet(*it)) )
// //     {
// //       std::cerr<<"CTopologicalMap::isMapOk : "
// //           <<"Un triplet fictif ne désigne pas un LCell fictif."<<std::endl;
// //       return false;
// //     }

//       // 3. Il faut que chaque doublet d'une arête non boucle désigne un PCell.
//       //if ( !isFictiveEdge(*it) && !isEdgeLoop(*it) )
//       if(!isEdgeLoop(*it))
//  {
//    if ( !isPCell(getDoublet(*it)) )
//      {
//        std::cerr<<"CTopologicalMap::isMapOk : "
//          <<"Le Doublet "<<getDoublet(*it)
//          <<" ne désigne pas un PCell."<<std::endl;
//        return false;
//      }
//  }

//       // Et que chaque triplet fictif désigne un PCellFictif.
// //        if ( isFictiveEdge(*it) && !isFictivePCell(getTriplet(*it)) )
// //   {
// //     std::cerr<<"CTopologicalMap::isMapOk : "
// //         <<"Un triplet fictif ne désigne pas un PCell fictif."<<std::endl;
// //     return false;
// //   }

//       // 4. Il faut que les pointels de chaque sommet soient les mêmes.
//       for (CDynamicCoverageVertex it2(this, *it); it2.cont(); ++it2)
//  {
//    if ( !getDoublet(*it).samePointel(getDoublet(*it2)) )
//      {
//        std::cerr<<"CTopologicalMap::isMapOk : "
//          <<"Problème de plongement des sommets pour le doublet "
//          <<getDoublet(*it)<<std::endl;
//        return false;
//      }
//  }

//       // 5. Il faut que les lignels de chaque demi-arête soient les mêmes.
//       // if ( !isFictiveEdge(*it) )
//  for (CDynamicCoverageEdge it2(this, *it); it2.cont(); ++it2)
//    {
//      if ( getDoublet(*it).getLinel()!=getDoublet(*it2).getLinel() )
//        {
//   std::cerr<<"CTopologicalMap::isMapOk : "
//     <<"Problème de plongement des arêtes pour le doublet "
//     <<getDoublet(*it)<<std::endl;
//   return false;
//        }
//      ++it2; // Pour sauter un brin sur deux de l'arête.
//    }

   }
   return true;
}
//******************************************************************************
void CTopologicalMap::displayMapCharacteristics()
{
   unsigned int nbD = 0;
   unsigned int nbV = 0;
   unsigned int nbE = 0;
   unsigned int nbF = 0;
   unsigned int nbCC = 0;

   getGlobalCharacteristics(&nbD, &nbV, &nbE, &nbF, &nbCC);

   cout << endl << "  Carte : "
   << "  brins : " << nbD << " (sur " << getNbDartsInTabs() << ")"
   << ", sommets : " << nbV
   << ", aretes : " << nbE
   << ", faces : " << nbF
   << ", cc : " << nbCC
   << "  régions : " << getNbRegions();

   double sum_length=0;
   double std_dev=0;
   for(CDynamicCoverageAll it(this) ; it.cont() ; it++)
   {
      sum_length += (CVertex(getDoublet(*it)) - getDoublet(beta0(*it))).norm();
   }
   for(CDynamicCoverageAll it(this) ; it.cont() ; it++)
   {
      std_dev += ((CVertex(getDoublet(*it)) -
            getDoublet(beta0(*it))).norm() - sum_length/nbD) *
            ((CVertex(getDoublet(*it)) -
            getDoublet(beta0(*it))).norm() - sum_length/nbD);
   }
   cout << " Longueur moyenne : "<<sum_length/nbD
   << " Std Dev : "<<std_dev/nbD<<endl;

   cout << "  Espace mémoire : "
   << "  carte : " << getMemoryForMap()
   << ", arbre : " << getMemoryForInclusionTree()
   << ", khalimsky : " << getMemoryForKhalimsky()
   << ", image : " << getMemoryForImage()
   << "  Total : "
   << getMemoryForMap() + getMemoryForInclusionTree() +
   getMemoryForKhalimsky() + getMemoryForImage() << endl << endl;
}
//******************************************************************************
void CTopologicalMap::markInteriorWithoutInfiniteFace(unsigned int AMark)
{
   CDynamicCoverage1 it(this, getInclusionTreeRoot()->getRepresentativeDart());
   for (;it.cont(); ++it)
   {
     setMark(*it, AMark); // To mark the infinite face
     markOrbit(beta2(*it), ORBIT_FACE, AMark); // To mark each face
					       // incident to the
					       // infinite face
   }

   // Now we mark each infinite face inside the map.
   for (CDynamicCoverageCCRegions itr(getInclusionTreeRoot());
	itr.cont(); ++itr)
     {
       markOrbit(itr->getRepresentativeDart()->getBeta(2), ORBIT_FACE, AMark);
     }

   negateMaskMark(AMark);
}
//******************************************************************************
void CTopologicalMap::markBiggestCC(unsigned int AMark)
{
  unsigned int treated = getNewMark();
  unsigned int currentmark = getNewMark();
  unsigned int biggestCCMark = AMark;
  unsigned int biggestCC = 0;
  unsigned int currentCC = 0;
  unsigned int tmp;

  assert(isWholeMapUnmarked(AMark));

  markInteriorWithoutInfiniteFace(treated);
  negateMaskMark(treated); // Because we want to mark the infinite face
			   // and its adjacent faces

  std::stack<CDart*> totreat;
  CDart* currentDart=NULL;

  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      if (!isMarked(*it, treated))
	{
	  currentCC = 0;
	  totreat.push(*it);
	  while ( !totreat.empty() )
	    {
	      currentDart=totreat.top(); totreat.pop();
	      if ( !isMarked(currentDart, treated))
		{
		  for (CDynamicCoverage1 it2(this, currentDart);
		       it2.cont(); ++it2)
		    {
		      setMark(*it2,treated);
		      setMark(*it2, currentmark);
		      ++currentCC;
		      if ( !isMarked((*it2)->getBeta(2),treated) )
			totreat.push((*it2)->getBeta(2));
		    }
		}
	    }
	  if ( currentCC > biggestCC )
	    {
	      biggestCC = currentCC;
	      tmp = biggestCCMark;
	      biggestCCMark = currentmark;
	      currentmark = tmp;
	    }
	  unmarkAll(currentmark);
	}
    }

  unsigned int res=0;
  if ( biggestCCMark!=AMark )
    {
      for (CDynamicCoverageAll it(this); it.cont(); ++it)
	{
	  if ( isMarked(*it, biggestCCMark))
	    {
	      ++res;
	      setMark(*it, AMark);
	      unsetMark(*it, biggestCCMark);
	    }
	  else unsetMark(*it, AMark);
	  unsetMark(*it,treated);
	}
      currentmark = biggestCCMark;
    }
  else
    {
      for (CDynamicCoverageAll it(this); it.cont(); ++it)
	{
	  if ( isMarked(*it, biggestCCMark))  ++res;
	  unsetMark(*it, treated);
	  unsetMark(*it, currentmark);
	}
    }

  //  assert( isWholeMapUnmarked(treated) );
  freeMark(treated);

  //  assert( isWholeMapUnmarked(currentmark) );
  freeMark(currentmark);
}
//******************************************************************************
TCoordinate CTopologicalMap::distanceToCurve(CDart* ADart, const CVertex & AV1,
      const CVertex& AV2)
{
   TCoordinate res = 0;
   TCoordinate temp = 0;

   CDoublet doublet(getDoublet(ADart));
   CDoublet tempDoublet(doublet);

   res = distancePointToLine(tempDoublet, AV1, AV2);

   do
   {
      /*doublet suivant*/
      tempDoublet.setNextPointel();

      temp = distancePointToLine(tempDoublet, AV1, AV2);
      if (temp > res) res = temp;

      /*on récupère le linel du doublet suivant de l'arête*/
      if (!isPCell(tempDoublet))
         tempDoublet = getOtherLinel(tempDoublet);
   }
   // l'arête se termine par un pointel ou par le linel initial (boucle).
   while ((!isPCell(tempDoublet)) && (tempDoublet != doublet));

   return res;
}
//******************************************************************************
TCoordinate CTopologicalMap::distanceToCurve(CDart* ADart, const CVertex & AV1,
                                             const CVertex& AV2,
                                             TCoordinate AThreshold)
{
   TCoordinate res = 0;
   TCoordinate temp = 0;

   CDoublet doublet(getDoublet(ADart));
   CDoublet tempDoublet(doublet);

   res = distancePointToLine(tempDoublet, AV1, AV2);
   if (temp > res) res = temp;
   if (res>=AThreshold) return res;

   do
   {
      /*doublet suivant*/
      tempDoublet.setNextPointel();

      temp = distancePointToLine(tempDoublet, AV1, AV2);
      if (temp > res)
      {
         res = temp;
         if (res>=AThreshold) return res;
      }
      /*on récupère le linel du doublet suivant de l'arête*/
      if (!isPCell(tempDoublet))
         tempDoublet = getOtherLinel(tempDoublet);
   }
   // l'arête se termine par un pointel ou par le linel initial (boucle).
   while ((!isPCell(tempDoublet)) && (tempDoublet != doublet));

   return res;
}
//******************************************************************************
void CTopologicalMap::randomizeDarts()
{
   if (getNbDarts() == 0) return;

   std::vector<CDart*> tabsDarts;
   unsigned int nbDartsInLastTab = 0;
   unsigned int nbTabsDarts = 0;

   unsigned int i = 0, j = 0;
   int toTreat = getNbDarts();

   int directIndex = getNewDirectInfo();
   assert(directIndex != -1);

   srand(time(NULL)*getpid());

   unsigned int treated = getNewMark();
   unsigned int dartI = 0, dartJ = 0;
   CTopologicalDart* randomDart = NULL;
   CTopologicalDart* destDart = NULL;
   CDart* cur = NULL;

   tabsDarts.push_back(newTabDarts());
   ++nbTabsDarts;

   while (toTreat > 0)
   {
        // 1) On cherche un brin aléatoirement
      randomDart = NULL;
      dartI = rand() % getNbTabsDarts();
      dartJ = rand() % (dartI == getNbTabsDarts() - 1 ?
            getNbDartsInLastTab() : getSizeDartArray());

      while (randomDart == NULL)
      {
         cur = getDart(dartI, dartJ);
         if (!isMarked(cur, treated) && !isMarkedDeleted(cur))
            randomDart = static_cast<CTopologicalDart*>(cur);
         else
         {
            ++dartJ;
            if ((dartI == getNbTabsDarts() - 1 &&
                 dartJ == getNbDartsInLastTab()) ||
                 dartJ == getSizeDartArray())
            {
               ++dartI; dartJ = 0;
               if (dartI == getNbTabsDarts())
                  dartI = 0;
            }
         }
      }

        // 2) On affecte ce brin dans la case [i][j]
      assert(randomDart != NULL);
      assert(!isMarkedDeleted(randomDart));
      assert(!isMarked(randomDart, treated));

      destDart = (static_cast<CTopologicalDart*>(tabsDarts[i]) + j);

      copyDart(destDart, randomDart);

      // On met à jour les brins représentants
      if ( isRepresentativeDart(randomDart) )
         setRepresentativeDart(getRegion(randomDart),destDart);

#ifdef CONTOUR_SIMPLIFICATION
      randomDart->setEdgeAttribute(NULL);
#endif

      setMark(randomDart, treated);
      setDirectInfo(randomDart, directIndex, destDart);

        // 3) On refait les coutures avec les brins voisins déjà traités
      for (int dim = 0; dim < 3; ++dim)
      {
         if (isMarked(beta(randomDart, dim), treated))
         {
            linkBeta(destDart,
                     static_cast<CTopologicalDart*>
                           (getDirectInfo(beta(randomDart, dim), directIndex)),
                            dim);
         }
      }

        // 4) On met à jour les indices
      --toTreat;
      ++j;

      if (j == FSizeDartArray)
      {
         ++i; j = 0;
         tabsDarts.push_back(newTabDarts());
         ++nbTabsDarts;
      }
   }

   nbDartsInLastTab = j;

    // On détruit tout les anciens tableaux de brins
   for (i = 0; i < FNbTabsDarts; ++i)
   {
      delete [](static_cast<CTopologicalDart*>(FTabsDarts[i]));
      FTabsDarts[i] = NULL;
   }

    // Maintenant on recopie les variables locales en global.
   FNbTabsDarts      = nbTabsDarts;
   FNbDartsInLastTab = nbDartsInLastTab;
   FFirstDeleteDart  = NULL;

   for (i = 0; i < FNbTabsDarts; ++i)
   {
      FTabsDarts[i] = tabsDarts[i];
   }

   freeDirectInfo(directIndex);
   freeMark(treated);
}
//******************************************************************************
bool CTopologicalMap::saveImage(const char* AFilename)
{
   CChrono c;
   c.start();

   CImage2D im(FImage->getXSize(),FImage->getYSize());
   for ( CDynamicCoverageAllRegions it(this); it.cont(); ++it )
   {
      if (!it->isInfiniteRegion())
      {
      TRegionId val(it->getColorMean());
      for (CCoverageRegionPixels pixels(this,*it);
           pixels.cont(); ++pixels)
      {
         im.setPixel((*pixels).getX(),(*pixels).getY(),val);
      }
      }
   }

   c.display("Calcul de l'image");

   im.setDisplayMinMax(0, 255);
   im.exportWithMagick(AFilename);
   c.display("Sauvegarde");
   return true;
}
//******************************************************************************
bool CTopologicalMap::savePartition(const char* AFilename)
{
   CChrono c;
   c.start();

   TLabel i=0;
   CImage2D im(FImage->getXSize(),FImage->getYSize());
   for ( CDynamicCoverageAllRegions it(this); it.cont(); ++it )
   {
      if (!it->isInfiniteRegion())
      {
        ++i;
        for (CCoverageRegionPixels pixels(this,*it);
        pixels.cont(); ++pixels)
        {
          im.setPixel((*pixels).getX(),(*pixels).getY(),i);
        }
      }
    }

   c.display("Calcul de l'image de label");

   im.setDisplayMinMax(1, i);
   im.exportWithMagick(AFilename);
   c.display("Sauvegarde");
   return true;
}
//******************************************************************************
bool CTopologicalMap::changeImageData( CImage2D * AImage )
{
  if(    AImage->getXSize() != getSizeX()
      || AImage->getYSize() != getSizeY() )
    {
      std::cerr << "Error : "
                << "cannot load new image data (incorrect size)."
                << std::endl;
      return false;
    }

  TRegionId color;
  for( CDynamicCoverageAllRegions rIt( this ) ; rIt.cont() ; rIt++ )
    {
      (*rIt)->resetColorData();
      for( CCoverageRegionPixels vIt( this, *rIt ) ; vIt.cont() ; vIt++ )
        {
          color = AImage->getPixel( (*vIt).getX(), (*vIt).getY() );
          FImage->setPixel( (*vIt).getX(), (*vIt).getY(), color );
          (*rIt)->addPixel( color );
        }
    }

  unmarkAllPixels();
  return true;
}
//******************************************************************************
void CTopologicalMap::exportGraph(std::ostream &os, int mark)
{
  int index = getNewDirectInfo();
  int treated = getNewMark();
  //  assert( isWholeMapUnmarked(treated) );

  unsigned int nbv=0;
  unsigned int nbe=0;
  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      if ( !isMarked(*it, treated) )
	{
	  if ( mark==-1 || isMarked(*it,mark) )
	    {
	      ++nbv;
	      for (CDynamicCoverageVertex it2(this,*it); it2.cont(); ++it2)
		{
		  setDirectInfo(*it2, index, (void*)nbv);
		  setMark(*it2,treated);
		}
	    }
	  else
	    {
	      setDirectInfo(*it, index, (void*)0);
	    }
	}

      if ( mark==-1 || isMarked(*it,mark) || isMarked((*it)->getBeta2(),mark) )
	++nbe;
    }

  os<<"#nbv #nbe"<<std::endl;
  os<<nbv<<"  "<<nbe/2<<std::endl;

  negateMaskMark(treated);
  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      if (!isMarked(*it, treated) &&
	  (unsigned long)getDirectInfo(*it, index)!=0 )
	{
	  os<<getDoublet(*it).getX()<<" "<<getDoublet(*it).getY()<<std::endl;
	  markOrbit(*it, ORBIT_VERTEX, treated);
	}
    }

  negateMaskMark(treated);
  for (CDynamicCoverageAll it(this); it.cont(); ++it)
    {
      if (!isMarked(*it, treated) &&
	  (unsigned long)getDirectInfo(*it, index)!=0 &&
	  (unsigned long)getDirectInfo((*it)->getBeta2(), index)!=0 )
	{
	  assert( (unsigned long)getDirectInfo(*it, index)<=nbv );
	  assert( (unsigned long)getDirectInfo((*it)->getBeta2(), index)<=nbv );
	  os<<(unsigned long)getDirectInfo(*it, index)<<" "
	    <<(unsigned long)getDirectInfo((*it)->getBeta2(), index)<<std::endl;
	  markOrbit(*it, ORBIT_EDGE, treated);
	}
      else setMark(*it, treated);
    }

  negateMaskMark(treated);
  freeMark(treated);
  freeDirectInfo(index);
}
//******************************************************************************
