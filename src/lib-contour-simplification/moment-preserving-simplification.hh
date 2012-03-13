//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#ifndef MOMENT_PRESERVING_SIMPLIFICATION_HH
#define MOMENT_PRESERVING_SIMPLIFICATION_HH
//******************************************************************************
// Méthodes d'approximation par préservation des moments

#include "contour-simplification.hh"

// Simplification séquentielle
void sequentielMomentPreservingSimplification(Map2d::CTopologicalMap* AMap,
                                              unsigned int APercentage = 5 );

#ifdef TBB_ACTIVE
// Simplification parallèle
unsigned int parallelMomentPreservingSimplification(Map2d::CTopologicalMap* AMap,
						    unsigned int ANbThreads,
						    unsigned int AGrainSize=0,
						    bool AParallelTBB=false,
						    unsigned int APercentage = 5 );
unsigned int parallelMomentPreservingSimplificationBlock(Map2d::CTopologicalMap* AMap,
							 unsigned int ANbThreads,
							 unsigned int AGrainSize=0,
							 bool AParallelTBB=false,
							 unsigned int APercentage = 5 );
#endif // TBB_ACTIVE

//******************************************************************************
#endif // MOMENT_PRESERVING_SIMPLIFICATION_HH
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
