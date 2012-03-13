//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#include "contour-simplification.hh"
#include "chrono.hh"

#ifdef TBB_ACTIVE
#include <pthread.h>
#endif // TBB_ACTIVE
//******************************************************************************
using namespace Map2d;
using namespace std;
//******************************************************************************
void systemError(const char* s)
{
   cerr << s << endl;
   exit(EXIT_FAILURE);
}
//******************************************************************************
void sequentielContourSimplification(CTopologicalMap* ATopologicalMap,
                                     bool (*ACriterion)(CTopologicalMap*,
                                                        CDart*),
                                     void (*AUpdate)(CTopologicalMap*,
                                                     CDart*))
{
   int treated = ATopologicalMap->getNewMark();
   CDart* current;

   CDynamicCoverageAll it(ATopologicalMap);
   while (it.cont())
   {
      current = it++;

      if (!ATopologicalMap->isMarked(current, treated))
      {
	if (current<ATopologicalMap->beta21(current))
	  {
	    // On supprime un sommet s'il est de degré deux,
	    // pas extrémité d'une arête pendante et non boucle.
	    if (ATopologicalMap->beta02(current) ==
		ATopologicalMap->beta21(current) &&
		ATopologicalMap->beta0(current) !=
		ATopologicalMap->beta2(current) &&
		ATopologicalMap->beta1(current) != current )
	      {
		if (ACriterion(ATopologicalMap, current))
		  {
		    if (it.cont() && *it == ATopologicalMap->beta21(current))
		      ++it;
		    
		    AUpdate(ATopologicalMap, current);
		    
		    ATopologicalMap->
		      vertexRemoval(current, ATopologicalMap->getMarkToDelete());
		  }
	      }
	  }
      }
   }
}
//******************************************************************************
#ifdef TBB_ACTIVE
//******************************************************************************
int GTreated;
CTopologicalMap* GTopologicalMap;
unsigned int GNbPb;
unsigned int GNbTabToProcess;
bool (*GCriterion)(CTopologicalMap*,CDart*);
void (*GUpdate)(CTopologicalMap*,CDart*);
bool (*GTakeMutexes)(CTopologicalMap*,CDart*);
void (*GReleaseMutexes)(CTopologicalMap*,CDart*);
//******************************************************************************
// Version avec prise de mutex si nécessaire
// Les 2 mutexs non bloquant, pas d'utilisation de numérotation des sommets.
void* parallelVertexRemovalOneThread(void* arg)
{
  // Maintenant la boucle proprement dite.
  unsigned int monNum = (unsigned int)(arg);
  unsigned int j = -1;
  unsigned int i = monNum*GNbTabToProcess;
  unsigned int endrange = i+GNbTabToProcess;

  //  cout<<"["<<pthread_self()<<"] Début thread pour traiter ["<<i<<", "<<endrange<<"]"<<endl;

  CDart* current;
  queue<CDart*> verticesToTreat;
  bool processDart = false;

  
  while (i != endrange || !verticesToTreat.empty())
    {
      // Soit on prend un sommet dans la pile,
      // soit dans le parcours de tout les brins
      if (i != endrange) //!verticesToTreat.empty())
	{
	  do
            {
	      ++j;
	      if ((i == GTopologicalMap->getNbTabsDarts() - 1 &&
		   j == GTopologicalMap->getNbDartsInLastTab()) ||
		  j == GTopologicalMap->getSizeDartArray())
		{ ++i; j = 0; }
	      if (i != endrange)
		current = GTopologicalMap->getDart(i, j);
	      else
		current = NULL;
	      //	      cout<<"i,j="<<i<<","<<j<<endl;
            }
	  while (i != endrange &&
		 GTopologicalMap->
		 isMarked(current, GTopologicalMap->getMarkToDelete()));
	  processDart = false;
	}
      else
	{
	  current = verticesToTreat.front();
	  verticesToTreat.pop();
	  processDart = true;
	}
      
         // On supprime un sommet s'il est de degré deux,
         // pas extrémité d'une arête pendante et non boucle.
         if (current != NULL && current<GTopologicalMap->beta21(current) )
         {
	   //	   cout<<"processDart="<<current<<endl;
            if (!processDart)
            {
               processDart = true;
            }

            if (processDart &&
                  GTopologicalMap->beta02(current) ==
                  GTopologicalMap->beta21(current) &&
                  GTopologicalMap->beta0(current) !=
                  GTopologicalMap->beta2(current) &&
                  GTopologicalMap->beta1(current) != current)
            {
               if ( GTakeMutexes(GTopologicalMap, current) )
               {
                  if ( GCriterion(GTopologicalMap, current) )
                  {
                     GUpdate(GTopologicalMap, current);
                     GTopologicalMap->
                     vertexRemoval(current,
                                   GTopologicalMap->getMarkToDelete());
                  }
                  GReleaseMutexes(GTopologicalMap,current);
               }
               else
               {
                  // On empile le sommet pour le traiter plus tard.
                  verticesToTreat.push(current);
                  ++GNbPb;
               }
            }
         }
    }

  if ( monNum==0 ) return NULL;
  
  //  cout<<"["<<pthread_self()<<"] Fin du thread"<<endl;
  pthread_exit(0);
}
//******************************************************************************
void parallelContourSimplification(CTopologicalMap* ATopologicalMap,
                                   unsigned int AGrainSize,
                                   bool (*ACriterion)(CTopologicalMap*,
                                                      CDart*),
                                   void (*AUpdate)(CTopologicalMap*,
                                                   CDart*),
                                   bool (*ATakeMutexes)(CTopologicalMap*,
                                                        CDart*),
                                   void (*AReleaseMutexes)(CTopologicalMap*,
                                                           CDart*))
{
  assert( AGrainSize>0 );
  
  // 1) Initialisation des variables globales
  GTreated	  = ATopologicalMap->getNewMark();
  GTopologicalMap = ATopologicalMap;
  GNbPb		  = 0;
  GCriterion	  = ACriterion;
  GUpdate	  = AUpdate;
  GTakeMutexes	  = ATakeMutexes;
  GReleaseMutexes = AReleaseMutexes;  
  GNbTabToProcess = GTopologicalMap->getNbTabsDarts()/AGrainSize;

  pthread_t thread[AGrainSize];

  /*  cout<<"["<<pthread_self()<<"] Main thread : nbTab="
      <<GTopologicalMap->getNbTabsDarts()
      <<", nbThreads="<<AGrainSize<<endl;
  */
  // 2) On créé NB_THREADS-1 threads, en les détachant pour qu'ils
  // s'arrêtent automatiquement à la fin.
  unsigned int num=1;
  for ( ; num<AGrainSize; ++num )
    {
      if ( pthread_create(&thread[num], NULL,
			  parallelVertexRemovalOneThread,
			  (void*)(num))!=0 )
	systemError("Creation du thread impossible.");
    }

  // Le mainthread se charge de la dernière partie.
  parallelVertexRemovalOneThread((void*)0);

  for ( num=1; num<AGrainSize; ++num )
    if ( pthread_join(thread[num],NULL)!=0 )
      systemError("Erreur lors du pthread_join");
  
  ATopologicalMap->negateMaskMark(GTreated);
  ATopologicalMap->freeMark(GTreated);
}
//******************************************************************************
#endif // TBB_ACTIVE
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
