//*****************************************************************************
#include "coverage-region-pixels.hh"
#include "region.hh"
using namespace Map2d;
//*****************************************************************************
CCoverageRegionPixels::CCoverageRegionPixels(CTopologicalMap* ATopologicalMap, 
					     CRegion* ARegion):
  FTopologicalMap(ATopologicalMap),
  FRegion(ARegion),
  FStop(false)
{
  if( ARegion->isInfiniteRegion() ) return;

  FPixelsStack.push(FTopologicalMap->getFirstPixel(ARegion));
  this->operator++();
  FTopologicalMap->markPixel(FCurrentPixel.getX(),FCurrentPixel.getY());
}
//*****************************************************************************
CCoverageRegionPixels::~CCoverageRegionPixels()
{
  FTopologicalMap->unmarkAllPixels();
}
//*****************************************************************************
void CCoverageRegionPixels::reinit()
{
  FStop = false;
  FTopologicalMap->unmarkAllPixels();
  while(!FPixelsStack.empty())
    {
      FPixelsStack.pop();
    }
  FPixelsStack.push(FTopologicalMap->getFirstPixel(FRegion));
  this->operator++();
  FTopologicalMap->markPixel(FCurrentPixel.getX(),FCurrentPixel.getY());
}
//*****************************************************************************
void CCoverageRegionPixels::operator++()
{
  if( FPixelsStack.empty() )
    {
      FStop = true;
      return;
    }
  
  FCurrentPixel = FPixelsStack.top();
  FPixelsStack.pop();

  CDoublet t(FCurrentPixel.getX(),FCurrentPixel.getY());
  t.setLinel(XPOS);
  if( !FTopologicalMap->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setY(FCurrentPixel.getY()-1);
      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
	{
	  FTopologicalMap->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    }   
  
  t.setLinel(YPOS);
  if( !FTopologicalMap->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setX(FCurrentPixel.getX()-1);
      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
	{
	  FTopologicalMap->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    }
      
 t = CDoublet(FCurrentPixel.getX()+1,FCurrentPixel.getY()+1);
 t.setLinel(XNEG);
  if( !FTopologicalMap->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setY(FCurrentPixel.getY()+1);
      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
	{
	  FTopologicalMap->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    }   
  
  t.setLinel(YNEG);
  if( !FTopologicalMap->isLCell(t) )
    {
      CPoint2D n(FCurrentPixel);
      n.setX(FCurrentPixel.getX()+1);
      if(!FTopologicalMap->isPixelMarked(n.getX(),n.getY()))
	{
	  FTopologicalMap->markPixel(n.getX(),n.getY());
	  FPixelsStack.push(n);
	}
    } 
}
//*****************************************************************************
const CPoint2D & CCoverageRegionPixels::operator++(int)
{
  this->operator ++();
  return FCurrentPixel;
}
//*****************************************************************************
bool CCoverageRegionPixels::cont()
{
  return !FStop;
}
//*****************************************************************************
const CPoint2D & CCoverageRegionPixels::operator*()
{
  return FCurrentPixel;
}
//*****************************************************************************
