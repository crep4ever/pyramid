//******************************************************************************
#ifdef CONTOUR_SIMPLIFICATION
//******************************************************************************
#include "moments2D.hh"
#include "topological-map.hh"
#include "vertex.hh"
#include "region.hh"
//******************************************************************************
using namespace Map2d;
//bool CMoments2D::FDebugConstructor;
//******************************************************************************
//-------------- internal functions -------------
inline double moment_0_0(const CVertex& AV1, const CVertex& AV2)
{
   return (AV1.getX() *AV2.getY() - AV2.getX() *AV1.getY()) / 2.0;
}
//-----------------------------------------------------------------------------
inline double moment_1_0(const CVertex& AV1, const CVertex& AV2)
{
   return ((AV2.getX() + AV1.getX()) * (AV1.getX() *AV2.getY() - AV2.getX() *AV1.getY())) / 6.0;
   //((AV2.getX()+AV1.getX())*(AV1.getX()*AV2.getY()-AV2.getX()*AV1.getY()))/6((AV1.getY() + AV1.getX())*(AV1.getX()*AV2.getY() - AV2.getX()*AV1.getY())) / 6.0;
}
//-----------------------------------------------------------------------------
inline double moment_0_1(const CVertex& AV1, const CVertex& AV2)
{
   return ((AV2.getY() + AV1.getY()) * (AV1.getX() *AV2.getY() - AV2.getX() *AV1.getY())) / 6.0;
   //((AV2.getY() + AV2.getX())*(AV1.getX()*AV2.getY() - AV2.getX()*AV1.getY())) / 6.0;
}
//-----------------------------------------------------------------------------
inline double moment_1_1(const CVertex& AV1, const CVertex& AV2)
{
   return ((AV1.getX() *AV2.getY() - AV2.getX() *AV1.getY()) * ((2*AV2.getX() + AV1.getX()) *AV2.getY() + (AV2.getX() + 2*AV1.getX()) *AV1.getY())) / 24.0;
   //((AV1.getX()*AV2.getY() - AV2.getX()*AV1.getY())*((2*AV1.getY() + AV1.getX())*AV2.getY() + AV2.getX()*AV1.getY() + 2*AV1.getX()*AV2.getX())) / 24.0;
}
//-----------------------------------------------------------------------------
inline double moment_2_0(const CVertex& AV1, const CVertex& AV2)
{
   return ((AV2.getX() *AV2.getX() + AV1.getX() *AV2.getX() + AV1.getX() *AV1.getX()) * (AV1.getX() *AV2.getY() - AV2.getX() *AV1.getY())) / 12.0;
   //((AV1.getY()*AV1.getY() + AV1.getX()*AV1.getY() + AV1.getX()*AV1.getX())*(AV1.getX()*AV2.getY() - AV2.getX()*AV1.getY())) / 12.0;
}
//-----------------------------------------------------------------------------
inline double moment_0_2(const CVertex& AV1, const CVertex& AV2)
{
   return ((AV1.getX() *AV2.getY() - AV2.getX() *AV1.getY()) * (AV2.getY() *AV2.getY() + AV1.getY() *AV2.getY() + AV1.getY() *AV1.getY())) / 12.0;
   //((AV1.getX()*AV2.getY() - AV2.getX()*AV1.getY())*(AV2.getY()*AV2.getY() + AV2.getX()*AV2.getY() + AV2.getX()*AV2.getX())) / 12.0;
}
//-----------------------------------------------------------------------------
double CMoments2D::getEdgeMoment0_0(CDart* ADart)
{
   return moment_0_0(FMap->getDoublet(ADart),
                     FMap->getDoublet(ADart->getBeta1()));
}
//-----------------------------------------------------------------------------
double CMoments2D::getEdgeMoment0_1(CDart* ADart)
{
   return moment_0_1(FMap->getDoublet(ADart),
                     FMap->getDoublet(ADart->getBeta1()));
}
//-----------------------------------------------------------------------------
double CMoments2D::getEdgeMoment1_0(CDart* ADart)
{
   return moment_1_0(FMap->getDoublet(ADart),
                     FMap->getDoublet(ADart->getBeta1()));
}
//-----------------------------------------------------------------------------
double CMoments2D::getEdgeMoment1_1(CDart* ADart)
{
   return moment_1_1(FMap->getDoublet(ADart),
                     FMap->getDoublet(ADart->getBeta1()));
}
//-----------------------------------------------------------------------------
double CMoments2D::getEdgeMoment2_0(CDart* ADart)
{
   return moment_2_0(FMap->getDoublet(ADart),
                     FMap->getDoublet(ADart->getBeta1()));
}
//-----------------------------------------------------------------------------
double CMoments2D::getEdgeMoment0_2(CDart* ADart)
{
   return moment_0_2(FMap->getDoublet(ADart),
                     FMap->getDoublet(ADart->getBeta1()));
}
//*****************************************************************************
CMoments2D& CMoments2D::operator += (const CMoments2D & AMoment)
{
   for (int i = 0;i <= MOMENT_ORDERS; ++i)
      for (int j = 0;j <= MOMENT_ORDERS; ++j)
      {
         FInitialMoments[i][j] += AMoment.FInitialMoments[i][j];
         FMoments[i][j] += AMoment.FMoments[i][j];
      }
   return *this;
}
//*****************************************************************************
void CMoments2D::updateMomentsForVertexRemoval(CDart* ADart,
      CMoments2D& AMoments2,
      double AMomentReg1
      [MOMENT_ORDERS+1][MOMENT_ORDERS+1],
      double AMomentReg2
      [MOMENT_ORDERS+1][MOMENT_ORDERS+1])
{
   assert(FMap != NULL && FRegion != NULL && ADart != NULL);

   double deltaMoments[MOMENT_ORDERS+1][MOMENT_ORDERS+1];
   CDart* prec = ADart->getBeta0();

   deltaMoments[0][0] = moment_0_0(FMap->getDoublet(prec),
                                   FMap->getDoublet(ADart->getBeta2()));
   deltaMoments[1][0] = moment_1_0(FMap->getDoublet(prec),
                                   FMap->getDoublet(ADart->getBeta2()));
   deltaMoments[0][1] = moment_0_1(FMap->getDoublet(prec),
                                   FMap->getDoublet(ADart->getBeta2()));
   deltaMoments[2][0] = moment_2_0(FMap->getDoublet(prec),
                                   FMap->getDoublet(ADart->getBeta2()));
   deltaMoments[0][2] = moment_0_2(FMap->getDoublet(prec),
                                   FMap->getDoublet(ADart->getBeta2()));
   deltaMoments[1][1] = moment_1_1(FMap->getDoublet(prec),
                                   FMap->getDoublet(ADart->getBeta2()));
   deltaMoments[1][2]=0;
   deltaMoments[2][1]=0;
   deltaMoments[2][2]=0;

   deltaMoments[0][0] -= getEdgeMoment0_0(ADart);
   deltaMoments[1][0] -= getEdgeMoment1_0(ADart);
   deltaMoments[0][1] -= getEdgeMoment0_1(ADart);
   deltaMoments[2][0] -= getEdgeMoment2_0(ADart);
   deltaMoments[0][2] -= getEdgeMoment0_2(ADart);
   deltaMoments[1][1] -= getEdgeMoment1_1(ADart);

   deltaMoments[0][0] -= getEdgeMoment0_0(prec);
   deltaMoments[1][0] -= getEdgeMoment1_0(prec);
   deltaMoments[0][1] -= getEdgeMoment0_1(prec);
   deltaMoments[2][0] -= getEdgeMoment2_0(prec);
   deltaMoments[0][2] -= getEdgeMoment0_2(prec);
   deltaMoments[1][1] -= getEdgeMoment1_1(prec);

   for (int i = 0;i <= MOMENT_ORDERS; ++i)
      for (int j = 0;j <= MOMENT_ORDERS; ++j)
      {
         AMomentReg1[i][j] = FMoments[i][j] + deltaMoments[i][j];
         AMomentReg2[i][j] = AMoments2.FMoments[i][j] - deltaMoments[i][j];
      }
}
//*****************************************************************************
bool CMoments2D::vertexRemoval(CDart* ADart, unsigned int APercentage)
{
   assert(FMap != NULL && FRegion != NULL && ADart != NULL);
   assert(0 <= APercentage && APercentage <= 100);
   assert(FRegion == FMap->getRegion(ADart));

   CDart* prec = ADart->getBeta0();

   ///Preliminary test: if vertices are colinear, we return true and we
   ///do not need to compute/update the moments
   /// A->B->C, test: det(AB,BC) == 0
   CVertex ab(CVertex(FMap->getDoublet(ADart)) - FMap->getDoublet(prec));
   CVertex bc(CVertex(FMap->getDoublet(ADart->getBeta2())) -
              FMap->getDoublet(ADart));
   if (det(ab, bc) == 0) return true;

   double newMoments [MOMENT_ORDERS+1][MOMENT_ORDERS+1];
   double newMoments2[MOMENT_ORDERS+1][MOMENT_ORDERS+1];

   CRegion*    region2 = FMap->getRegion(ADart->getBeta2());
   CMoments2D& moments2 = region2->FMoments;

   updateMomentsForVertexRemoval(ADart, moments2, newMoments, newMoments2);

   /*
   std::cout<<"Try to remove "<<APercentage<<" : ";
   double ratio = 0;
   if ( !FRegion->isInfiniteRegion() && FRegion->getId() == 2 )
     {
       for ( int i = 0;i <= MOMENT_ORDERS; ++i )
         for ( int j = 0;j <= MOMENT_ORDERS; ++j )
             std::cout << newMoments[i][j] << "; ";
       std::cout << std::endl <<" Ratio : ";
       for ( int i = 0;i <= MOMENT_ORDERS; ++i )
          for ( int j = 0;j <= MOMENT_ORDERS; ++j )
         {
             ratio = newMoments[i][j] / FInitialMoments[i][j];
             std::cout << ratio<< "("
                   <<(ratio <= (100 - APercentage) / 100.0 ||
                   ratio >= (100 + APercentage) / 100.0)<<"); ";

         }
       std::cout << std::endl;
     }
   else if ( !region2->isInfiniteRegion() && region2->getId() == 2 )
     {
       for ( int i = 0;i <= MOMENT_ORDERS; ++i )
         for ( int j = 0;j <= MOMENT_ORDERS; ++j )
             std::cout << newMoments2[i][j] <<"; ";
       std::cout << std::endl <<" Ratio : ";
       for ( int i = 0;i <= MOMENT_ORDERS; ++i )
          for ( int j = 0;j <= MOMENT_ORDERS; ++j )
         {
            ratio = newMoments2[i][j] / moments2.FInitialMoments[i][j];
            std::cout << ratio<< "("
                  <<(ratio <= (100 - APercentage) / 100.0 ||
                  ratio >= (100 + APercentage) / 100.0)<<"); ";
         }
       std::cout << std::endl;
     }
  */

   double ratio = 0;
   for (int i = 0;i <= MOMENT_ORDERS; ++i)
      for (int j = 0;j <= MOMENT_ORDERS; ++j)
      {
         if (!FRegion->isInfiniteRegion() && FInitialMoments[i][j]!=0)
         {
            ratio = newMoments[i][j] / FInitialMoments[i][j];
            if (ratio <= (100 - APercentage) / 100.0 ||
                  ratio >= (100 + APercentage) / 100.0) return false;
         }

         if (!region2->isInfiniteRegion() && moments2.FInitialMoments[i][j]!=0)
         {
            ratio = newMoments2[i][j] / moments2.FInitialMoments[i][j];
            if (ratio <= (100 - APercentage) / 100.0 ||
                  ratio >= (100 + APercentage) / 100.0) return false;
         }
      }

   for (int i = 0;i <= MOMENT_ORDERS; ++i)
      for (int j = 0;j <= MOMENT_ORDERS; ++j)
      {
         FMoments[i][j] = newMoments[i][j];
         moments2.FMoments[i][j] = newMoments2[i][j];
      }

   return true;
}
//-----------------------------------------------------------------------------
CMoments2D::CMoments2D(CRegion *ARegion) :    //FIsLocked(false),
      FMap(NULL),
      FRegion(ARegion)

