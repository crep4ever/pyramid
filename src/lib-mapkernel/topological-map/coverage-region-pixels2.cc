////*****************************************************************************
//#include "coverage-region-pixels.hh"
//using namespace Map2d;
////*****************************************************************************
//CCoverageRegionPixels::CCoverageRegionPixels(CTopologicalMap* ATopologicalMap, 
//					     CRegion* ARegion):
//  FTopologicalMap(ATopologicalMap),
//  FRegion(ARegion),
//  FCurrentPixel(ARegion->getFirstPixel()),
//  FStop(false)
//{
//  if( ARegion->isInfiniteRegion() ) return;
//  assert(FPixelsStack.empty());
//
//  FPixelsStack.push(ARegion->getFirstPixel());
//  this->operator++();
//  FTopologicalMap->markPixel(FCurrentPixel.getX(),FCurrentPixel.getY());
//}
////*****************************************************************************
//CCoverageRegionPixels::~CCoverageRegionPixels()
//{
//  FTopologicalMap->unmarkAllPixels();
//}
////*****************************************************************************
//void CCoverageRegionPixels::reinit()
//{
//  FStop = false;
//  FTopologicalMap->unmarkAllPixels();
//  while(!FPixelsStack.empty())
//    {
//      FPixelsStack.pop();
//    }
//  FPixelsStack.push(FTopologicalMap->getFirstPixel(FRegion));
//  this->operator++();
//  FTopologicalMap->markPixel(FCurrentPixel.getX(),FCurrentPixel.getY());
//}
////*****************************************************************************
//void CCoverageRegionPixels::operator++()
//{
//
//if( FPixelsStack.empty() )
//    {
//      FStop = true;
//      return;
//    }
//  
//  FCurrentPixel = FPixelsStack.top();
//  FPixelsStack.pop();
//
//  CDoublet t(FCurrentPixel.getX(),FCurrentPixel.getY(), XPOS);
//  if( !FTopologicalMap->isLCell(t) )
//    {
//      CCoordinate n(FCurrentPixel);
//      n.incY(-1);
//      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
//        {
//          FTopologicalMap->markPixel(n.getX(),n.getY());
//          FPixelsStack.push(n);
//        }
//    }
//  
//  t.setLinel(YPOS);
//  if( !FTopologicalMap->isLCell(t) )
//    {
//      CCoordinate n(FCurrentPixel);
//      n.incX(-1);
//      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
//        {
//          FTopologicalMap->markPixel(n.getX(),n.getY());
//          FPixelsStack.push(n);
//        }
//    }
//      
//  t = CDoublet(FCurrentPixel.getX()+1,FCurrentPixel.getY()+1, XNEG);
//  if( !FTopologicalMap->isLCell(t) )
//    {
//      CCoordinate n(FCurrentPixel);
//      n.incY();
//      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
//        {
//          FTopologicalMap->markPixel(n.getX(),n.getY());
//          FPixelsStack.push(n);
//        }
//    }   
//  
//  t.setLinel(YNEG);
//  if( !FTopologicalMap->isLCell(t) )
//    {
//      CCoordinate n(FCurrentPixel);
//      n.incX();
//      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
//        {
//          FTopologicalMap->markPixel(n.getX(),n.getY());
//          FPixelsStack.push(n);
//        }
//    } 
//}
////*****************************************************************************
//const CCoordinate & CCoverageRegionPixels::operator++(int)
//{
//  this->operator ++();
//  return FCurrentPixel;
//}
////*****************************************************************************
//bool CCoverageRegionPixels::cont()
//{
//  return !FStop;
//}
////*****************************************************************************
//const CCoordinate & CCoverageRegionPixels::operator*()
//{
//  return FCurrentPixel;
//}
//*****************************************************************************
