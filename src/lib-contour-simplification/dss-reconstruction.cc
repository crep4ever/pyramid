//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#include "dss-reconstruction.hh"
#include "chrono.hh"
//******************************************************************************
using namespace Map2d;
using namespace std;
//******************************************************************************
bool mergeCriterionDSSReconstruction(CTopologicalMap* AMap, CDart* ADart)
{
   ADart->getBeta0()->orientEdgesAttribute(ADart);
   return ( ADart->getBeta0()->getEdgeAttribute()->FDss.unionDSS
         (ADart->getEdgeAttribute()->FDss) );
}
//-----------------------------------------------------------------------------
void updateAttributesDSSReconstruction(CTopologicalMap* AMap, CDart* ADart)
{}   
//******************************************************************************
void sequentielDSSReconstruction(CTopologicalMap* AMap)
{
   sequentielContourSimplification(AMap, 
                                   mergeCriterionDSSReconstruction,
                                   updateAttributesDSSReconstruction);
}
//******************************************************************************
#ifdef TBB_ACTIVE
//-----------------------------------------------------------------------------
bool takeMutexesDSSReconstruction(CTopologicalMap* AMap, CDart* ADart,
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
void takeMutexesDSSReconstructionBlock(CTopologicalMap* AMap, CDart* ADart,
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
unsigned int parallelDSSReconstruction(CTopologicalMap* AMap,
			       unsigned int ANbThreads,
			       unsigned int AGrainSize,
			       bool AParallelTBB)
{
  if ( AParallelTBB)
    return parallelContourSimplification(AMap, ANbThreads, AGrainSize,
				  mergeCriterionDSSReconstruction,
				  updateAttributesDSSReconstruction,
				  takeMutexesDSSReconstruction);
  else
    return parallelContourSimplificationPThread(AMap, ANbThreads, 
					 mergeCriterionDSSReconstruction,
					 updateAttributesDSSReconstruction,
					 takeMutexesDSSReconstruction);

}
//-----------------------------------------------------------------------------
unsigned int parallelDSSReconstructionBlock(CTopologicalMap* AMap,
					    unsigned int ANbThreads,
					    unsigned int AGrainSize,
					    bool AParallelTBB)
{
  if ( AParallelTBB)
    return parallelContourSimplificationBlock(AMap, ANbThreads, AGrainSize,
					      mergeCriterionDSSReconstruction,
					      updateAttributesDSSReconstruction,
					      takeMutexesDSSReconstructionBlock);
  else
    return parallelContourSimplificationPThreadBlock(AMap, ANbThreads, 
						     mergeCriterionDSSReconstruction,
						     updateAttributesDSSReconstruction,
						     takeMutexesDSSReconstructionBlock);

}
//-----------------------------------------------------------------------------
#endif // TBB_ACTIVE
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
