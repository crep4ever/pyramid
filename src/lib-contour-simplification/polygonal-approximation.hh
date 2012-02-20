//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#ifndef POLYGONAL_APPROXIMATION_HH
#define POLYGONAL_APPROXIMATION_HH
//******************************************************************************
// Méthodes d'approximation du polygone

#include "contour-simplification.hh"

// Simplification séquentielle
void sequentielPolygonalApproximation(Map2d::CTopologicalMap* AMap,
				      TCoordinate AError = 1);

#ifdef TBB_ACTIVE
// Simplification parallèle
unsigned int parallelPolygonalApproximation(Map2d::CTopologicalMap* AMap,
				    unsigned int ANbThreads,
                                    unsigned int AGrainSize=0,
				    bool AParallelTBB=false,
                                    TCoordinate AError = 1);
unsigned int parallelPolygonalApproximationBlock(Map2d::CTopologicalMap* AMap,
						 unsigned int ANbThreads,
						 unsigned int AGrainSize=0,
						 bool AParallelTBB=false,
						 TCoordinate AError = 1);
#endif // TBB_ACTIVE

//******************************************************************************
#endif // POLYGONAL_APPROXIMATION_HH
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
