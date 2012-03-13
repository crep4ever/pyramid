//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#ifndef DSS_RECONSTRUCTION_HH
#define DSS_RECONSTRUCTION_HH
//******************************************************************************
// Différentes méthodes de reconstruction de droites discrètes

#include "contour-simplification.hh"

// Simplification séquentielle
void sequentielDSSReconstruction(Map2d::CTopologicalMap* ATopologicalMap);

#ifdef TBB_ACTIVE
// Simplification parallèle
unsigned int parallelDSSReconstruction(Map2d::CTopologicalMap* ATopologicalMap,
			       unsigned int ANbThreads,
                               unsigned int AGrainSize=0,
			       bool AParallelTBB=false);
unsigned int parallelDSSReconstructionBlock(Map2d::CTopologicalMap* ATopologicalMap,
			       unsigned int ANbThreads,
                               unsigned int AGrainSize=0,
			       bool AParallelTBB=false);
#endif // TBB_ACTIVE

//******************************************************************************
#endif // DSS_RECONSTRUCTION_HH
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
