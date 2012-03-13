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
#ifdef DEFORMABLE_METHOD
//******************************************************************************
#include "inline-macro.hh"
#include "deformable-partition.hh"
#include "region.hh"
#include "dynamic-coverage.hh"
#include INCLUDE_NON_INLINE("deformable-partition.icc")

#include <vector>
#include <iostream>
#include <sstream>

#define EPSILON .003 // for numerical errors

//******************************************************************************
using namespace Map2d;
using namespace std;
using namespace ImaGene;
//******************************************************************************
bool CDeformablePartition::partitionMinimizeEnergyFirstGain(unsigned int AStop)
{
   if ( FEnergy==0 )
   {
      partitionInitializeEnergy();

      // Initialization of the first positive move of each region
      CDynamicCoverageRegions itr(FMap->getInclusionTreeRoot());
      for ( ++itr; itr.cont(); ++itr)
      {
         updateFirstGainForRegion(*itr);
      }
   }

   std::cout<<"Start partition minimize energy first gain: wregion="<<FOmegaRegion
	    <<", wcontour="<<FOmegaContour<<", wedge="<<FOmegaEdge<<std::endl;
   
   int i = 0;
   double oldEnergy = FEnergy;
   CRegion* regionToDeform = NULL;
   while( (regionToDeform = findFirstPositiveDeformation()) !=NULL && i<AStop)
   {
      applyFirstDeformation( regionToDeform );
      ++i;
      if( i % 100 == 0 )
      {
        std::cout << "["<<i<<"] "<<FEnergy << std::endl;

	  /*	  double energy = computeGlobalEnergy(true); // DEBUG to remove
	  if( energy > oldEnergy )
	    {
	      std::cout << "Error: non decreasing : " << energy <<" > "
			<< oldEnergy << std::endl;
	    }
	    oldEnergy = energy;*/
	  
//      std::cout << "Total energy : " << energy
//              << " (" << currentPriority <<")"
//              << std::endl;   

/*     if ( fabs(energy-FEnergy)>=EPSILON )
     {
        std::cout<<"ERROR: FEnergy="<<FEnergy<<"  oldEnergy="<<oldEnergy
        <<"  energy="<<energy<<std::endl;
	}*/
      }
   }

   std::cout<<"End energy: "<<FEnergy<<std::endl;

   computeGlobalEnergy(true);
   
   if ( regionToDeform==NULL )
     return false;
   
   return true;
   //   std::cout << "Fin déformation: " << pq.priority() << std::endl;
}
//******************************************************************************
void CDeformablePartition::partitionMinimizeEnergyBestGain()
{
   TPartitionQueue pq;
   
   if ( FEnergy==0 )
   {
      FIndex = FMap->getNewDirectInfo();
      assert( FIndex != -1 );
      
      partitionInitializeEnergy();
      //std::cout<<"after init";computeGlobalEnergy(); // DEBUG to remove
   }
   else if ( FIndex==-1 )
   {
      FIndex = FMap->getNewDirectInfo();
      assert( FIndex != -1 );
      for (CDynamicCoverageAll it(FMap); it.cont(); ++it)
         FMap->setDirectInfo( *it, FIndex, NULL );
   }
   
   putBestDeformationsInQueue(pq);
   //std::cout<<"after putBest...";computeGlobalEnergy(); // DEBUG to remove
   //std::cout.setf(ios::fixed,ios::floatfield);std::cout.precision(3);
   //std::cout<<"Init energy: "<<FEnergy<<std::endl;
   
   // if the priority (i.e. expected gain is strictly positive)
   int i = 0;
   double oldEnergy = FEnergy;
   double currentPriority = 0.0;
   if( ! pq.empty() )
   {
      /*if*/while( (currentPriority = pq.priority()) > EPSILON && i<10000)
      {
         //      std::cout << pq.priority() << " "
         //           << (pq.top())->getId() << std::endl;
         
         applyBestDeformation( pq, pq.top() );
         ++i;
         if( i % 100 == 0 )
            std::cout << "["<<i<<"] "<<FEnergy << std::endl;
         
         //  std::cout<<"partitionMinimizeEnergy1 ";
	 /*  double energy = computeGlobalEnergy(); // DEBUG to remove
         if( energy > oldEnergy )
         {
            std::cout << "Error: non decreasing energy with priority : "
            << currentPriority << std::endl;
         }
         //      std::cout << "Total energy : " << energy
         //              << " (" << currentPriority <<")"
         //              << std::endl;
         
         if (fabs(energy-FEnergy)>=EPSILON ||
            fabs(oldEnergy-currentPriority-FEnergy)>=EPSILON) // DEBUG
            {
               std::cout<<"ERROR: FEnergy="<<FEnergy<<"  oldEnergy="<<oldEnergy
               <<"  energy="<<energy<<std::endl;
            }
            
            oldEnergy = energy;
	 */
      }
   }
   
   //   std::cout << "Fin déformation: " << pq.priority() << std::endl;
   
   std::cout<<"End energy: "<<FEnergy<<std::endl;
   //deleteAllocatedElements();
}
//******************************************************************************
CDeformablePartition::~CDeformablePartition()
{ deleteAllocatedElements(); }
//******************************************************************************
double CDeformablePartition::partitionInitializeEnergy()
{
  std::cout<<"[BEGIN] CDeformablePartition::partitionInitializeEnergy"<<std::endl;
  FEnergy = 0.0;
  
  double contourenergie = 0.;
  double edgeenergie = 0.;
  for( CDynamicCoverageAll dIt(FMap); dIt.cont(); ++dIt )
    {
      if ( getContour(*dIt)==NULL )
        {
          CContour* contour = new CContour(*dIt);
          for( CDynamicCoverage1 dFace(FMap, *dIt); dFace.cont(); ++dFace )
            {
              setContour( *dFace, contour );
            }
          
          contourenergie += initializeContourEnergy(*dIt);
          
          /*#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
            if (contour->getDMLP()!=NULL)
            {
              std::cout<<"Size with DMLP: "<<contour->getDMLP()->getLength() << std::endl;
              std::cout<<"     with CMLP: "
                       <<ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(*dIt))
                       << std::endl;
            }
            #endif*/

          //std::cout<<"contour energie : "<<getContourEnergy(*dIt)<<std::endl;
        }
      if ( getEdge(*dIt)==NULL &&
           !FMap->getRegion(*dIt)->isInfiniteRegion() &&
           !FMap->getRegion((*dIt)->getBeta(2))->isInfiniteRegion() )
        {
          /*EDGEENERGY
            CEdge* edge = new CEdge;
            setEdge( *dIt, edge );
            setEdge( (*dIt)->getBeta(2), edge );            
            edgeenergie += initializeEdgeEnergy(*dIt);
          */
          // std::cout<<"edge energie : "<<getEdgeEnergy(*dIt)<<std::endl;
        }
      if ( FIndex!=-1 )
        FMap->setDirectInfo( *dIt, FIndex, NULL );
    }
  
  CDynamicCoverageRegions itr(FMap->getInclusionTreeRoot());
  for ( ++itr; itr.cont(); ++itr)
    {
      // std::cout<<"mse: "<<(*itr)->mse()<<"  "<<computeRegionEnergy(*itr)<<std::endl;
      FEnergy += getRegionEnergy(*itr);
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
      getContour((*itr)->getRepresentativeDart())->FExternalContour=true;
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
      // regionsToUpdate.insert(*itr);
      (*itr)->setDeformationGain(-1); // -1 for not initialized
     //std::cout<<"region energie : "<<getRegionEnergy(*itr)<<std::endl;
    }

  FCurrentRegion=NULL;
  
  FEnergy += contourenergie + edgeenergie;
  
  std::cout<<"Region energy (mse): " <<FEnergy;
  std::cout<<";  Contour energy (mlp): "<<contourenergie;
  std::cout<<";  Edge energy (gradient): "<<edgeenergie<<std::endl;
  
  std::cout<<"Total energy initialized: "<<FEnergy<<std::endl;
  
  std::cout<<"[END] CDeformablePartition::partitionInitializeEnergy"<<std::endl;
  
  return FEnergy;
}
//******************************************************************************
void CDeformablePartition::putBestDeformationsInQueue(TPartitionQueue& AQueue)
{
   assert(FIndex!=-1);
   
   double gain;
   TPartitionQueue::iterator pqIt;
   
   CDynamicCoverageRegions itr(FMap->getInclusionTreeRoot());
   for ( ++itr; itr.cont(); ++itr)
   {
      // Computing the best deformation on the region
      gain = updateMaximumGainForRegion(*itr);
      
      //      std::cout<<"Best deformation for region "<<itr->getId()<<" : "<<gain<<std::endl;
      
      // Insert the best deformation into the priority queue
      pqIt = AQueue.insert( *itr, gain );
      
      // Association of the dart with the value in the list
      if ( FIndex!=-1 )
         FMap->setDirectInfo( itr->getRepresentativeDart(), FIndex,
                              new TPartitionQueue::iterator(pqIt) );
   }
   
   //  std::cout << "Fin initialisation :" << AQueue.priority() << std::endl;
}
//******************************************************************************
CRegion* CDeformablePartition::findFirstPositiveDeformation()
{
   double gain;
   
   /*   CDynamicCoverageRegions itr(FMap->getInclusionTreeRoot());
   for ( ++itr; itr.cont(); ++itr)
   {
      if ( itr->getDeformationGain()>EPSILON )
         return *itr;
         }*/

   if ( FCurrentRegion!=NULL )
     {
       if ( FCurrentRegion->getDeformationGain()==-1 ) // uninitialized
         updateFirstGainForRegion(FCurrentRegion);
       
       if ( FCurrentRegion->getDeformationGain()>EPSILON )
         return FCurrentRegion;
     }
   
   
   CDynamicCoverageRegions itr(FMap->getInclusionTreeRoot());
   for ( ++itr; itr.cont(); ++itr)
   {
     if ( itr->getDeformationGain()==-1 ) // uninitialized
       updateFirstGainForRegion(*itr);
     
     if ( itr->getDeformationGain()>EPSILON )
       {
         FCurrentRegion=*itr;
         return *itr;
       }
   }

   /*   CRegion* cur = NULL;
   while ( !regionsToUpdate.empty() )
     {
       cur = *regionsToUpdate.begin();
       regionsToUpdate.erase(regionsToUpdate.begin());

       if (updateFirstGainForRegion(cur)>EPSILON )
         return cur;       
         }*/
   
   return NULL;
}
//******************************************************************************
void CDeformablePartition::deleteAllocatedElements()
{
   for( CDynamicCoverageAll dIt( FMap ) ; dIt.cont() ; ++dIt )
   {
      if ( getContour(*dIt)!=NULL )
      {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
         delete getContour(*dIt)->getDMLP();
         getContour(*dIt)->setDMLP(NULL);

         delete FMap->getRegion(*dIt)->getDMLPIterator();
         FMap->getRegion(*dIt)->setDMLPIterator(NULL);

         delete FMap->getRegion(*dIt)->getDMLPIterator2();
         FMap->getRegion(*dIt)->setDMLPIterator2(NULL);
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP

         delete getContour(*dIt);
         for( CDynamicCoverage1 dFace( FMap, *dIt ) ; dFace.cont() ; ++dFace )
         {
            setContour( *dFace, NULL );
         }
      }
      if ( getEdge(*dIt)!=NULL )
        {
          delete getEdge(*dIt);
          setEdge( *dIt, NULL );
          setEdge( (*dIt)->getBeta(2), NULL );
        }
      if ( FIndex!=-1 && FMap->getDirectInfo( *dIt, FIndex)!=NULL )
        {
          delete static_cast<TPartitionQueue::iterator*>
            (FMap->getDirectInfo( *dIt, FIndex));
          FMap->setDirectInfo( *dIt, FIndex, NULL );
        }
   }
   
   if ( FIndex!=-1 )
   {
      FMap->freeDirectInfo(FIndex);
      FIndex = -1;
   }
}
//******************************************************************************
double CDeformablePartition::computeGlobalEnergy(bool ATrace) const
{
  double regionenergy = 0;
  double contourenergy = 0.;
  double edgeenergy = 0.;

  int mark = FMap->getNewMark();

  if ( ATrace )
  {
     std::cout<<"[BEG]************************************************"<<std::endl;
     std::cout<<"FEnergy="<<FEnergy<<std::endl;
     std::cout<<"Id cur stocke CMLP"<<std::endl;
  }
  
  CDynamicCoverageRegions itr(FMap->getInclusionTreeRoot());
  for ( ++itr; itr.cont(); ++itr)
  {
     regionenergy += getRegionEnergy(*itr);
     if ( ATrace )
        std::cout<<itr->getFirstPixel()<<": "<<getRegionEnergy(*itr)
        <<"  "<<getRegionEnergy(*itr)<<std::endl;
  }
  
  for ( CDynamicCoverageAll dIt( FMap ) ; dIt.cont() ; ++dIt )
  {
     if ( ! FMap->getRegion( *dIt )->isInfiniteRegion() )
     {
        if ( !FMap->isMarked(*dIt,mark) )
        {
           contourenergy += computeContourEnergy(*dIt);
           for( CDynamicCoverage1 dFace( FMap, *dIt ) ; dFace.cont() ; ++dFace )
              FMap->setMark(*dFace,mark);
           
           if (ATrace)
             std::cout<<FMap->getRegion(*dIt)->getFirstPixel()
                      <<": "<<computeContourEnergy(*dIt)
                      <<"  "<<getContourEnergy(*dIt)
                      <<"  "<<FOmegaContour*computeMLPLengthGradient(*dIt)<<std::endl;
        }
	/*EDGEENERGY	if ( *dIt<(*dIt)->getBeta(2) ) // to compute the edge energy only once
	  { 
	    edgeenergy += computeEdgeEnergy(*dIt);
	    if (ATrace)
	      std::cout<<FMap->getRegion(*dIt)->getId()
                       <<": "<<computeEdgeEnergy(*dIt)
                       <<"  "<<getEdgeEnergy(*dIt)<<std::endl;
		       }*/
     }
     else
     {
        FMap->setMark(*dIt,mark);
     }
  }
  
  FMap->negateMaskMark(mark);
  assert( FMap->isWholeMapUnmarked(mark));
  FMap->freeMark(mark);

  if ( ATrace )
  {
    std::cout<<"Region energy (mse): "<<regionenergy;
    std::cout<<";  Contour energy (mlp): "<<contourenergy;
    std::cout<<";  Edge energy (gradient): "<<edgeenergy;   
    std::cout<<".  Total: "<<regionenergy+contourenergy+edgeenergy
	     <<";  Stored: "<<FEnergy<<std::endl;
     std::cout<<"[END]************************************************"
              <<std::endl;
  }

  return regionenergy+contourenergy+edgeenergy;
}
//******************************************************************************
double CDeformablePartition::computeEnergyGain( CDart * ADart,
                                                const CDoublet & ADoublet
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
                                                , GridCurve::Visitor& AIte
                                                , GridCurve::Visitor& AIte2
#endif
                                                )
{
  assert(ADart!=NULL);
  // assert(fabs(FEnergy-computeGlobalEnergy(true))<EPSILON);
  // std::cout<<"computeEnergyGain "<<ADoublet<<": "<<FEnergy<<std::endl;

  double initEnergy=FEnergy;
  
  if ( FMap->getRegion(ADart)->isInfiniteRegion()             ||
       FMap->getRegion(ADart->getBeta(2))->isInfiniteRegion() ||
       !FMap->deformationIsSimple(ADoublet) )
    return -1;
  
  CDart * dart = ADart;
  CDart * dart2 = FMap->beta2( ADart );

  assert(FMap->getRegion(dart)!=FMap->getRegion(dart2));
  assert(getContour(dart)!=getContour(dart2));

  // apply deformation on dEdge, with ADoublet
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
  double gain = deformationApply( dart, ADoublet, AIte, AIte2, true );
#else
  double gain = deformationApply( dart, ADoublet, true );
#endif

  /*  std::cout<<"initEnergy="<<initEnergy<<";  currentEnergy="<<FEnergy
      <<";  => gain="<<gain<<std::endl;*/
  
  /*if (fabs(FEnergy-gain-computeGlobalEnergy())>=EPSILON) // DEBUG TO REMOVE
  {
     std::cout<<"FEnergy="<<FEnergy<<"  gain="<<gain
              <<"  computeGlobalEnergy="<<computeGlobalEnergy(true)<<std::endl;
     std::cout<<"ERROR3 partitionComputeEnergyGain"<<std::endl;
     exit(-1);
  }
  assert(fabs(FEnergy-gain-computeGlobalEnergy())<EPSILON);*/
  
  // revert deformation
  deformationRevert();

  //  assert(fabs(FEnergy-computeGlobalEnergy())<EPSILON);
  
  /*if (fabs(FEnergy-computeGlobalEnergy())>=EPSILON) // DEBUG to remove
    {
       std::cout<<"ERROR4 partitionComputeEnergyGain "<<FEnergy<< " -- "
                << computeGlobalEnergy(true) <<std::endl;
      exit(-1);
    }
    assert(fabs(FEnergy-computeGlobalEnergy())<EPSILON);*/

  // std::cout<<"End computeEnergyGain\n";
  
  return gain;
}
//******************************************************************************
void CDeformablePartition::deformationRevert()
{
  assert( FRevertDart != NULL );
  assert( !FMap->getRegion(FRevertDart)->isInfiniteRegion() &&
          !FMap->getRegion(FRevertDart->getBeta(2))->isInfiniteRegion());
  assert( FMap->deformationIsSimple( FRevertDoublet ) );

  // std::cout<<"deformationRevert for "<<FRevertDoublet<<std::endl;

  CDart * dEdge = FRevertDart;
  CDart * dEdge2 = FMap->beta2( FRevertDart );

  CRegion * r1 = FMap->getRegion( dEdge ); // région du pixel X
  CRegion * r2 = FMap->getRegion( dEdge2 ); // région ou flipper le pixel R

  bool vertex1Moved = false;
  bool vertex2Moved = false;
  int linels = 0;

  // Mise à jour des informations associées aux régions
  r1->removePixel( FMap->getPixel( FMap->getPixelIn( FRevertDoublet ) ) );
  r2->addPixel( FMap->getPixel( FMap->getPixelIn( FRevertDoublet ) ) );

  // Mise à jour du plongement
  CDoublet tCur( FRevertDoublet );

  int jump = 0;

  CDoublet t1( tCur );
  t1.setPrevLinel();
  while( !FMap->isPCell( t1 ) && FMap->isLCell( t1 ) )
    {
      ++jump;
      t1.setNextPointel();
      t1.setPrevLinel();
    }
  t1.setNextLinel();
  // t1 est le premier lignel de clf( FRevertDoublet )

  CDoublet t2( tCur );
  do
    {
      ++jump;
      t2.setNextPointel();
      t2.setNextLinel();
    }
  while( !FMap->isPCell( t2 ) && FMap->isLCell( t2 ) );
  t2.setPrevLinel();
  // t2 est le dernier lignel de clf( FRevertDoublet ) et désigne le pointel de bord

  tCur = t1;
  // Suppression du chemin de lignel
  //EDGEENERGY  getEdge(dEdge)->restore();
  for( int i = 0 ; i < jump ; ++i )
    {
      FMap->setLCell( tCur, false );
      --linels;
      // Lignel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }

  // si le brin appartient à une boucle, il faut faire attention à
  // déplacer le brin convenablement.
  // autre solution, déplacer le brin sauvagement
  if ( FRevertDart == FMap->beta1(FRevertDart) && 
       FMap->beta2(FRevertDart) == FMap->beta21(FRevertDart)  )
    {
      t1.setPrevLinel();
      FMap->setDoublet( FRevertDart, t1 );
      t1.setNextLinel();
      do t1.setNextLinel();
      while( !FMap->isLCell( t1 ) );
      FMap->setDoublet( FMap->beta2(FRevertDart), t1 );
    }
  else
    {
      // si t1 est un pointel alors getDoublet( dEdge ) == t1
      if( FMap->isPCell( t1 ) )
        {
          assert( FMap->getDoublet( dEdge ) == t1 );
          t1.setPrevLinel();
          // Tant que t1 est un lignel alors on décale le pointel
          while( FMap->isLCell( t1 ) )
            {
              vertex1Moved = true;
              FMap->setPCell( t1, false );
              t1.setNextPointel();
              t1.setPrevLinel();
              FMap->setPCell( t1, true );
            }
          // Nouveau plongement
          FMap->setDoublet( dEdge, t1 );

          // Mise à jour du plongement des brins de l'orbite sommet
          // (si nécessaire)
          if( vertex1Moved )
            {
              CDart * dFirst = dEdge;
              dEdge = FMap->beta02( dEdge );
              // TODO remove the loop since we can removeLinel in dEdge
              // and add them in beta02(dEdge)
              while( dFirst != dEdge )
                {
                  // Si frontière OR => augmente (r2=R)
                  // Si frontière OX => diminue (r1=x)
                  //EDGEENERGY  if ( getEdge(dEdge)!=NULL ) getEdge(dEdge)->restore();
                  t1 = FMap->getDoublet(dEdge);
                  if ( FMap->getRegion(dEdge->getBeta(2))==r1 )
                    {
                      assert( FMap->getRegion(dEdge)!=r1 && 
                              FMap->getRegion(dEdge)!=r2 );
                      do
                        {
                          t1.setNextPointel();
                          t1 = FMap->getOtherLinel(t1);
                        }
                      while( !FMap->isPCell( t1 ) );		      
                    }
                  else
                    {
                      assert( FMap->getRegion(dEdge->getBeta(2))!=r2 );
                      assert( FMap->getRegion(dEdge)==r2 );
                      do
                        {
                          t1 = FMap->getOtherLinel(t1);
                          t1.setNextPointel();
                        }
                      while( !FMap->isPCell( t1 ) ); 
                    }		  
                  FMap->setDoublet( dEdge, t1 );
                  dEdge = FMap->beta02( dEdge );
                }
            }
        }
      // si t2 est un pointel alors getDoublet( dEdge2 ) == t2
      if( FMap->isPCell( t2 ) )
        {
          assert( FMap->getDoublet( dEdge2 ) == t2 );
          t2.setNextLinel();
          // Tant que t2 est un lignel alors on décale le pointel
          while( FMap->isLCell( t2 ) )
            {
              vertex2Moved = true;
              FMap->setPCell( t2, false );
              t2.setNextPointel();
              t2.setNextLinel();
              FMap->setPCell( t2, true );
            }
          // Nouveau plongement
          FMap->setDoublet( dEdge2, t2 );
          // Mise à jour du plongement des brins de l'orbite sommet
          // (si nécessaire)
          if( vertex2Moved )
            {
              CDart * dFirst = dEdge2;
              dEdge2 = FMap->beta02( dEdge2 );
              while( dFirst != dEdge2 )
                {
                  // Si frontière OR => augmente (r2=R)
                  // Si frontière OX => diminue (r1=X)
                  //EDGEENERGY if ( getEdge(dEdge2)!=NULL ) getEdge(dEdge2)->restore();
                  t2 = FMap->getDoublet(dEdge2);
                  if ( FMap->getRegion(dEdge2)==r1 )
                    {
                      assert( FMap->getRegion(dEdge2->getBeta(2))!=r1 &&
                              FMap->getRegion(dEdge2->getBeta(2))!=r2 );
                      do
                        {
                          t2.setNextPointel();
                          t2 = FMap->getOtherLinel(t2);
                        }
                      while( !FMap->isPCell( t2 ) );		      
                    }
                  else
                    {
                      assert( FMap->getRegion(dEdge2)!=r2 );
                      assert( FMap->getRegion(dEdge2->getBeta(2))==r2 );
                      do
                        {
                          t2 = FMap->getOtherLinel(t2);
                          t2.setNextPointel();
                        }
                      while( !FMap->isPCell( t2 ) ); 
                    }		  
                  FMap->setDoublet( dEdge2, t2 );
                  dEdge2 = FMap->beta02( dEdge2 );
                }
            }
        }
    }

  // On peut à présent remplir les lignels vides
  // Remplissage des lignels vides
  for( int i = jump ; i < 4 ; ++i )
    {
      if( !FMap->isLCell( tCur ) )
        {
          FMap->setLCell( tCur, true );
          ++linels;
        }
      // Lignel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
  getContour(dEdge)->getDMLP()->undoFlip();
  getContour(dEdge2)->getDMLP()->undoFlip();

  /*  std::cout<<"Size with DMLP: l1="<<getContour(dEdge)->getDMLP()->getLength()
          <<" l2="<<getContour(dEdge2)->getDMLP()->getLength() << std::endl;

  std::cout<<"          CMLP: l1="<<ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dEdge))
  <<" l2="<<ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dEdge2))<< std::endl;*/

  /*if ( getContour(dEdge)->getDMLP()->getLength()!=ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dEdge)))
    {
      std::cout<<"Freeman DMLP: "<<getContour(dEdge)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<"        CMLP: "<<freemanChainFromFace(dEdge)<<std::endl;
      
      exit(-1);
    }
  if ( getContour(dEdge2)->getDMLP()->getLength()!=ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dEdge2)))
    {
      std::cout<<"2 Freeman DMLP: "<<getContour(dEdge2)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<"          CMLP: "<<freemanChainFromFace(dEdge2)<<std::endl;
      
      exit(-1);
      }*/
  /*  if ( !(getContour(dEdge)->getDMLP()->getFreemanChain()==freemanChainFromFace(dEdge)) )
    {
      std::cout<<"PB1"<<std::endl;
      std::cout<<getContour(dEdge)->getDMLP()->getFreemanChain()<<std::endl
	       <<freemanChainFromFace(dEdge)<<std::endl;
      exit(-1);
    }
  if ( !(getContour(dEdge2)->getDMLP()->getFreemanChain()==freemanChainFromFace(dEdge2)) )
    {
      std::cout<<"PB1b"<<std::endl;
      std::cout<<getContour(dEdge2)->getDMLP()->getFreemanChain()<<std::endl
	       <<freemanChainFromFace(dEdge2)<<std::endl;
      exit(-1);
      }*/
#endif

  FRevertDart=NULL;
}
//******************************************************************************
double CDeformablePartition::deformationApply( CDart * ADart, 
                                               const CDoublet & ALinel,
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
                                               GridCurve::Visitor& AIte,
                                               GridCurve::Visitor& AIte2,
#endif
                                               bool ARevert )
{
  //  std::cout<<"deformationApply for "<<ALinel<<std::endl;

  assert( ADart != NULL );
  assert( !FMap->getRegion(ADart)->isInfiniteRegion() &&
          !FMap->getRegion(ADart->getBeta(2))->isInfiniteRegion());
  assert( FMap->deformationIsSimple( ALinel ) );

  if ( ARevert ) FRevertDart = ADart->getBeta(2);
  else FRevertDart = NULL;

  CDart * dEdge = ADart;
  CDart * dEdge2 = FMap->beta2( ADart );

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
  /*  assert( getContour(dEdge)!=getContour(dEdge2) );
  assert( getContour(dEdge)->getDMLP()!=getContour(dEdge2)->getDMLP() );
  */  
  /*  if ( !(getContour(dEdge)->getDMLP()->getFreemanChain()==freemanChainFromFace(dEdge)) )
    {
      std::cout<<"PB2a"<<std::endl;
      std::cout<<" C1 Map : "<<freemanChainFromFace(dEdge)<<std::endl;      
      std::cout<<" C1 DMLP: "<<getContour(dEdge)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<" C1 Edges:"<<*getContour(dEdge)->getDMLP()<<std::endl;
      exit(-1);
      }*/
  /*  else
    {
      std::cout<<"C1 Map  :"<<freemanChainFromFace(dEdge)<<std::endl;
      std::cout<<"C1 DMLP : "<<getContour(dEdge)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<"C1 Edges:"<<*getContour(dEdge)->getDMLP()<<std::endl;
      }*/
  /*  if ( !(getContour(dEdge2)->getDMLP()->getFreemanChain()==freemanChainFromFace(dEdge2)) )
    {
      std::cout<<"PB2b"<<std::endl;
      std::cout<<" C2 Map : "<<freemanChainFromFace(dEdge2)<<std::endl;
      std::cout<<" C2 DMLP: "<<getContour(dEdge2)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<" C2 Edges:"<<*getContour(dEdge)->getDMLP()<<std::endl;
      exit(-1);
      }*/
  /*  else
    {
      std::cout<<"C2 Map :"<<freemanChainFromFace(dEdge2)<<std::endl;
      std::cout<<"C2 DMLP: "<<getContour(dEdge2)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<"C2 Edges:"<<*getContour(dEdge2)->getDMLP()<<std::endl;
      }*/
#endif
  
  CRegion * r1 = FMap->getRegion( dEdge ); // région du pixel X
  CRegion * r2 = FMap->getRegion( dEdge2 ); // région ou flipper le pixel R

  bool vertex1Moved = false;
  bool vertex2Moved = false;
  int linels = 0;

  double delta = getRegionEnergy(r1)+getRegionEnergy(r2);

  // Mise à jour des informations associées aux régions
  r1->removePixel( FMap->getPixel( FMap->getPixelIn( ALinel ) ) );
  r2->addPixel( FMap->getPixel( FMap->getPixelIn( ALinel ) ) );

  delta -= (getRegionEnergy(r1)+getRegionEnergy(r2));

  delta += getContourEnergy(dEdge) + getContourEnergy(dEdge2);

  //  std::cout<<"Before flip: "<<computeContourEnergy(ADart)<<"  "<<computeContourEnergy(ADart->getBeta(2))<<std::endl;
      
  // Mise à jour du plongement
  CDoublet tCur( ALinel );

  int jump = 0;

  CDoublet t1( tCur );
  t1.setPrevLinel();
  while( !FMap->isPCell( t1 ) && FMap->isLCell( t1 ) )
    {
      ++jump;
      t1.setNextPointel();
      t1.setPrevLinel();
    }
  t1.setNextLinel();
  // t1 est le premier lignel de clf( ALinel )

  CDoublet t2( tCur );
  do
    {
      ++jump;
      t2.setNextPointel();
      t2.setNextLinel();
    }
  while( !FMap->isPCell( t2 ) && FMap->isLCell( t2 ) );
  t2.setPrevLinel();
  // t2 est le dernier lignel de clf( ALinel ) et désigne le pointel de bord

  tCur = t1;
  // Suppression du chemin de lignel
  //EDGEENERGY getEdge(dEdge)->save();
  for( int i = 0 ; i < jump ; ++i )
    {
      FMap->setLCell( tCur, false );
      //EDGEENERGY removeLinelInEdge( dEdge, tCur );
      --linels;
      // Lignel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }

  // si le brin appartient à une boucle, il faut faire attention à
  // déplacer le brin convenablement.
  // autre solution, déplacer le brin sauvagement
  if ( ADart == FMap->beta1(ADart) && 
       FMap->beta2(ADart) == FMap->beta21(ADart)  )
    {
      t1.setPrevLinel();
      FMap->setDoublet( ADart, t1 );
      t1.setNextLinel();
      do t1.setNextLinel();
      while( !FMap->isLCell( t1 ) );
      FMap->setDoublet( FMap->beta2(ADart), t1 );
    }
  else
    {
      // si t1 est un pointel alors getDoublet( dEdge ) == t1
      if( FMap->isPCell( t1 ) )
        {
          assert( FMap->getDoublet( dEdge ) == t1 );
          t1.setPrevLinel();
          // Tant que t1 est un lignel alors on décale le pointel
          while( FMap->isLCell( t1 ) )
            {
              vertex1Moved = true;
              FMap->setPCell( t1, false );
              t1.setNextPointel();
              t1.setPrevLinel();
              FMap->setPCell( t1, true );
            }
          // Nouveau plongement
          FMap->setDoublet( dEdge, t1 );

          // Mise à jour du plongement des brins de l'orbite sommet
          // (si nécessaire)
          if( vertex1Moved )
            {
              CDart * dFirst = dEdge;
              dEdge = FMap->beta02( dEdge );
              // TODO remove the loop since we can removeLinel in dEdge
              // and add them in beta02(dEdge)
              while( dFirst != dEdge )
                {
                  // Si frontière OR => augmente (r2=R)
                  // Si frontière OX => diminue (r1=x)
                  //EDGEENERGY if ( getEdge(dEdge)!=NULL ) getEdge(dEdge)->save();
                  t1 = FMap->getDoublet(dEdge);
                  if ( FMap->getRegion(dEdge->getBeta(2))==r1 )
                    {
                      assert( FMap->getRegion(dEdge)!=r1 && 
                              FMap->getRegion(dEdge)!=r2 );
                      do
                        {
                          t1.setNextPointel();
                          t1 = FMap->getOtherLinel(t1);
                          /*EDGEENERGY if ( getEdge(dEdge)!=NULL ) 
                            removeLinelInEdge( dEdge, t1 );*/
                        }
                      while( !FMap->isPCell( t1 ) );		      
                    }
                  else
                    {
                      assert( FMap->getRegion(dEdge->getBeta(2))!=r2 );
                      assert( FMap->getRegion(dEdge)==r2 );
                      do
                        {
                          t1 = FMap->getOtherLinel(t1);
                          t1.setNextPointel();
                          /*EDGEENERGY if ( getEdge(dEdge)!=NULL )
                            addLinelInEdge( dEdge, t1 );*/
                        }
                      while( !FMap->isPCell( t1 ) ); 
                    }		  
                  FMap->setDoublet( dEdge, t1 );
                  /*EDGEENERGY		  if ( getEdge(dEdge)!=NULL )
                    delta += getSavedEdgeEnergy(dEdge) - getEdgeEnergy(dEdge);*/		  
                  dEdge = FMap->beta02( dEdge );
                }
            }
        }
      // si t2 est un pointel alors getDoublet( dEdge2 ) == t2
      if( FMap->isPCell( t2 ) )
        {
          assert( FMap->getDoublet( dEdge2 ) == t2 );
          t2.setNextLinel();
          // Tant que t2 est un lignel alors on décale le pointel
          while( FMap->isLCell( t2 ) )
            {
              vertex2Moved = true;
              FMap->setPCell( t2, false );
              t2.setNextPointel();
              t2.setNextLinel();
              FMap->setPCell( t2, true );
            }
          // Nouveau plongement
          FMap->setDoublet( dEdge2, t2 );
          // Mise à jour du plongement des brins de l'orbite sommet
          // (si nécessaire)
          if( vertex2Moved )
            {
              CDart * dFirst = dEdge2;
              dEdge2 = FMap->beta02( dEdge2 );
              while( dFirst != dEdge2 )
                {
                  // Si frontière OR => augmente (r2=R)
                  // Si frontière OX => diminue (r1=X)
                  //EDGEENERGY if ( getEdge(dEdge2)!=NULL ) getEdge(dEdge2)->save();
                  t2 = FMap->getDoublet(dEdge2);
                  if ( FMap->getRegion(dEdge2)==r1 )
                    {
                      assert( FMap->getRegion(dEdge2->getBeta(2))!=r1 &&
                              FMap->getRegion(dEdge2->getBeta(2))!=r2 );
                      do
                        {
                          t2.setNextPointel();
                          t2 = FMap->getOtherLinel(t2);
                          /*EDGEENERGY if ( getEdge(dEdge2)!=NULL )
                            removeLinelInEdge( dEdge2, t2 );*/
                        }
                      while( !FMap->isPCell( t2 ) );		      
                    }
                  else
                    {
                      assert( FMap->getRegion(dEdge2)!=r2 );
                      assert( FMap->getRegion(dEdge2->getBeta(2))==r2 );
                      do
                        {
                          t2 = FMap->getOtherLinel(t2);
                          t2.setNextPointel();
                          /*EDGEENERGY if ( getEdge(dEdge2)!=NULL )
                            addLinelInEdge( dEdge2, t2 );*/
                        }
                      while( !FMap->isPCell( t2 ) ); 
                    }		  
                  FMap->setDoublet( dEdge2, t2 );
                  /*EDGEENERGY		  if ( getEdge(dEdge2)!=NULL )
                    delta += getSavedEdgeEnergy(dEdge2) - getEdgeEnergy(dEdge2);*/
                  dEdge2 = FMap->beta02( dEdge2 );
                }
            }
        }
    }

  // On peut à présent remplir les lignels vides
  // Remplissage des lignels vides
  for( int i = jump ; i < 4 ; ++i )
    {
      if( !FMap->isLCell( tCur ) )
        {
          if ( ARevert )
            {
              FRevertDoublet = tCur;
            }
          FMap->setLCell( tCur, true );
          //EDGEENERGY addLinelInEdge( ADart, tCur );
          ++linels;
        }
      // Lignel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }
  //EDGEENERGY  delta += getSavedEdgeEnergy(ADart) - getEdgeEnergy(ADart);

  // Mise à jour des informations associées aux contour,
  // uniquement dans le cas DMPL
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
  // Flip the first DMLP.
  getContour(ADart)->getDMLP()->
    flipAndSimplify(AIte, false/*getContour(ADart)->FExternalContour*/, ARevert);

  // Flip the second one.
  getContour(dEdge2)->getDMLP()->
    flipAndSimplify(AIte2, true/*!getContour(dEdge2)->FExternalContour*/, ARevert);

  /*  std::cout<<"Size with DMLP: l1="<<getContour(dEdge)->getDMLP()->getLength()
           <<" l2="<<getContour(dEdge2)->getDMLP()->getLength() << std::endl;
  
  std::cout<<"          CMLP: l1="<<ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dEdge))
           <<" l2="<<ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dEdge2))<< std::endl;

  std::cout<<"          CMLP ponderated l1= "<<computeMLPLengthGradient(dEdge)
  <<" l2="<<computeMLPLengthGradient(dEdge2)<< std::endl;*/

  /*  if ( computeMLPLengthGradient(dEdge)!=getContour(dEdge)->getDMLP()->getLength() ) 
    {
      std::cout<<"PB3a"<<std::endl;
      std::cout<<"Freeman DMLP: "<<getContour(dEdge)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<"        CMLP: "<<freemanChainFromFace(dEdge)<<std::endl;
      std::cout<<"L1="<<computeMLPLengthGradient(dEdge)<<";  L2="
               <<getContour(dEdge)->getDMLP()->getLength()<<std::endl;
    }
  if ( computeMLPLengthGradient(dEdge2)!=getContour(dEdge2)->getDMLP()->getLength() ) 
    {
      std::cout<<"PB3b"<<std::endl;
      std::cout<<"Freeman DMLP: "<<getContour(dEdge2)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<"        CMLP: "<<freemanChainFromFace(dEdge2)<<std::endl;
      std::cout<<"L1="<<computeMLPLengthGradient(dEdge2)<<";  L2="
               <<getContour(dEdge2)->getDMLP()->getLength()<<std::endl;
               }*/

  /*
  if ( !(getContour(ADart)->getDMLP()->getFreemanChain()==freemanChainFromFace(ADart)) )
    {
      std::cout<<"PB3c pour le flip de "<<ALinel<<std::endl;
      std::cout<<" C1 Map : "<<freemanChainFromFace(ADart)<<std::endl;
      std::cout<<" C1 DMLP: "<<getContour(ADart)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<" C1 Edges:"<<*getContour(ADart)->getDMLP()<<std::endl;
      exit(-1);
    }
  if ( !(getContour(dEdge2)->getDMLP()->getFreemanChain()==freemanChainFromFace(dEdge2)) )
    {
      std::cout<<"PB3d pour le flip de "<<ALinel<<std::endl;
      std::cout<<" C2 Map : "<<freemanChainFromFace(dEdge2)<<std::endl;
      std::cout<<" C2 DMLP: "<<getContour(dEdge2)->getDMLP()->getFreemanChain()<<std::endl;
      std::cout<<" C2 Edges:"<<*getContour(dEdge2)->getDMLP()<<std::endl;      
      exit(-1);
      }*/
  
  /*if ( std::abs(getContour(dEdge2)->getDMLP()->getLength()-
		ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dEdge2)))>1 )
    {
    std::cout<<"2 Freeman DMLP: "<<getContour(dEdge2)->getDMLP()->getFreemanChain()<<std::endl;
    std::cout<<"          CMLP: "<<freemanChainFromFace(dEdge2)<<std::endl;
      
    exit(-1);
    }*/
#endif

  if ( !ARevert )
    {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
      delta -= computeContourEnergy(ADart);
      delta -= computeContourEnergy(ADart->getBeta(2));
      getContour(ADart)->setLengthMLP(getContour(ADart)->getDMLP()->getLength());
      getContour(ADart->getBeta(2))->setLengthMLP(getContour(ADart->getBeta(2))->getDMLP()->getLength());
#else
      delta -= initializeContourEnergy(ADart);
      delta -= initializeContourEnergy(ADart->getBeta(2));
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
    }
  else
    {
      delta -= computeContourEnergy(ADart);
      delta -= computeContourEnergy(ADart->getBeta(2));
    }

  return delta;
}
//******************************************************************************
double CDeformablePartition::updateFirstGainForRegion( CRegion* ARegion )
{
   if ( ARegion->isInfiniteRegion() ) return -1;

   double gain = -1;

   CDart* initDart = ARegion->getRepresentativeDart();
   CDynamicCoverageCCSons it(ARegion);
   bool needInit = true;
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
   ImaGene::GridCurve::Visitor itmlp=getContour(initDart)->getDMLP()->begin();
   ImaGene::GridCurve::Visitor itmlp2=itmlp;
#endif
   
   while ( initDart!=NULL )
   {
     needInit = true;
     
     for (CDynamicCoverage1 it2(FMap,initDart);it2.cont(); ++it2)
     {
       if ( !FMap->getRegion((*it2)->getBeta(2))->isInfiniteRegion() )
       {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
         if ( needInit )
         {
           itmlp = getContour(*it2)->getDMLP()->
             findPointel(FMap->getDoublet(*it2).getX(),
                         FMap->getDoublet(*it2).getY());
           assert ((*itmlp).first.x()==FMap->getDoublet(*it2).getX() &&
                   (*itmlp).first.y()==FMap->getDoublet(*it2).getY());
           // assert ((*itmlp).second=='0'+FMap->getDoublet(initDart).getLinel() );
         }
            
         CDoublet doublet2 = FMap->getDoublet(*it2).getNextPointel();
         itmlp2 = getContour((*it2)->getBeta(2))->
           getDMLP()->findPointel( doublet2.getX(), doublet2.getY() );
         assert ((*itmlp2).first.x()==doublet2.getX() &&
                 (*itmlp2).first.y()==doublet2.getY());
         // assert ((*itmlp2).second=='0'+doublet2.getLinel() );
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP

         //  std::cout<<"*** New edge***"<<std::endl;
            
         for ( CCoverageEdgeLinels itLinels(FMap,*it2); itLinels.cont();
               ++itLinels )
         {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
           // TEMP TO REMOVE
           doublet2 = (*itLinels).getNextPointel();
           /*
             std::cout<<"C1: "<<(*itLinels)<<"  "<<itmlp<<";   ";
             std::cout<<"C2: "<<doublet2<<"  "<<itmlp2<<";   "<<std::endl;
           */
           /* if ( !((getContour(*it2)->getDMLP()->
                    findPointel((*itLinels).getX(),
                                (*itLinels).getY())==itmlp ) ) )
           {
             std::cout<<"Contour C1 : "<<freemanChainFromFace(*it2)<<std::endl;
           }
           if ( !(getContour((*it2)->getBeta(2))->getDMLP()->
                    findPointel(doublet2.getX(),
                                doublet2.getY())==itmlp2 ))
           {
             std::cout<<"Contour C2 : "<<freemanChainFromFace((*it2)->getBeta(2))<<std::endl;
             }*/
           
           assert( (getContour(*it2)->getDMLP()->
                    findPointel((*itLinels).getX(),
                                (*itLinels).getY())==itmlp ) );
           assert( (getContour((*it2)->getBeta(2))->getDMLP()->
                    findPointel(doublet2.getX(),
                                doublet2.getY())==itmlp2 ));
           /* itmlp = getContour(initDart)->getDMLP()->
              findPointel((*itLinels).getX(),
              (*itLinels).getY());*/

           /* itmlp2 = getContour((*it2)->getBeta(2))->
              getDMLP()->findPointel( doublet2.getX(), doublet2.getY() );
           */
                
           gain = computeEnergyGain(*it2,*itLinels,itmlp, itmlp2);
#else
           gain = computeEnergyGain(*it2,*itLinels);
#endif //DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
            
           if ( gain > EPSILON )
           {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
             /*              std::cout<<"updateFirstGainForRegion "<<ARegion->getFirstPixel()<<": "
                             <<gain<<std::endl;
                             std::cout<<"L1="<<getContour(*it2)->getDMLP()->getLength()<<"  "
                             <<"L2="<<getContour((*it2)->getBeta(2))->getDMLP()->getLength()<<std::endl;
             */
#endif //DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
             ARegion->setDeformationDart(*it2);
             ARegion->setDeformationDoublet(*itLinels);
             ARegion->setDeformationGain(gain);
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
             *(ARegion->getDMLPIterator())=itmlp;
             *(ARegion->getDMLPIterator2())=itmlp2;
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
                    
             /* std::cout<<"updateFirstGainForRegion "<<ARegion->getId()<<": "
                <<gain<<std::endl;*/
	       
             return gain;
           }
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
           --itmlp2;
           ++itmlp;
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
         }
       }
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
       else
       {
         needInit = true;
       }
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
     }
     if ( it.cont() )
     {
       initDart = (*it)->getRepresentativeDart()->getBeta(2);
       ++it;
     }
     else initDart = NULL;
   }

   ARegion->setDeformationDart(NULL);
   ARegion->setDeformationGain(0); // -1 if no initialized, 0 if no gain

   /*    std::cout<<"updateFirstGainForRegion "<<ARegion->getFirstPixel()<<": "
         <<"NULL"<<std::endl;*/

   return -1;
}
//******************************************************************************
double CDeformablePartition::updateMaximumGainForRegion( CRegion* ARegion )
{
   if ( ARegion->isInfiniteRegion() ) return -1;
   
   double maxgain = -1.0;
   double gain = 0.0;
   
   ARegion->setDeformationDart(NULL);
   ARegion->setDeformationGain(-1);
   
   CDart* initDart = ARegion->getRepresentativeDart();
   CDynamicCoverageCCSons it(ARegion);
   
   while ( initDart!=NULL )
   {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
      ImaGene::GridCurve::Visitor itmlp =
              getContour(initDart)->getDMLP()->
              findPointel(FMap->getDoublet(initDart).getX(),
                          FMap->getDoublet(initDart).getY());
      assert ((*itmlp).first.x()==FMap->getDoublet(initDart).getX() &&
              (*itmlp).first.y()==FMap->getDoublet(initDart).getY());
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
      
      for (CDynamicCoverage1 it2(FMap,initDart); it2.cont(); ++it2)
      {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
        CDoublet doublet2 = FMap->getDoublet(*it2).getNextPointel();
        ImaGene::GridCurve::Visitor itmlp2 = getContour((*it2)->getBeta(2))->
          getDMLP()->findPointel( doublet2.getX(), doublet2.getY() );
        assert ((*itmlp2).first.x()==doublet2.getX() &&
                (*itmlp2).first.y()==doublet2.getY());
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP

        for ( CCoverageEdgeLinels itLinels(FMap,*it2);
               itLinels.cont(); ++itLinels)
         {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
           gain = computeEnergyGain(*it2,*itLinels,itmlp, itmlp2);
#else
         gain = computeEnergyGain(*it2,*itLinels);
#endif //DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
         if ( gain>maxgain )
            {
               ARegion->setDeformationDart(*it2);
               ARegion->setDeformationDoublet(*itLinels);
               ARegion->setDeformationGain(gain);
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
               *(ARegion->getDMLPIterator())=itmlp;
               *(ARegion->getDMLPIterator2())=itmlp2;
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
               maxgain  = gain;
            }
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
            ++itmlp;
            --itmlp;
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
         }
      }
      if ( it.cont() )
      {
         initDart = (*it)->getRepresentativeDart()->getBeta(2);
         ++it;
      }
      else initDart = NULL;
   }

   return maxgain;
}
//******************************************************************************
bool CDeformablePartition::firstDeformationValid( CRegion* ARegion,
                                                  CRegion* ARegion2,
                                                  CRegion* ARegion3 )
{
   assert( ARegion != NULL && ARegion2!=NULL && ARegion3!=NULL );
   assert( ARegion!=ARegion2 && ARegion!=ARegion3 );
   assert( !ARegion->isInfiniteRegion() );

   if ( ARegion->getDeformationDart()==NULL ||
        ARegion->getDeformationGain()==-1 ) return false;
   if ( FMap->getRegion(ARegion->getDeformationDart()->getBeta2())==ARegion2 ||
        FMap->getRegion(ARegion->getDeformationDart()->getBeta2())==ARegion3 )
      return false;

   /*   if ( !FMap->isLCell(ARegion->getDeformationDoublet()) )
      std::cout<<"not is LCell "<<ARegion->getFirstPixel()<<" : "<<ARegion->getDeformationDoublet();
   */
   
   /*   if ( !FMap->deformationIsSimple(ARegion->getDeformationDoublet()) )
      std::cout<<"not simple "<<ARegion->getId()<<" : "<<ARegion->getDeformationDoublet();

   if ( fabs(computeEnergyGain(ARegion->getDeformationDart(),
        ARegion->getDeformationDoublet()) -
        ARegion->getDeformationGain())>EPSILON )
   {
      std::cout<<"ARegion: "<<ARegion->getId()
      <<" ARegion2:"<<ARegion2->getId()<<" ARegion3:"<<ARegion3->getId()
      <<"gain "<<ARegion->getDeformationGain()
      <<"  gain computed"<<computeEnergyGain(ARegion->getDeformationDart(),
                                             ARegion->getDeformationDoublet())
      <<std::endl;
   }*/
   
   assert( FMap->isLCell(ARegion->getDeformationDoublet()) );
   
#if DEFORMABLE_METHOD!=CONTOUR_ENERGY_DMLP
   // Because for DMLP computeEnergyGain needs an iterator in parameter
   // assert( !FMap->deformationIsSimple(ARegion->getDeformationDoublet()) ||
   //         computeEnergyGain(ARegion->getDeformationDart(),
   //                           ARegion->getDeformationDoublet())>EPSILON );
#endif

   return FMap->deformationIsSimple(ARegion->getDeformationDoublet());
}
//******************************************************************************
void CDeformablePartition::applyFirstDeformation( CRegion* ARegion )
{
  assert( ARegion != NULL );
  assert( FMap->isLCell(ARegion->getDeformationDoublet()) );
  assert( FMap->deformationIsSimple(ARegion->getDeformationDoublet()) );

/*  std::cout<<"[BEGIN] applyFirstDeformation  region: "
      <<ARegion->getDeformationDoublet()<<std::endl;*/

  CDart* dart      = ARegion->getDeformationDart();
  CDart* dart2     = dart->getBeta(2);
  CRegion* region2 = FMap->getRegion(dart2);
  
  //std::cout<<"applyFirstDeformation: "<<ARegion->getDeformationGain()<<std::endl;
  
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
  
  FEnergy -= deformationApply( dart, ARegion->getDeformationDoublet(),
                               *(ARegion->getDMLPIterator()),
                               *(ARegion->getDMLPIterator2()),
                               false );
  /*  std::cout<<"Size with DMLP: "<<getContour(dart)->getDMLP()->getLength()
           <<"; with CMLP: " <<ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dart))
           <<"; with CMLP-wt: " <<computeMLPLengthGradient(dart)
           << std::endl;

  std::cout<<"Size2 with DMLP: "<<getContour(dart2)->getDMLP()->getLength()
           <<"; with CMLP: "<<ImaGene::FreemanChain::lengthMLP(freemanChainFromFace(dart2))
           <<"; with CMLP-wt: " <<computeMLPLengthGradient(dart2)
           << std::endl;
  */
  // computeGlobalEnergy(true);
  
#else
  FEnergy -= deformationApply( dart, ARegion->getDeformationDoublet(), false );
#endif

  //assert( fabs(FEnergy-computeGlobalEnergy(true))<EPSILON );
  /*
  std::cout<<"AVANT DEFORMATION FEnergy="<<FEnergy;
  std::cout<<"  gain="<<ARegion->getDeformationGain()
           <<"  computeGlobalEnergy="<<computeGlobalEnergy(true)<<std::endl;
  */
  /*if (fabs(FEnergy-computeGlobalEnergy())>=EPSILON) // DEBUG TO REMOVE
  {
     std::cout<<"FEnergy="<<FEnergy<<"  gain="<<ARegion->getDeformationGain()
              <<"  computeGlobalEnergy="<<computeGlobalEnergy(true)<<std::endl;
     std::cout<<"ERROR applyFirstDeformation"<<std::endl;
     exit(-1);
  }
  assert(fabs(FEnergy-computeGlobalEnergy())<EPSILON);*/

  /*#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
   // apply the deformation on the first contour
   getContour(dart)->getDMLP()->
           flipAndSimplify(*ARegion->getDMLPIterator(),
			   getContour(dart)->FExternalContour, false);
   
   // apply deformation on the second DMLP, we need to compute an iterator
   CDoublet doublet2 = ARegion->getDeformationDoublet().getNextPointel();
   GridCurve::Visitor itmlp2 = getContour(dart2)->getDMLP()->
           findPointel( doublet2.getX(), doublet2.getY() );
   getContour(dart2)->getDMLP()->
           flipAndSimplify(itmlp2,
			   !getContour(dart2)->FExternalContour,
			   false); 

   if ( !(getContour(dart)->getDMLP()->getFreemanChain()==freemanChainFromFace(dart)) )
     {
       std::cout<<"PB4"<<std::endl;
       std::cout<<getContour(dart)->getDMLP()->getFreemanChain()<<std::endl
		<<freemanChainFromFace(dart)<<std::endl;
       exit(-1);
     }
   if ( !(getContour(dart2)->getDMLP()->getFreemanChain()==freemanChainFromFace(dart2)) )
     {
       std::cout<<"PB4b"<<std::endl;
       std::cout<<getContour(dart2)->getDMLP()->getFreemanChain()<<std::endl
		<<freemanChainFromFace(dart2)<<std::endl;
       exit(-1);
     }
     #endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP */

  //   regionsToUpdate.insert(ARegion);
  //   regionsToUpdate.insert(region2);
  ARegion->setDeformationGain(-1); // -1 for not initialized
  region2->setDeformationGain(-1); // -1 for not initialized
        
   //std::cout<<"Region:"<<ARegion->getId()<<"   region2:"<<region2->getId()<<std::endl;
   
   {
   for ( CDynamicCoverageNeighbors itr(FMap,ARegion); itr.cont(); ++itr)
   {
      //std::cout<<"n1:"<<itr->getId()<<" "<<std::flush;
      if ( !itr->isInfiniteRegion() && (*itr)!=region2 &&
           !firstDeformationValid(*itr,ARegion,region2) )
        (*itr)->setDeformationGain(-1); // -1 for not initialized
        //  regionsToUpdate.insert(*itr);
      //std::cout<<std::endl;
   }
   }
   {
   for ( CDynamicCoverageNeighbors itr(FMap,region2); itr.cont(); ++itr)
   {
      //std::cout<<"n2:"<<itr->getId()<<" "<<std::flush;
      if ( !itr->isInfiniteRegion() && (*itr)!=ARegion &&
           !firstDeformationValid(*itr,ARegion,region2) )
        (*itr)->setDeformationGain(-1); // -1 for not initialized
      // regionsToUpdate.insert(*itr);
      //std::cout<<std::endl;
   }
   }   
  
   /*  for (std::set<CRegion*>::iterator it=regionsToUpdate.begin();
       it!=regionsToUpdate.end(); ++it)
  {
     updateFirstGainForRegion(*it);
     }*/
  
  //  std::cout<<"[END] applyFirstDeformation "<<std::endl;
}
//******************************************************************************
void CDeformablePartition::applyBestDeformation( TPartitionQueue & APq,
                                                 CRegion* ARegion )
{
  assert( ARegion != NULL );
  assert( FMap->isLCell(ARegion->getDeformationDoublet()) );
  assert( FMap->deformationIsSimple(ARegion->getDeformationDoublet()) );

//   std::cout<<"[BEGIN] applyBestDeformation "<<APq.priority()<<" region: "
//       <<ARegion->getId()<<" : "<<ARegion->getDeformationDoublet()<<std::endl;

  CDoublet tRevert;

  FEnergy -= APq.priority();
  
  // Apply the best deformation of the given region
  int vertexMoved =
    FMap->deformationApply( ARegion->getDeformationDart(),
             ARegion->getDeformationDoublet(),
             tRevert );

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
   // apply the deformation on the first contour
  getContour(ARegion->getDeformationDart())->getDMLP()->
    flipAndSimplify(*ARegion->getDMLPIterator(),
                    false/*getContour(ARegion->getDeformationDart())->FExternalContour*/,
                    false);

   // apply deformation on the second DMLP, we need to compute an iterator
  getContour(ARegion->getDeformationDart()->getBeta(2))->getDMLP()->
    flipAndSimplify(*ARegion->getDMLPIterator2(),
                    true/*!getContour(ARegion->getDeformationDart()->getBeta(2))
                          ->FExternalContour*/,
                    false); 

  if ( !(getContour(ARegion->getDeformationDart())->getDMLP()->getFreemanChain()==
         freemanChainFromFace(ARegion->getDeformationDart()) ) )
     {
       std::cout<<"PB5"<<std::endl;
       std::cout<<getContour(ARegion->getDeformationDart())
         ->getDMLP()->getFreemanChain()<<std::endl
                <<freemanChainFromFace(ARegion->getDeformationDart())
                <<std::endl;
       exit(-1);
     }
   if ( !(getContour(ARegion->getDeformationDart()->getBeta(2))->getDMLP()
          ->getFreemanChain()==
          freemanChainFromFace(ARegion->getDeformationDart()->getBeta(2)) ) )
     {
       std::cout<<"PB5b"<<std::endl;
       std::cout<<getContour(ARegion->getDeformationDart()->getBeta(2))->
         getDMLP()->getFreemanChain()<<std::endl
                <<freemanChainFromFace(ARegion->getDeformationDart()->getBeta(2))
                <<std::endl;
       exit(-1);
       }
#endif // DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
             
  std::set<CRegion*> regionsToUpdate;
  //  computeAndSaveRegionEnergy(ARegion);
  regionsToUpdate.insert(ARegion);

  /*  computeAndSaveRegionEnergy(FMap->getRegion(ARegion->getDeformationDart()
      ->getBeta(2)));*/
  regionsToUpdate.insert(FMap->getRegion(ARegion->getDeformationDart()
                                         ->getBeta(2)));

  //int mark = FMap->getNewMark();

  //computeAndSaveContourEnergy(ARegion->getDeformationDart());
  //FMap->markOrbit(ARegion->getDeformationDart(),ORBIT_1,mark);

  //assert(!FMap->isMarked(ARegion->getDeformationDart()->getBeta(2),mark));
  //computeAndSaveContourEnergy(ARegion->getDeformationDart()->getBeta(2));
  //FMap->markOrbit(ARegion->getDeformationDart()->getBeta(2),ORBIT_1,mark);

  // If the first vertex was moved
  if( vertexMoved & 1 )
  {
     for (CDynamicCoverageVertex dVertex(FMap,ARegion->getDeformationDart());
                                         dVertex.cont();++dVertex)
     {
        if ( !FMap->getRegion(*dVertex)->isInfiniteRegion() )
        {
           if ( regionsToUpdate.find(FMap->getRegion(*dVertex))==
                regionsToUpdate.end())
           {
              //computeAndSaveRegionEnergy(FMap->getRegion(*dVertex));
              regionsToUpdate.insert(FMap->getRegion(*dVertex));
           }
           /*
         if ( !FMap->isMarked(*dVertex,mark) )
         {
            computeAndSaveContourEnergy(*dVertex);
            FMap->markOrbit(*dVertex,ORBIT_1,mark);
        }*/
        }
     }
  }
  
   // If the second vertex was moved
  if( vertexMoved & 2 )
    {
      for (CDynamicCoverageVertex dVertex(FMap,ARegion->getDeformationDart()
                                               ->getBeta(2));
           dVertex.cont();++dVertex)
      {
         if ( !FMap->getRegion(*dVertex)->isInfiniteRegion() )
         {
            if ( regionsToUpdate.find(FMap->getRegion(*dVertex))==
                 regionsToUpdate.end())
            {
               //computeAndSaveRegionEnergy(FMap->getRegion(*dVertex));
               regionsToUpdate.insert(FMap->getRegion(*dVertex));
            }
         }
         /*
         if ( !FMap->isMarked(*dVertex,mark) )
         {
            computeAndSaveContourEnergy(*dVertex);
            FMap->markOrbit(*dVertex,ORBIT_1,mark);
      }*/
      }
    }

  // Unmark the darts.
  //FMap->unmarkOrbit(ARegion->getDeformationDart(),ORBIT_1,mark);
  //FMap->unmarkOrbit(ARegion->getDeformationDart()->getBeta(2),ORBIT_1,mark);
  /*
  for(CDynamicCoverageVertex dVertex(FMap,ARegion->getDeformationDart());
      dVertex.cont();++dVertex)
    if ( FMap->isMarked(*dVertex,mark) )
      FMap->unmarkOrbit(*dVertex,ORBIT_1,mark);
  for(CDynamicCoverageVertex dVertex(FMap,ARegion->getDeformationDart()
                 ->getBeta(2));dVertex.cont();++dVertex)
    if ( FMap->isMarked(*dVertex,mark) )
      FMap->unmarkOrbit(*dVertex,ORBIT_1,mark);
*/
  //assert( FMap->isWholeMapUnmarked(mark) );
  //FMap->freeMark(mark);

//   std::cout<<"After apply, before updateMaximum ";
//  computeGlobalEnergy();
  
  for (std::set<CRegion*>::iterator it=regionsToUpdate.begin();
       it!=regionsToUpdate.end(); ++it)
    {
      double gain = updateMaximumGainForRegion(*it);
      
      // Modify the priority of the region in the queue
      if ( FIndex!=-1 )
         APq.update(*static_cast<TPartitionQueue::iterator*>
                    (FMap->getDirectInfo((*it)->getRepresentativeDart(),
                                         FIndex)), gain );
    }
  
//   std::cout<<"[END] applyBestDeformation "<<std::endl;
}
//*****************************************************************************
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_MLP || DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
FreemanChain CDeformablePartition::freemanChainFromFace( CDart * ADart ) const
{
  std::stringstream s;
  for( CDynamicCoverage1 dFace( FMap, ADart ) ; dFace.cont() ; ++dFace )
    {
      for( CCoverageEdgeLinels tEdge(FMap,*dFace) ; tEdge.cont() ; ++tEdge )
        {
          // Difference between YNEG and YPOS between carteTopo2D and ImaGene
          TLinel correctedLinel = (*tEdge).getLinel();
          if ( correctedLinel==YNEG || correctedLinel==YPOS)
            correctedLinel=CDoublet::getOppositeLinel(correctedLinel);
          s << static_cast<int>(correctedLinel);
        }
    }
  
  //   std::cout<<s.str()<<"  "<<static_cast<CTopologicalDart*>(AEdge)->doublet()
  //       <<std::endl;
  
  FreemanChain c(s.str(),
                 static_cast<CTopologicalDart*>(ADart)->doublet().getX(),
                 static_cast<CTopologicalDart*>(ADart)->doublet().getY());
  return c;
}
#endif
//******************************************************************************
#endif // DEFORMABLE_METHOD
//******************************************************************************
