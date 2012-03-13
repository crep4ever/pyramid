//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#include "contour-simplification.hh"
#include "chrono.hh"

#ifdef TBB_ACTIVE
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/mutex.h"
#include "region.hh"
using namespace tbb;
#endif // TBB_ACTIVE
//******************************************************************************
using namespace Map2d;
using namespace std;
#ifdef TBB_ACTIVE
tbb::mutex FGlobalMutex;
#endif // TBB_ACTIVE
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
    CDart* current;

    CDynamicCoverageAll it(ATopologicalMap);
    while (it.cont())
    {
        current = it++;

        // On supprime un sommet s'il est de degré deux,
        // pas extrémité d'une arête pendante et non boucle.
        if (current < ATopologicalMap->beta21(current) &&
                ATopologicalMap->beta02(current) ==
                ATopologicalMap->beta21(current) &&
                ATopologicalMap->beta0(current) !=
                ATopologicalMap->beta2(current) &&
                ATopologicalMap->beta1(current) != current)
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
//******************************************************************************
#ifdef TBB_ACTIVE
//******************************************************************************
// Version avec prise de mutex si nécessaire
// Les 2 mutexs non bloquant, pas d'utilisation de numérotation des sommets.
class CParallelContourSimplification
{
public:
  CParallelContourSimplification(bool (*ACriterion)(CTopologicalMap*, CDart*),
				 void (*AUpdate)(CTopologicalMap*, CDart*),
				 bool (*ATakeMutexes)
				 (CTopologicalMap*, CDart*,
				  tbb::spin_mutex::scoped_lock& ALock1,
				  tbb::spin_mutex::scoped_lock& ALock2)
				 ):
    FCriterion(ACriterion),
    FUpdate(AUpdate),
    FTakeMutexes(ATakeMutexes)
  {}

    ~CParallelContourSimplification()
    {}
  
    void operator()(const blocked_range<unsigned int>& range)const
    {
        CDart* current;
        queue<CDart*> verticesToTreat;
        tbb::spin_mutex::scoped_lock lock1, lock2;
	tbb::mutex::scoped_lock FGlobalLock;

        // Maintenant la boucle proprement dite.
        unsigned int i = range.begin();
        unsigned int j = -1;

	//std::cout<<"RANGE["<<i<<","<<range.end()<<"["<<std::endl;	

        while (i != range.end() || !verticesToTreat.empty())
	  {
	    // Soit on prend un sommet dans la pile,
	    // soit dans le parcours de tout les brins
	    if (i != range.end())
	      {
		do
		  {
		    ++j;
		    if ((i == FTopologicalMap->getNbTabsDarts() - 1 &&
			 j == FTopologicalMap->getNbDartsInLastTab()) ||
			j == FTopologicalMap->getSizeDartArray())
		      { ++i; j = 0; }
                    if (i != range.end())
		      current = FTopologicalMap->getDart(i, j);
                    else
		      current = NULL;
		  }
                while (i != range.end() &&
                       (FTopologicalMap->
                        isMarked(current, FTopologicalMap->getMarkToDelete()) ||
                        current > FTopologicalMap->beta21(current)) );
	      }
	    else
	      {
                current = verticesToTreat.front();
                verticesToTreat.pop();
	      }
	    //	    std::cout<<"step1..."<<std::flush;

            // On supprime un sommet s'il est de degré deux,
            // pas extrémité d'une arête pendante et non boucle.
            if (current != NULL &&
                    FTopologicalMap->beta02(current) ==
                    FTopologicalMap->beta21(current) &&
                    FTopologicalMap->beta0(current) !=
                    FTopologicalMap->beta2(current) &&
                    FTopologicalMap->beta1(current) != current)
            {
	      //	      std::cout<<"FGlobalLock.acquire(FGlobalMutex)..."<<std::flush;
	      //FGlobalLock.acquire(FGlobalMutex);
	      if (FTakeMutexes(FTopologicalMap, current, lock1, lock2))
                {
		  // FGlobalLock.release();//std::cout<<"OK."<<std::endl;
                    if (FCriterion(FTopologicalMap, current))
                    {
                        FUpdate(FTopologicalMap, current);
                        FTopologicalMap->
                        vertexRemoval(current,
                                      FTopologicalMap->getMarkToDelete());
                    }
                    lock1.release();
                    lock2.release();
                }
                else
                {
		  //FGlobalLock.release();//std::cout<<"OK."<<std::endl;
		  //std::cout<<"PROBLEME !!!!!!!"<<std::endl;
                    // On empile le sommet pour le traiter plus tard.
		  verticesToTreat.push(current);
		  ++FNbPb;
                }
            }
	    //	    std::cout<<"step2..."<<std::endl;
        }
	//	std::cout<<"END FOR RANGE["<<i<<","<<j<<"]"<<std::endl;
    }

unsigned int getNbPb() const
{ return FNbPb; }

public:
static unsigned int FNbPb; ///< Pour compter le nombre de conflits.

/// La carte en cours de simplification
static CTopologicalMap* FTopologicalMap;

private:
bool (*FCriterion)(CTopologicalMap*, CDart*);
void (*FUpdate)(CTopologicalMap*, CDart*);
bool (*FTakeMutexes)(CTopologicalMap*, CDart*,
                     tbb::spin_mutex::scoped_lock& ALock1,
                     tbb::spin_mutex::scoped_lock& ALock2);
};
//******************************************************************************
// Version avec prise de mutex bloquant.
class CParallelContourSimplificationBlock
{
public:
  CParallelContourSimplificationBlock(bool (*ACriterion)(CTopologicalMap*, CDart*),
				      void (*AUpdate)(CTopologicalMap*, CDart*),
				      void (*ATakeMutexes)
				      (CTopologicalMap*, CDart*,
				       tbb::spin_mutex::scoped_lock& ALock1,
				       tbb::spin_mutex::scoped_lock& ALock2)
				      ):
    FCriterion(ACriterion),
    FUpdate(AUpdate),
    FTakeMutexes(ATakeMutexes)
  {}

    ~CParallelContourSimplificationBlock()
    {}
  
    void operator()(const blocked_range<unsigned int>& range)const
    {
        CDart* current;
        tbb::spin_mutex::scoped_lock lock1, lock2;

        // Maintenant la boucle proprement dite.
        unsigned int i = range.begin();
        unsigned int j = -1;

        while (i != range.end() )
	  {
	    do
	      {
		++j;
		if ((i == FTopologicalMap->getNbTabsDarts() - 1 &&
		     j == FTopologicalMap->getNbDartsInLastTab()) ||
		    j == FTopologicalMap->getSizeDartArray())
		  { ++i; j = 0; }
		if (i != range.end())
		  current = FTopologicalMap->getDart(i, j);
		else
		  current = NULL;
	      }
	    while (i != range.end() &&
		   (FTopologicalMap->
		    isMarked(current, FTopologicalMap->getMarkToDelete()) ||
		    current > FTopologicalMap->beta21(current)) );
	    //	    std::cout<<"step1..."<<std::flush;
	    
            // On supprime un sommet s'il est de degré deux,
            // pas extrémité d'une arête pendante et non boucle.
            if (current != NULL &&
		FTopologicalMap->beta02(current) ==
		FTopologicalMap->beta21(current) &&
		FTopologicalMap->beta0(current) !=
		FTopologicalMap->beta2(current) &&
		FTopologicalMap->beta1(current) != current)
	      {
		//	      std::cout<<"FGlobalLock.acquire(FGlobalMutex)..."<<std::flush;
	      //FGlobalLock.acquire(FGlobalMutex);
		FTakeMutexes(FTopologicalMap, current, lock1, lock2);
		// FGlobalLock.release();//std::cout<<"OK."<<std::endl;
		if (FCriterion(FTopologicalMap, current))
		  {
		    FUpdate(FTopologicalMap, current);
		    FTopologicalMap->
		      vertexRemoval(current,
				    FTopologicalMap->getMarkToDelete());
		  }
		lock1.release();
		lock2.release();
	      }
	  }
	//	std::cout<<"END FOR RANGE["<<i<<","<<j<<"]"<<std::endl;
    }

unsigned int getNbPb() const
{ return FNbPb; }

public:
static unsigned int FNbPb; ///< Pour compter le nombre de conflits.

/// La carte en cours de simplification
static CTopologicalMap* FTopologicalMap;

private:
bool (*FCriterion)(CTopologicalMap*, CDart*);
void (*FUpdate)(CTopologicalMap*, CDart*);
void (*FTakeMutexes)(CTopologicalMap*, CDart*,
                     tbb::spin_mutex::scoped_lock& ALock1,
                     tbb::spin_mutex::scoped_lock& ALock2);
};
//******************************************************************************
// En paramètre, nbthreads, et grainSize
CTopologicalMap* CParallelContourSimplification::FTopologicalMap;
unsigned int CParallelContourSimplification::FNbPb;
//------------------------------------------------------------------------------
unsigned int parallelContourSimplification(CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   unsigned int AGrainSize,
                                   bool (*ACriterion)(CTopologicalMap*,
                                                      CDart*),
                                   void (*AUpdate)(CTopologicalMap*,
                                                   CDart*),
                                   bool (*ATakeMutexes)
                                   (CTopologicalMap*,
                                    CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2))
{
  task_scheduler_init init;//(task_scheduler_init::automatic);
  
  if ( ANbThreads==0 ) init.initialize(task_scheduler_init::automatic);
  else init.initialize(ANbThreads);
  
  CParallelContourSimplification parallelRemoval(ACriterion, AUpdate,
						 ATakeMutexes);

  CParallelContourSimplification::FTopologicalMap = ATopologicalMap;
  CParallelContourSimplification::FNbPb = 0;

  if (AGrainSize == 0)
    {
      parallel_for(blocked_range<unsigned int>
		   (0,ATopologicalMap->getNbTabsDarts()),
		   parallelRemoval, auto_partitioner());
    }
    else
      {
        parallel_for(blocked_range<unsigned int>
		     (0,ATopologicalMap->getNbTabsDarts(),
		      AGrainSize), parallelRemoval);
      }

  return parallelRemoval.getNbPb();
}
//******************************************************************************
// En paramètre, nbthreads, et grainSize
CTopologicalMap* CParallelContourSimplificationBlock::FTopologicalMap;
unsigned int CParallelContourSimplificationBlock::FNbPb;
//------------------------------------------------------------------------------
unsigned int parallelContourSimplificationBlock(CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   unsigned int AGrainSize,
                                   bool (*ACriterion)(CTopologicalMap*,
                                                      CDart*),
                                   void (*AUpdate)(CTopologicalMap*,
                                                   CDart*),
                                   void (*ATakeMutexes)
                                   (CTopologicalMap*,
                                    CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2))
{
  // TODO (or not?)
  return 0;
}

//******************************************************************************
CParallelContourSimplification* GParallelRemoval;
CParallelContourSimplificationBlock* GParallelRemovalBlock;
unsigned int GNbTabToProcess;
unsigned int GNbTabReste;
// Version avec prise de mutex si nÃ©cessaire
// Les 2 mutexs non bloquant, pas d'utilisation de numÃ©rotation des sommets.
void* parallelVertexRemovalOneThread(void* arg)
{
  unsigned long int monNum = (unsigned long int)(arg);
  unsigned int i;
  unsigned int endrange;
  if ( monNum<GNbTabReste )
    {
      i=monNum*(GNbTabToProcess+1);
      endrange=i+GNbTabToProcess+1;
    }
  else
    {
      i=GNbTabReste*(GNbTabToProcess+1)+(monNum-GNbTabReste)*(GNbTabToProcess);
      endrange=i+GNbTabToProcess;
    }
  if ( endrange > CParallelContourSimplification::FTopologicalMap->getNbTabsDarts() )
    endrange=CParallelContourSimplification::FTopologicalMap->getNbTabsDarts();
  
  blocked_range<unsigned int> range(i,endrange);

  //  std::cout<<"Thread "<<monNum<<" : ["<<i<<","<<endrange<<"]"<<std::endl;
  GParallelRemoval->operator() (range);
  return 0;
}
//------------------------------------------------------------------------------
// Version avec prise de mutex bloquant
void* parallelVertexRemovalOneThreadBlock(void* arg)
{
  unsigned long int monNum = (unsigned long int)(arg);
  unsigned int i = monNum*GNbTabToProcess;
  unsigned int endrange = i+GNbTabToProcess;
  blocked_range<unsigned int> range(i,endrange);

  //  std::cout<<"Thread "<<monNum<<" : ["<<i<<","<<endrange<<"]"<<std::endl;
  GParallelRemovalBlock->operator() (range);
  return 0;
}
//******************************************************************************
unsigned int parallelContourSimplificationPThread(CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   bool (*ACriterion)(CTopologicalMap*,
                                                      CDart*),
                                   void (*AUpdate)(CTopologicalMap*,
                                                   CDart*),
                                   bool (*ATakeMutexes)
                                   (CTopologicalMap*,
                                    CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2))
{
  task_scheduler_init init(task_scheduler_init::automatic);

  if ( ANbThreads==0 ) ANbThreads=8; // TODO better...

  pthread_t thread[ANbThreads];
  CParallelContourSimplification parallelRemoval(ACriterion, AUpdate,
						 ATakeMutexes);
  GParallelRemoval = &parallelRemoval;
  GNbTabToProcess = ATopologicalMap->getNbTabsDarts()/ANbThreads;
  GNbTabReste = ATopologicalMap->getNbTabsDarts()%ANbThreads;
  CParallelContourSimplification::FTopologicalMap = ATopologicalMap;

  // std::cout<<"ANbThreads="<<ANbThreads<<", "
  // 	   <<"GNbTabToProcess="<<GNbTabToProcess<<"["<<0<<","
  // 	   <<ATopologicalMap->getNbTabsDarts()-1<<"]"<<std::endl;
  
  unsigned long int num=1;
  for ( ; num<ANbThreads; ++num )
    {
      if ( pthread_create(&thread[num], NULL,
			  parallelVertexRemovalOneThread,
			  (void*)(num))!=0 )
	systemError("Creation du thread impossible.");
    }

  // Le mainthread se charge de la derniÃ¨re partie.
  parallelVertexRemovalOneThread((void*)0);
  
  for ( num=1; num<ANbThreads; ++num )
    if ( pthread_join(thread[num],NULL)!=0 )
      systemError("Erreur lors du pthread_join");

  return parallelRemoval.getNbPb();
}
//******************************************************************************
unsigned int parallelContourSimplificationPThreadBlock(CTopologicalMap* ATopologicalMap,
				   unsigned int ANbThreads,
                                   bool (*ACriterion)(CTopologicalMap*,
                                                      CDart*),
                                   void (*AUpdate)(CTopologicalMap*,
                                                   CDart*),
                                   void (*ATakeMutexes)
                                   (CTopologicalMap*,
                                    CDart*,
                                    tbb::spin_mutex::scoped_lock& ALock1,
                                    tbb::spin_mutex::scoped_lock& ALock2))
{
  task_scheduler_init init(task_scheduler_init::automatic);

  if ( ANbThreads==0 ) ANbThreads=8; // TODO better...

  pthread_t thread[ANbThreads];
  CParallelContourSimplificationBlock parallelRemoval(ACriterion, AUpdate,
						      ATakeMutexes);
  GParallelRemovalBlock = &parallelRemoval;
  GNbTabToProcess = ATopologicalMap->getNbTabsDarts()/ANbThreads;
  CParallelContourSimplificationBlock::FTopologicalMap = ATopologicalMap;
  
  unsigned long int num=1;
  for ( ; num<ANbThreads; ++num )
    {
      if ( pthread_create(&thread[num], NULL,
			  parallelVertexRemovalOneThreadBlock,
			  (void*)(num))!=0 )
	systemError("Creation du thread impossible.");
    }

  // Le mainthread se charge de la derniÃ¨re partie.
  parallelVertexRemovalOneThreadBlock((void*)0);
  
  for ( num=1; num<ANbThreads; ++num )
    if ( pthread_join(thread[num],NULL)!=0 )
      systemError("Erreur lors du pthread_join");

  return parallelRemoval.getNbPb();
}
//******************************************************************************
#endif // TBB_ACTIVE
//******************************************************************************
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
