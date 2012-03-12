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
#include "chrono.hh"
#include "region.hh"
using namespace std;
using namespace Map2d;
//******************************************************************************
bool CTopologicalMap::findMotifFrom(CDart* AStartDart,
                                    unsigned int AIndex,
                                    unsigned int AMarkToTest,
                                    unsigned int AMarkTreated,
                                    CTopologicalMap* AMap,
                                    CDart* ADestDart,
                                    unsigned int AMarkToTest2,
				    unsigned int AMarkTreated2,
                                    unsigned int* ANbMatched, bool AMirror)

{
   bool match = true;

   if ( ANbMatched!=NULL) *ANbMatched = 0;

   if (!isMarked(AStartDart, AMarkToTest) ||
       !AMap->isMarked(ADestDart, AMarkToTest2))
     return false;

   // Les 2 piles pour parcourir les 2 cartes en parallèle.
   stack<CDart*> toTreat;
   stack<CDart*> toTreat2;

   toTreat.push(AStartDart);
   toTreat2.push(ADestDart);

   // Le parcours, tant que la pile n'est pas vide.
   CDart* current;
   CDart* other;

   int i = 0;
   int j = 0;

   while (match && !toTreat.empty())
   {
      // Le prochain brin.
      current = toTreat.top();  toTreat.pop();
      other   = toTreat2.top(); toTreat2.pop();

      if ( isMarked(current, AMarkToTest) && !isMarked(current, AMarkTreated) )
      {
	if ( ANbMatched!=NULL) ++(*ANbMatched);

	if (AMap->isMarked(other, AMarkTreated2) ||
	    !AMap->isMarked(other, AMarkToTest2))
            match = false;
         else
         {
            setDirectInfo(current, AIndex, other);

            setMark(current, AMarkTreated);
            AMap->setMark(other, AMarkTreated2);

            // On teste si l'injection est valide avec les voisins.
            // On sort dès qu'il y a un problème.
            for (i = 0; match && i <= 2; ++i)
            {
               j = (AMirror && (i == 0 || i == 1)) ? (i + 1) % 2 : i;

               if (isMarked(beta(current, i), AMarkTreated))
               {
                  if (getDirectInfoAsDart(beta(current, i), AIndex) !=
                        AMap->beta(other, j))
                     match = false;
               }
               else
               {
                  if (AMap->isMarked(AMap->beta(other, j), AMarkTreated2))
                     match = false;
                  else
                  {
                     toTreat.push(beta(current, i));
                     toTreat2.push(AMap->beta(other, j));
                  }
               }
            }
         }
      }
   }

   return match;
}
//******************************************************************************
int CTopologicalMap::findMotif(unsigned int AMarkToTest,
                               CTopologicalMap* AMap,
                               unsigned int* ANbMatched, bool AMirror)
{
   unsigned int oneMatching;
   unsigned int * ptrOneMatching=NULL;

   if ( ANbMatched!=NULL )
     {
       *ANbMatched=0; ptrOneMatching = &oneMatching;
     }

   int index = getNewDirectInfo();

   CDynamicCoverageAll it1(this);
   for (; it1.cont(); ++it1)
      setDirectInfo(*it1, index, NULL);

   for (it1.reinit(); !isMarked(*it1, AMarkToTest) && it1.cont(); ++it1) {}

   if (!it1.cont()) return -1;

   int markTreated  = getNewMark();
   int markToTest2 = AMap->getNewMark();
   int markTreated2 = AMap->getNewMark();
   bool match = false;

   // Now we mark each infinite face inside the map.
   AMap->markOrbit(AMap->getInclusionTreeRoot()->getRepresentativeDart(),
		   ORBIT_FACE, markToTest2);
   for (CDynamicCoverageCCRegions itr(AMap->getInclusionTreeRoot());
        itr.cont(); ++itr)
     {
       AMap->markOrbit(AMap->beta2(itr->getRepresentativeDart()),
		       ORBIT_FACE, markToTest2);
     }

   AMap->negateMaskMark(markToTest2);

   CChrono c;
   c.start();

   for (CDynamicCoverageAll it2(AMap); !match && it2.cont(); ++it2)
   {
     match = findMotifFrom(*it1, index, AMarkToTest, markTreated,
			   AMap, *it2,markToTest2, markTreated2,
			   ptrOneMatching, AMirror);

     unmarkMotifMark(*it1, markTreated, (match ? -1 : index), -1,
		     AMap, *it2, markTreated2, AMirror);

     if ( ANbMatched!=NULL && oneMatching>(*ANbMatched) )
       (*ANbMatched) = oneMatching;
   }

   c.stop();
   c.display("Temps de recherche du motif");

   // assert( isWholeMapUnmarked(markTreated) );
   freeMark(markTreated);

   // assert( AMap->isWholeMapUnmarked(markTreated2) );
   AMap->freeMark(markTreated2);

   if (!match)
   {
      freeDirectInfo(index);
      return -1;
   }

   return index;
}
//******************************************************************************
unsigned int CTopologicalMap::countNumberOfMotifs(unsigned int AMarkToTest,
						  CTopologicalMap* AMap,
						  unsigned int* ANbMatched,
						  bool AMirror)
{
  unsigned int res = 0;
  unsigned int oneMatching;
  unsigned int * ptrOneMatching=NULL;

   if ( ANbMatched!=NULL )
     {
       *ANbMatched=0; ptrOneMatching = &oneMatching;
     }

   int index = getNewDirectInfo();

   CDynamicCoverageAll it1(this);
   for (; it1.cont(); ++it1)
      setDirectInfo(*it1, index, NULL);

   for (it1.reinit(); !isMarked(*it1, AMarkToTest) && it1.cont(); ++it1) {}

   if (!it1.cont()) return 0;

   int markTreated  = getNewMark();
   int markToTest2 = AMap->getNewMark();
   int markTreated2 = AMap->getNewMark();
   bool match = false;

   // Now we mark each infinite face inside the map.
   AMap->markOrbit(AMap->getInclusionTreeRoot()->getRepresentativeDart(),
		   ORBIT_FACE, markToTest2);
   for (CDynamicCoverageCCRegions itr(AMap->getInclusionTreeRoot());
        itr.cont(); ++itr)
     {
       AMap->markOrbit(AMap->beta2(itr->getRepresentativeDart()),
		       ORBIT_FACE, markToTest2);
     }

   AMap->negateMaskMark(markToTest2);

   CChrono c; c.start();
   for (CDynamicCoverageAll it2(AMap); it2.cont(); ++it2)
   {
     match = findMotifFrom(*it1, index, AMarkToTest, markTreated,
			   AMap, *it2,markToTest2, markTreated2,
			   ptrOneMatching, AMirror);

     unmarkMotifMark(*it1, markTreated, (match ? -1 : index), -1,
		     AMap, *it2, markTreated2, AMirror);

     if ( ANbMatched!=NULL && oneMatching>(*ANbMatched) )
       (*ANbMatched) = oneMatching;

     if ( match ) ++res;
   }

   c.stop();
   c.display("Temps de recherche de tout les motifs");

   // assert( isWholeMapUnmarked(markTreated) );
   freeMark(markTreated);

   // assert( AMap->isWholeMapUnmarked(markTreated2) );
   AMap->freeMark(markTreated2);

   freeDirectInfo(index);

   return res;
}
//******************************************************************************
bool CTopologicalMap::isFacesMatch(CDart * AFromDart, int AIndex,
                                   unsigned int AMarkTreated,
                                   CTopologicalMap * AMap, CDart * ADestDart)
{
   assert(!isMarked(AFromDart, AMarkTreated));

   CDynamicCoverage1 it1(this, AFromDart);
   CDynamicCoverage1 it2(AMap, ADestDart);
   bool match = true;

   while (it1.cont())
   {
      if (!it2.cont())
         match = false;
      else
      {
         // On vérifie la comptabilité du voisin par beta2 si besoin
         if (match && isMarked(beta2(*it1), AMarkTreated) &&
               getDirectInfoAsDart(beta2(*it1), AIndex) != beta2(*it2))
            match = false;
         ++it2;
      }
      setMark(*it1, AMarkTreated);
      ++it1;
   }

   // On a fini it1, si on n'a pas fini it2 alors on ne matche pas.
   if (it2.cont()) match = false;

   return match;
}
//******************************************************************************
/*unsigned int CTopologicalMap::findMaximalMotifFrom(CDart * AFromDart,
      unsigned int AMarkTreated, int AIndex, CTopologicalMap * AMap,
      CDart * ADestDart)
{
   // Les 2 piles pour parcourir les 2 cartes en parallèle.
   stack<CDart*> toTreat;
   stack<CDart*> toTreat2;

   toTreat.push(AFromDart);
   toTreat2.push(ADestDart);

   // Le parcours, tant que la pile n'est pas vide.
   CDart* current;
   CDart* other;

   unsigned int nbMatch = 0;

   while (!toTreat.empty())
   {
      // Le prochain brin.
      current = toTreat.top();  toTreat.pop();
      other   = toTreat2.top(); toTreat2.pop();

      // On teste si les faces matche
      if ( !isMarked(current, AMarkTreated)
    && isFacesMatch(current, AIndex, AMarkTreated, AMap, other) )
      {
         CDynamicCoverage1 it1(this, current);
         CDynamicCoverage1 it2(AMap, other);
         while (it1.cont())
         {
            setDirectInfo(*it1, AIndex, *it2);

            if (!isMarked(beta2(*it1), AMarkTreated))
            {
               toTreat.push(beta2(*it1));
               toTreat2.push(beta2(*it2));
            }

            ++it1; ++it2;
            ++nbMatch;
         }
      }
   }

   return nbMatch;
}
*/
//******************************************************************************
unsigned int CTopologicalMap::findMaximalMotif2From(CDart * AFromDart,
      unsigned int AMarkTreated, int AIndex, CTopologicalMap * AMap,
      CDart * ADestDart, unsigned int AMarkTreated2)
{
   // Les 2 piles pour parcourir les 2 cartes en parallèle.
   stack<CDart*> toTreat;
   stack<CDart*> toTreat2;

   toTreat.push(AFromDart);
   toTreat2.push(ADestDart);

   // Le parcours, tant que la pile n'est pas vide.
   CDart* current;
   CDart* other;

   unsigned int nbMatch = 0;
   bool match = true;
   int i = 0;

   while (!toTreat.empty())
   {
      // Le prochain brin.
      current = toTreat.top();  toTreat.pop();
      other   = toTreat2.top(); toTreat2.pop();

      // On teste si les brins matche
      if (!isMarked(current, AMarkTreated))
      {
         setMark(current, AMarkTreated);

         if (!AMap->isMarked(other, AMarkTreated2))
         {
            AMap->setMark(other, AMarkTreated2);
            match = true;
            for (i = 0; match && i <= 2; ++i)
            {
               if (isMarked(beta(current, i), AMarkTreated))
               {
                  if (getDirectInfo(beta(current, i), AIndex)
                        != AMap->beta(other, i))
                     match = false;
               }
               else
               {
                  if (AMap->isMarked(AMap->beta(other, i),
                                     AMarkTreated2))
                     match = false;
               }
            }
            if (match)
            {
               setDirectInfo(current, AIndex, other);
               ++nbMatch;

               for ( i=0; i<=2; ++i )
               {
                  if (!isMarked(beta(current, i), AMarkTreated))
                  {
                     toTreat.push(beta(current, i));
                     toTreat2.push(AMap->beta(other, i));
                  }
               }
            }
         }
      }
   }

   return nbMatch;
}
//******************************************************************************
void CTopologicalMap::unmarkMotifIndex(CDart* ADart, int AIndex)
{
   assert(getDirectInfo(ADart, AIndex) != NULL);

   stack<CDart*> toTreat;
   toTreat.push(ADart);
   CDart* current;
   int i;

   while (!toTreat.empty())
   {
      current = toTreat.top(); toTreat.pop();

      if (getDirectInfo(current, AIndex) != NULL)
      {
         for (i = 0; i <= 2; ++i)
            if (getDirectInfo(beta(current, i), AIndex) != NULL)
               toTreat.push(beta(current, i));

         setDirectInfo(current, AIndex, NULL);
      }
   }
}
//******************************************************************************
void CTopologicalMap::unmarkMotifMark(CDart* ADart, int AMark, int AIndex,
                                      int AIndexCopy, CTopologicalMap* AMap,
                                      CDart* ADart2, int AMark2,
                                      bool AMirror)
{
   if (!isMarked(ADart, AMark)) return;

   stack<CDart*> toTreat;
   stack<CDart*> toTreat2;

   toTreat.push(ADart);
   if (AMark2!=-1)
   {
      assert(AMap!=NULL);
      assert(ADart2!=NULL);
      toTreat2.push(ADart2);
   }

   CDart* current=NULL;
   CDart* other=NULL;
   int i,j;

   while (!toTreat.empty())
   {
      current = toTreat.top(); toTreat.pop();

      if ( AMark2!=-1 )
         { other = toTreat2.top(); toTreat2.pop(); }

      if ( isMarked(current, AMark) )
      {
         for (i = 0; i <= 2; ++i)
         {
            j = (AMirror && (i == 0 || i == 1)) ? (i + 1) % 2 : i;

            toTreat.push(beta(current, i));
            if ( AMark2!=-1 )
                  toTreat2.push(AMap->beta(other, j));
         }
         if (AIndex != -1)
         {
            if (AIndexCopy!=-1)
               setDirectInfo(current, AIndexCopy,
                             getDirectInfo(current, AIndex));

            setDirectInfo(current, AIndex, NULL);
         }
         unsetMark(current, AMark);
         if ( AMark2!=-1 ) AMap->unsetMark(other, AMark2);
      }
   }
}
//******************************************************************************
unsigned int CTopologicalMap::findMaximalMotif(int AIndex,
                                               CTopologicalMap * AMap)
{
   unsigned int nbMatch = 0;
   unsigned int res = 0;
   CDart* precModif = NULL;
   bool newMax = false;

   int treatedMark = getNewMark();
   int index2 = getNewDirectInfo();
   int treatedMark2 = AMap->getNewMark();

   for (CDynamicCoverageAll it1(this); it1.cont(); ++it1)
   {
      setDirectInfo(*it1, AIndex, NULL);
      setDirectInfo(*it1, index2, NULL);
   }

   unsigned int nbDarts = 0;
   unsigned int percent = 0;
   unsigned int onePercent = (getNbDarts() + 99) / 100;

   for (CDynamicCoverageAll it1(this); it1.cont(); ++it1)
   {
      for (CDynamicCoverageAll it2(AMap); it2.cont(); ++it2)
      {
         res = findMaximalMotif2From(*it1, treatedMark,
                                     index2, AMap, *it2, treatedMark2);

         if (res > nbMatch)
         {
            if (precModif != NULL) unmarkMotifIndex(precModif, AIndex);
            nbMatch = res;
            precModif = *it1;
            newMax = true;
         }
         else newMax = false;

         unmarkMotifMark(*it1, treatedMark, index2, (newMax?AIndex:-1));
         // Bug bizarre....  //   AMap, *it2, treatedMark2);
         AMap->unmarkMotifMark(*it2, treatedMark2);
      }
      ++nbDarts;
      if (nbDarts == onePercent && it1.cont())
      {
         ++percent;
         std::cout << " [" << setw(3) << percent
         << "%] findMaximalMotif" << std::endl;
         nbDarts = 0;
      }
   }

   if (nbDarts != 0)
      std::cout << " [100%] findMaximalMotif" << std::endl;

   freeMark(treatedMark);
   AMap->freeMark(treatedMark2);
   return nbMatch;
}
//******************************************************************************
