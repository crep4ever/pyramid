//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#include "moment-preserving-simplification.hh"
#include "polygonal-approximation.hh"
#include "region.hh"
#include "chrono.hh"
//******************************************************************************
using namespace Map2d;
using namespace std;
//******************************************************************************
static unsigned int percentageError = 5;
//******************************************************************************
bool mergeCriterionMomentPreservingSimplification(CTopologicalMap* AMap,
        CDart* ADart)
{
    return (AMap->getRegion(ADart)->
            FMoments.vertexRemoval(ADart, percentageError));
}
//-----------------------------------------------------------------------------
void updateAttributesMomentPreservingSimplification(CTopologicalMap* AMap,
        CDart* ADart)
{
}
//******************************************************************************
void sequentielMomentPreservingSimplification(CTopologicalMap* AMap,
        unsigned int APercentage)
{
    assert(AMap != NULL);
    assert(0 <= APercentage && APercentage <= 100);

    percentageError = APercentage;

    sequentielContourSimplification(AMap,
                                    mergeCriterionMomentPreservingSimplification,
                                    updateAttributesMomentPreservingSimplification);
}
//******************************************************************************
#ifdef TBB_ACTIVE
//-----------------------------------------------------------------------------
bool takeMutexesMomentPreservingSimplification(CTopologicalMap* AMap,
        CDart* ADart,
        tbb::spin_mutex::scoped_lock& ALock1,
        tbb::spin_mutex::scoped_lock& ALock2)
{
  CRegion *r1 = AMap->getRegion(ADart);
  CRegion *r2 = AMap->getRegion(ADart->getBeta(2));

  if ( r1>r2 )
    {
      CRegion *tmp=r1;
      r1=r2;
      r2=tmp;
    }  


  assert( r1!=NULL && r2!=NULL );
  assert( &r1->FMoments != &r2->FMoments);
  
  bool res = false;
  if (ALock1.try_acquire(r1->FMoments.FMutex))
    {
      if (ALock2.try_acquire(r2->FMoments.FMutex))
	res = true;
      else
	ALock1.release();
    }
  return res;
}
//-----------------------------------------------------------------------------
void takeMutexesMomentPreservingSimplificationBlock(CTopologicalMap* AMap,
        CDart* ADart,
        tbb::spin_mutex::scoped_lock& ALock1,
        tbb::spin_mutex::scoped_lock& ALock2)
{
  CRegion *r1 = AMap->getRegion(ADart);
  CRegion *r2 = AMap->getRegion(ADart->getBeta(2));

  if ( r1>r2 )
    {
      CRegion *tmp=r1;
      r1=r2;
      r2=tmp;
    }

  assert( r1!=NULL && r2!=NULL );
  assert( &r1->FMoments != &r2->FMoments);
  
  ALock1.acquire(r1->FMoments.FMutex);
  ALock2.acquire(r2->FMoments.FMutex);
}
//-----------------------------------------------------------------------------
unsigned int parallelMomentPreservingSimplification(CTopologicalMap* AMap,
					    unsigned int ANbThreads,
					    unsigned int AGrainSize,
					    bool AParallelTBB,
					    unsigned int APercentage)
{
    assert(AMap != NULL);
    assert(0 < AGrainSize);
    assert(0 <= APercentage && APercentage <= 100);

    percentageError = APercentage;

  if ( AParallelTBB)
    return parallelContourSimplification(AMap, ANbThreads, AGrainSize,
                                  mergeCriterionMomentPreservingSimplification,
                                  updateAttributesMomentPreservingSimplification,
                                  takeMutexesMomentPreservingSimplification);
  else
    return parallelContourSimplificationPThread(AMap, ANbThreads, 
					 mergeCriterionMomentPreservingSimplification,
					 updateAttributesMomentPreservingSimplification,
					 takeMutexesMomentPreservingSimplification);
}
//-----------------------------------------------------------------------------
unsigned int parallelMomentPreservingSimplificationBlock(CTopologicalMap* AMap,
					    unsigned int ANbThreads,
					    unsigned int AGrainSize,
					    bool AParallelTBB,
					    unsigned int APercentage)
{
    assert(AMap != NULL);
    assert(0 < AGrainSize);
    assert(0 <= APercentage && APercentage <= 100);

    percentageError = APercentage;

  if ( AParallelTBB)
    return parallelContourSimplificationBlock(AMap, ANbThreads, AGrainSize,
                                  mergeCriterionMomentPreservingSimplification,
                                  updateAttributesMomentPreservingSimplification,
                                  takeMutexesMomentPreservingSimplificationBlock);
  else
    return parallelContourSimplificationPThreadBlock(AMap, ANbThreads, 
					 mergeCriterionMomentPreservingSimplification,
					 updateAttributesMomentPreservingSimplification,
					 takeMutexesMomentPreservingSimplificationBlock);
}
//-----------------------------------------------------------------------------
#endif // TBB_ACTIVE
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************