{
   for (int i = 0;i <= MOMENT_ORDERS;++i)
      for (int j = 0;j <= MOMENT_ORDERS;++j)
      {
         FMoments[i][j] = 0;
         FInitialMoments[i][j] = 0;
      }
//   if (FDebugConstructor) assert(false);
}
//-----------------------------------------------------------------------------
void CMoments2D::setMap(CTopologicalMap* AMap)
{
   assert(FMap == NULL && AMap != NULL);
   FMap = AMap;
}
//-----------------------------------------------------------------------------
void CMoments2D::addPixel(CDart *ADart)
{
   assert(FMap != NULL && FRegion != NULL);

   if (ADart == NULL) return;

   CDart* cur = ADart;
   do
   {
      FInitialMoments[0][0] += getEdgeMoment0_0(cur);
      FInitialMoments[1][0] += getEdgeMoment1_0(cur);
      FInitialMoments[0][1] += getEdgeMoment0_1(cur);
      FInitialMoments[2][0] += getEdgeMoment2_0(cur);
      FInitialMoments[0][2] += getEdgeMoment0_2(cur);
      FInitialMoments[1][1] += getEdgeMoment1_1(cur);
      cur = cur->getBeta1();
   }
   while (cur != ADart);

   for (int i = 0;i <= MOMENT_ORDERS;++i)
      for (int j = 0;j <= MOMENT_ORDERS;++j)
         FMoments[i][j] = FInitialMoments[i][j];
}
//-----------------------------------------------------------------------------
CMoments2D::~CMoments2D()
{
   // unlock();
//   if (FDebugConstructor) assert(false);
}

#ifdef TBB_ACTIVE
//******************************************************************************
/*void CMoments2D::lock()
{
   assert(false);
  FLock.acquire ( FMutex );
  //  FIsLocked = true;
}
//------------------------------------------------------------------------------
bool CMoments2D::tryLock()
{
  if ( FLock.try_acquire ( FMutex ) )
    {
      // std::cout<<"["<<this<<"] tryLock : OK."<<std::endl;
      // FIsLocked = true;
      return true;
    }

  //  std::cout<<"["<<this<<"] tryLock : No."<<std::endl;
  return false;
}
//------------------------------------------------------------------------------
void CMoments2D::unlock()
{
  //  if ( FIsLocked )
    {
      //      FIsLocked = false;
      // std::cout<<"["<<this<<"] unLock."<<std::endl;
      FLock.release();
    }
    }*/
//******************************************************************************
#endif //TBB_ACTIVE
//******************************************************************************
#endif //CONTOUR_SIMPLIFICATION
//******************************************************************************
