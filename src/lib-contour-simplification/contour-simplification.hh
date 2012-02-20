//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#ifndef CONTOUR_SIMPLIFICATION_HH
#define CONTOUR_SIMPLIFICATION_HH
//******************************************************************************
// Méthodes génériques de simplification de contours

#include "topological-map.hh"

// En cas d'erreur sur les threads.
void systemError(const char* s);

// Simplification séquentielle
void sequentielContourSimplification(Map2d::CTopologicalMap* ATopologicalMap,
                                     bool (*ACriterion)
                                     (Map2d::CTopologicalMap*, Map2d::CDart*),
                                     void (*AUpdate)(Map2d::CTopologicalMap*,
                                                     Map2d::CDart*));

#ifdef TBB_ACTIVE
// Simplification parallèle
// Si AGrainSize==0, utilise auto_partitioner(), sinon utilise la
// valeur AGrainSize.
// retourne le nombre de conflit
unsigned int parallelContourSimplification(Map2d::CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   unsigned int AGrainSize,
                                   bool (*ACriterion)(Map2d::CTopologicalMap*,
                                                      Map2d::CDart*),
                                   void (*AUpdate)(Map2d::CTopologicalMap*,
                                                   Map2d::CDart*),
                                   bool (*ATakeMutexes)
                                   (Map2d::CTopologicalMap*, Map2d::CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2));
// Version avec prise de thread en mode bloquant
unsigned int parallelContourSimplificationBlock(Map2d::CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   unsigned int AGrainSize,
                                   bool (*ACriterion)(Map2d::CTopologicalMap*,
                                                      Map2d::CDart*),
                                   void (*AUpdate)(Map2d::CTopologicalMap*,
                                                   Map2d::CDart*),
                                   void (*ATakeMutexes)
                                   (Map2d::CTopologicalMap*, Map2d::CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2));
// idem parallelContourSimplification avec pthread.
unsigned int parallelContourSimplificationPThread(Map2d::CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   bool (*ACriterion)(Map2d::CTopologicalMap*,
                                                      Map2d::CDart*),
                                   void (*AUpdate)(Map2d::CTopologicalMap*,
                                                   Map2d::CDart*),
                                   bool (*ATakeMutexes)
                                   (Map2d::CTopologicalMap*,
                                    Map2d::CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2));
// idem parallelContourSimplificationBlock avec pthread.
unsigned int parallelContourSimplificationPThreadBlock(Map2d::CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   bool (*ACriterion)(Map2d::CTopologicalMap*,
                                                      Map2d::CDart*),
                                   void (*AUpdate)(Map2d::CTopologicalMap*,
                                                   Map2d::CDart*),
                                   void (*ATakeMutexes)
                                   (Map2d::CTopologicalMap*,
                                    Map2d::CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2));
#endif // TBB_ACTIVE

//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION_HH
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
