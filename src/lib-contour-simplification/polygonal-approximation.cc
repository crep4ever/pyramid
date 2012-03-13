//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#include "contour-simplification.hh"
#include "polygonal-approximation.hh"
#include "chrono.hh"
//******************************************************************************
using namespace Map2d;
using namespace std;
//******************************************************************************
static TCoordinate maximalErrorPolygonalApprixomation=0;
//******************************************************************************
bool mergeCriterionPolygonalApprixomation(CTopologicalMap* AMap,
                                          CDart* ADart)
{
  CDart* prec = ADart->getBeta0();
  CVertex s1(AMap->getDoublet(prec));
  CVertex s2(AMap->getDoublet(AMap->beta2(ADart)));
  
  if ( CVertex(s1)==CVertex(s2) ) return false;
       
  ///Preliminary test: if vertices are colinear, we return true and we
  ///do not need to compute the distances with curves
  /// A->B->C, test: det(AB,BC) == 0
  CVertex ab(CVertex(AMap->getDoublet(ADart))-s1);
  CVertex bc(s2-AMap->getDoublet(ADart));
  if ( det(ab,bc) == 0 ) return true;

  TCoordinate res1 = AMap->distanceToCurve(ADart, s1, s2,
                                           maximalErrorPolygonalApprixomation);
  if (res1>=maximalErrorPolygonalApprixomation) return false;
  
  TCoordinate res2 = AMap->distanceToCurve(prec, s1, s2,
                                           maximalErrorPolygonalApprixomation);
  
  return res2<maximalErrorPolygonalApprixomation;
}
//-----------------------------------------------------------------------------
void updateAttributesPolygonalApprixomation(CTopologicalMap* AMap,
                                            CDart* ADart)
{}   
//******************************************************************************
void sequentielPolygonalApproximation(CTopologicalMap* AMap,
				                      TCoordinate AError)
{
   maximalErrorPolygonalApprixomation = AError;
   
   sequentielContourSimplification(AMap, 
                                   mergeCriterionPolygonalApprixomation,
                                   updateAttributesPolygonalApprixomation);
}
//******************************************************************************
#ifdef TBB_ACTIVE
//-----------------------------------------------------------------------------
bool takeMutexesPolygonalApprixomation(CTopologicalMap* AMap,
                                       CDart* ADart,
                                       tbb::spin_mutex::scoped_lock& ALock1,
                                       tbb::spin_mutex::scoped_lock& ALock2)
{
  CEdgeAttribute *e1 = ADart->getEdgeAttribute();
  CEdgeAttribute *e2 = ADart->getBeta(0)->getEdgeAttribute();

  if ( e1>e2 )
    {
      CEdgeAttribute *tmp=e1;
      e1=e2;
      e2=tmp;
    }  

  bool res = false;
  if (e1 != NULL && ALock1.try_acquire(e1->FMutex) )
    {
      assert( e2!=NULL );
      if ( ALock2.try_acquire(e2->FMutex) )
	res = true;
      else
	ALock1.release();
    }
  return res;
 }   
//-----------------------------------------------------------------------------
void takeMutexesPolygonalApprixomationBlock(CTopologicalMap* AMap,
					    CDart* ADart,
					    tbb::spin_mutex::scoped_lock& ALock1,
					    tbb::spin_mutex::scoped_lock& ALock2)
{
  CEdgeAttribute *e1 = ADart->getEdgeAttribute();
  CEdgeAttribute *e2 = ADart->getBeta(0)->getEdgeAttribute();

  if ( e1>e2 )
    {
      CEdgeAttribute *tmp=e1;
      e1=e2;
      e2=tmp;
    }  

  assert(e1!=NULL && e2!=NULL);
  ALock1.acquire(e1->FMutex);
  ALock2.acquire(e2->FMutex);
 }   
//-----------------------------------------------------------------------------
unsigned int parallelPolygonalApproximation(CTopologicalMap* AMap,
				    unsigned int ANbThreads,
				    unsigned int AGrainSize,
				    bool AParallelTBB,
                                    TCoordinate AError)
{
   maximalErrorPolygonalApprixomation = AError;

   if (AParallelTBB)
     return parallelContourSimplification(AMap, ANbThreads, AGrainSize,
				   mergeCriterionPolygonalApprixomation,
				   updateAttributesPolygonalApprixomation,
				   takeMutexesPolygonalApprixomation);
   else
     return parallelContourSimplificationPThread(AMap, ANbThreads, 
					  mergeCriterionPolygonalApprixomation,
					  updateAttributesPolygonalApprixomation,
					  takeMutexesPolygonalApprixomation);
}
//-----------------------------------------------------------------------------
unsigned int parallelPolygonalApproximationBlock(CTopologicalMap* AMap,
				    unsigned int ANbThreads,
				    unsigned int AGrainSize,
				    bool AParallelTBB,
                                    TCoordinate AError)
{
   maximalErrorPolygonalApprixomation = AError;

   if (AParallelTBB)
     return parallelContourSimplificationBlock(AMap, ANbThreads, AGrainSize,
				   mergeCriterionPolygonalApprixomation,
				   updateAttributesPolygonalApprixomation,
				   takeMutexesPolygonalApprixomationBlock);
   else
     return parallelContourSimplificationPThreadBlock(AMap, ANbThreads, 
					  mergeCriterionPolygonalApprixomation,
					  updateAttributesPolygonalApprixomation,
					  takeMutexesPolygonalApprixomationBlock);
}
//-----------------------------------------------------------------------------
#endif // TBB_ACTIVE
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
