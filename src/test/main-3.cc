#include <iostream>
#include <tiffio.h>
#include "pyramid.hh"

using namespace fogrimmi;
using namespace std;


//------------------------------------------------------------------
int main(int argc, char* argv[])
{
  CImage2D* image = new CImage2D("./img/lena-color.tif");
  //image = image->tilePages();

  for(unsigned int i=0; i<image->getNbPages(); ++i)
    image->writeInTiles(8, 8, i);
  
  return 0;
}
