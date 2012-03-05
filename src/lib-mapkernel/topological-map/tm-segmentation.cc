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
#include "orders.hh"
#include "coverage-edge-linels.hh"
#include "chrono.hh"
using namespace std;
using namespace Map2d;
//******************************************************************************
void CTopologicalMap::transformInclusionTreeInOrderedList()
{
  CDynamicCoverageAllRegions it(this);
  assert((*it)->isInfiniteRegion());
   
  CRegion* first = it++;
  CRegion* prev  = first;
  CRegion* actu  = NULL;

  std::deque< CRegion* > regionQueue;
         
  while ( it.cont() )
    {
      regionQueue.push_front( *it );
      ++it;   
    }
  std::sort< std::deque< CRegion* >::iterator >
    ( regionQueue.begin(), regionQueue.end(), orderRegionFirstPixel() );
   
  first->setFather(NULL); // pour les arbres UF
  first->setBrother(NULL); // précédent
  first->setNextSameCC(NULL); 
   
  while ( !regionQueue.empty() )
    {
      actu=regionQueue.front(); regionQueue.pop_front();
     
      prev->setFirstSon(actu); // suivant         
      actu->setBrother(prev);  // précédent
      
      actu->setFather(NULL);
      actu->setNextSameCC(NULL);   
        
      prev = actu;
    }
   
  prev->setFirstSon(NULL);
  first->setBrother(prev); 
}
//******************************************************************************
void CTopologicalMap::relabelDartsFromInclusionTree()
{
  assert( FUFTreeMode );
  
  CDynamicCoverageAllRegions it(this);
  assert((*it)->isInfiniteRegion());
   
  CRegion* first = it++;
  CRegion* prev  = first;
  CRegion* actu  = NULL;

  std::deque< CRegion* > regionQueue;
  std::deque< CRegion* > todeleteQueue;

  while ( it.cont() )
    {
      if ( isRegionRoot(*it) )
	regionQueue.push_front( *it );
      else
	todeleteQueue.push_front( *it );
      ++it;
    }

  std::sort< std::deque< CRegion* >::iterator >
    ( regionQueue.begin(), regionQueue.end(), orderRegionFirstPixel() );
  
  // On parcours les brins de la carte pour effectuer une mise à
  //    jour de leur étiquettage
  for (CDynamicCoverageAll It(this);It.cont();++It)
    {
      // Si l'arête est interne on l'a supprime.
      if ( getRegion(*It)==getRegion((*It)->getBeta2()) )
	{
	  getRegion(*It)->incBoundarySize
	    (-static_cast<CTopologicalDart*>(*It)->getEdge()->getNbLinels());
	  edgeRemoval(*It);
	}
      else
	// Si la région d'appartenance du brin n'est pas racine d'un arbre uf.
	if ( !isRegionRoot(*It) )
	  {
	    assert(getRegion(*It)->getFirstPixel() <
		   static_cast<CTopologicalDart*>(*It)->getRegion()
		   ->getFirstPixel());
	   
	    //on récupère la région avec laquelle il va falloir étiquetter
	    //la région courante
	    setRegion(*It, getRegion(*It));
	  }
      
      // Au passage, on détruit les arêtes.
      if ( static_cast<CTopologicalDart*>(*It)->getEdge()!=NULL )
	{
	  delete static_cast<CTopologicalDart*>(*It)->getEdge();
	  static_cast<CTopologicalDart*>(*It)->setEdge(NULL);
	  static_cast<CTopologicalDart*>((*It)->getBeta2())->setEdge(NULL);
	}
    }

  // On transforme l'arbre d'inclusion en liste de régions.
  first->setFather(NULL); // pour les arbres UF
  first->setBrother(NULL); // précédent
  first->setNextSameCC(NULL); 
   
  while ( !regionQueue.empty() )
    {
      actu=regionQueue.front(); regionQueue.pop_front();
       
      prev->setFirstSon(actu); // suivant         
      actu->setBrother(prev);  // précédent
       
      actu->setFather(NULL);
      actu->setNextSameCC(NULL);   
       
      prev = actu;
    }
   
  prev->setFirstSon(NULL);
  first->setBrother(prev);

  // On détruit les anciennes régions
  while ( !todeleteQueue.empty() )
    {
      actu = todeleteQueue.front();
      todeleteQueue.pop_front();

      //on détruit la région courante par appel au destructeur.
      //Avant de la détruire on remet les champs de la région à NULL
      //afin de ne pas supprimer l'arbre union
      actu->setFirstSon(NULL);
      actu->setBrother(NULL);
      actu->setNextSameCC(NULL);
      actu->setFather(NULL);
       
      delete actu;
       
      //on décrémente le nombre de régions de la carte
      --FNbRegions;
    }
}
//******************************************************************************
unsigned int CTopologicalMap::mergeRegions
(bool (CRegion::*AMethod)(CTopologicalMap*,CRegion*,int),
 int AThreshold, bool ASort)
{
  // Si on ne trie pas les arêtes, on les randomize.
  if ( !ASort )
    {
      std::cout<<"Randomize darts."<<std::endl;
      randomizeDarts();
    }
   
  //#ifdef DISPLAY_TIME_EXTRACT_IMAGE
  CChrono c;
  c.start();
  //#endif

  unsigned int nbMerged = 0;
  FMaxLg=0;

  // 1) On ordonne les arêtes par différence de couleur entre les 2 régions
  //    adjacentes
  std::deque< CTopologicalDart* > edgeQueue;
  for( CDynamicCoverageAll dIt(this); dIt.cont(); ++dIt )
    {
      assert( !isMarkedDeleted(*dIt) );
      static_cast<CTopologicalDart*>(*dIt)->getRegion()->setFather(NULL);
      if ( static_cast<CTopologicalDart*>(*dIt)->getEdge()==NULL )
	{
	  assert(static_cast<CTopologicalDart*>(*dIt)!=NULL);
	  edgeQueue.push_front( static_cast<CTopologicalDart*>(*dIt) );

	  // On crée des arêtes pour stocker la longueur de chaque frontière.
	  CEdge* edge = new CEdge;
	  static_cast<CTopologicalDart*>(*dIt)->setEdge( edge );
	  static_cast<CTopologicalDart*>((*dIt)->getBeta(2))->setEdge( edge );

	  unsigned int nblinels = 0;
	  for( CCoverageEdgeLinels tEdge( this, *dIt ) ; tEdge.cont() ; ++tEdge )
	    { ++nblinels; }
	  edge->setNbLinels(nblinels);
	  if ( nblinels>FMaxLg ) FMaxLg = nblinels;
	}
    }

  // On passe en mode UFTree
  FUFTreeMode = true;
  
  if ( ASort )
    std::sort< std::deque< CTopologicalDart* >::iterator >
      ( edgeQueue.begin(), edgeQueue.end(),
	orderEdgesInterval() );
  //	 orderEdgesLg());
       
  // 2) On traite chaque arête dans l'ordre croissant pour commencer à 
  //    fusionner les régions les plus proches.
  CTopologicalDart* currentDart = NULL; 
  CRegion* r1 = NULL;
  CRegion* r2 = NULL;
  while ( !edgeQueue.empty() )
    {
      currentDart = edgeQueue.front();
      edgeQueue.pop_front();
      assert( !isMarkedDeleted(currentDart) );
      
      r1 = getRegion(currentDart);
      r2 = getRegion(beta2(currentDart));
      if ( r1==r2 || (r1->*AMethod)(this,r2,AThreshold) )
	{      
	  if (r1!=r2)
	    {               
	      unionRegionRoot(r1,r2);
	      ++nbMerged;         
	    }
	
	  r1->incBoundarySize
	    (-static_cast<CTopologicalDart*>(currentDart)->getEdge()->getNbLinels());

	  delete static_cast<CTopologicalDart*>(currentDart)->getEdge();
	  static_cast<CTopologicalDart*>(currentDart)->setEdge(NULL);
	  static_cast<CTopologicalDart*>((currentDart)->getBeta2())->setEdge(NULL);

	  edgeRemoval(currentDart);
	}
    }
   
  // 3) On relabel les brins pour que les régions soient les racines des UFTree
  relabelDartsFromInclusionTree();
  // On sort du mode UFTree
  FUFTreeMode=false;

  // On simplifie la carte et on calcule l'arbre d'inclusion.
  simplifyMap();
  computeInclusionTree();      

  //#ifdef DISPLAY_TIME_EXTRACT_IMAGE
  c.stop();
  c.display("Fusion des régions");
  //#endif

  assert(isMapOk());

  return nbMerged;
}   
//******************************************************************************
unsigned int CTopologicalMap::segment(unsigned int AThreshold, bool ASort)
{
  return mergeRegions(&CRegion::canMergeWith,(int)AThreshold, ASort);
}
//******************************************************************************
unsigned int CTopologicalMap::segmentWithPonderation(unsigned int AThreshold, 
						     bool ASort)
{  
  return mergeRegions(&CRegion::canMergeWithPonderated, (int)AThreshold, ASort);
}
//******************************************************************************
unsigned int CTopologicalMap::removeSmallRegions(unsigned int AThreshold)
{
  return mergeRegions(&CRegion::smallRegion,(int)AThreshold);
}   
//******************************************************************************
unsigned int CTopologicalMap::segmentWithCavityConstraint
(unsigned int AThreshold, int ACavity, bool ASort)
{
  initializeCavityCount();
  unsigned int res = mergeRegionsWithEdgeConstraint
    (&CRegion::canMergeWith,(int)AThreshold,
     &CRegion::canMergeWithCavityConstraint,(int)ACavity, ASort);

  return res;
}
//******************************************************************************
unsigned int CTopologicalMap::mergeRegionsWithEdgeConstraint
(bool (CRegion::*AMethod)(CTopologicalMap*,CRegion*,int),
 int AThreshold, bool (CRegion::*AMethod2)(CTopologicalMap*,CDart*,int),
 int AConstraint, bool ASort)
{
  // Si on ne trie pas les arêtes, on les randomize.
  if ( !ASort )
    {
      randomizeDarts();
    }
   
  //#ifdef DISPLAY_TIME_EXTRACT_IMAGE
  CChrono c;
  c.start();
  //#endif

  unsigned int nbMerged = 0;
  FMaxLg=0;

  // 1) On ordonne les arêtes par différence de couleur entre les 2 régions
  //    adjacentes
  std::deque< CTopologicalDart* > edgeQueue;
  for( CDynamicCoverageAll dIt(this); dIt.cont(); ++dIt )
    {
      assert( !isMarkedDeleted(*dIt) );
      static_cast<CTopologicalDart*>(*dIt)->getRegion()->setFather(NULL);
      if ( static_cast<CTopologicalDart*>(*dIt)->getEdge()==NULL )
	{
	  assert(static_cast<CTopologicalDart*>(*dIt)!=NULL);
	  edgeQueue.push_front( static_cast<CTopologicalDart*>(*dIt) );

	  // On crée des arêtes pour stocker la longueur de chaque frontière.
	  CEdge* edge = new CEdge;
	  static_cast<CTopologicalDart*>(*dIt)->setEdge( edge );
	  static_cast<CTopologicalDart*>((*dIt)->getBeta(2))->setEdge( edge );

	  unsigned int nblinels = 0;
	  for( CCoverageEdgeLinels tEdge( this, *dIt ) ; tEdge.cont() ; ++tEdge )
	    { ++nblinels; }
	  edge->setNbLinels(nblinels);
	  if ( nblinels>FMaxLg ) FMaxLg = nblinels;
	}
    }

  // On passe en mode UFTree
  FUFTreeMode = true;
  
  if ( ASort )
    std::sort< std::deque< CTopologicalDart* >::iterator >
      ( edgeQueue.begin(), edgeQueue.end(),
	orderEdgesInterval() );
  //	 orderEdgesLg());
       
  // 2) On traite chaque arête dans l'ordre croissant pour commencer à 
  //    fusionner les régions les plus proches.
  CTopologicalDart* currentDart = NULL; 
  CRegion* r1 = NULL;
  CRegion* r2 = NULL;
  while ( !edgeQueue.empty() )
    {
      currentDart = edgeQueue.front();
      edgeQueue.pop_front();
      assert( !isMarkedDeleted(currentDart) );

      r1 = getRegion(currentDart);
      r2 = getRegion(beta2(currentDart));
      if ( r1==r2 || (r1->*AMethod)(this,r2,AThreshold) )
	{
	  if ((r1->*AMethod2)(this, currentDart, AConstraint))
	    {
	      if (r1!=r2)
		{
		  unionRegionRoot(r1,r2);
		  r1->setCavityCount(lastCavityCount());
		  ++nbMerged;
		}
	      r1->incBoundarySize
		(-static_cast<CTopologicalDart*>(currentDart)->getEdge()->getNbLinels());

	      delete static_cast<CTopologicalDart*>(currentDart)->getEdge();
	      static_cast<CTopologicalDart*>(currentDart)->setEdge(NULL);
	      static_cast<CTopologicalDart*>((currentDart)->getBeta2())->setEdge(NULL);

	      edgeRemoval(currentDart);
	    }
	}
    }
   
  // 3) On relabel les brins pour que les régions soient les racines des UFTree
  relabelDartsFromInclusionTree();
  // On sort du mode UFTree
  FUFTreeMode=false;
  
  // On simplifie la carte et on calcule l'arbre d'inclusion.
  simplifyMap();
  computeInclusionTree();      

  //#ifdef DISPLAY_TIME_EXTRACT_IMAGE
  c.stop();
  c.display("Fusion des régions");
  //#endif

  assert(isMapOk());

  return nbMerged;
}  
//******************************************************************************
void CTopologicalMap::initializeCavityCount()
{
  CRegion *rCur = NULL;
  unsigned int cavity = 0;
  for (CDynamicCoverageAllRegions rIt(this); rIt.cont(); ++rIt)
    {
      rCur = *rIt;
      cavity = 0;
      for (CDynamicCoverageCCSons cavityIt(rCur); cavityIt.cont(); ++cavityIt)
	{
	  cavity++;
	}
      rCur->setCavityCount(cavity);
    }
}
//******************************************************************************
void CTopologicalMap::setLastCavityCount(unsigned int ALastCavityCount)
{
  FLastCavityCount = ALastCavityCount;
}
//******************************************************************************
unsigned int CTopologicalMap::lastCavityCount() const
{
  return FLastCavityCount;
}
//******************************************************************************
