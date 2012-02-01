/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//******************************************************************************
#include "inline-macro.hh"
#include "imageTiff.hh"
#include "image2D.hh"
#include "KCtree.h"
#include "KMdata.h"
#include "KMfilterCenters.h"
#include "KMterm.h"
#include "KMlocal.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <valarray>

#include INCLUDE_NON_INLINE("imageTiff.icc")
#include INCLUDE_NON_INLINE("omp_kmeans.icc")

#define UNUSED(x) (void)x

using namespace fogrimmi;
//******************************************************************************
CImageTiff::CImageTiff(const std::string & AFilename) :
  CImage2D()
  , IM_Tiff(AFilename)
  , FNbPages(0)
{
  FNbPages = getNbPages();
  assert(FNbPages>0);

  FFilename = AFilename;

  FPages = new uint[FNbPages];
  for(uint k=0; k<FNbPages; ++k)
    FPages[k]=k;

  switch(getProperties().colorMode)
    {
    case 1:
      FColorMode = Grey;
      break;
    case 3:
      FColorMode = RGB;
      break;
    default:
      std::cout<<"CImageTiff: Warning color encoding not supported : "
	       << getProperties().colorMode << std::endl;
      break;
    }

  FDepth = 0;
}

//------------------------------------------------------------------------------
CImageTiff::CImageTiff( IM_ImageMemory& imgMem, const CFile& _fileName ):
    IM_Tiff( imgMem, _fileName )
{ }

//------------------------------------------------------------------------------
CImageTiff::~CImageTiff()
{
  delete[] FPages;
}

//------------------------------------------------------------------------------
uint8* CImageTiff::kmeansKMLocal(uint ANbClass)
{
  //std::cout<<" [start] CImageTiff::kmeans"<<std::endl;
  // 1. On crée une image et on retrouve les infos qui nous intéressent
  regularization(10, 2, 0.1, 1);

  // 2. On convertit les données pour la quantif
  //todo: pour niveau de gris, faire dim = 1 et pas mettre 3x la même valeur
  int dim = 3; // dimension
  int nPts = nbCurrentBoxPixels(); // number of data points
  KMdata dataPts(dim, nPts);//stockage des points
  KMterm term(50, 0, 0, 0, // run for 10 stages
	      0.10, 0.10, 3, // other typical parameter values
	      0.60, 10, 0.95);

  getKmeansDataRGB(dataPts);

  // 3. Quantification avec kmlocal
  dataPts.buildKcTree();
  KMfilterCenters ctrs(ANbClass, dataPts);
  //KMlocalHybrid kmAlg(ctrs, term); //bug sur petites images
  KMlocalLloyds kmAlg(ctrs, term);
  ctrs = kmAlg.execute();
  KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()];
  double* sqDist = new double[dataPts.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist);
  KMcenterArray centers=ctrs.getCtrPts();
  std::vector< valarray<double> > v;
  std::vector<double> d1,d2;
  d1.resize( ANbClass );
  d2.resize( ANbClass );

  for(uint b=0;b< ANbClass;b++){
    v.push_back(valarray<double>(centers[b],dim));
    d1[b]=d2[b]=((v[v.size()-1] * v[v.size()-1]).sum());
  }

  std::sort(d2.begin(),d2.end());

  for(uint b=0;b< ANbClass;b++){
    d1[b]=find(d2.begin(),d2.end(),d1[b])-d2.begin();
  }

  // 4. On crée l'image de label à partir de la quantification
  //uint8* result = new uint8[(xstop-xstart)*(ystop-ystart)*3];
  uint8* result = new uint8[nPts];
  uint i = 0; uint j = 0;
  for(uint y=ystart(); y<ystop(); ++y)
    for(uint x=xstart(); x<xstop(); ++x)
      {
	//result[i++] = centers[closeCtr[j]][0];
	//result[i++] = centers[closeCtr[j]][1];
	//result[i++] = centers[closeCtr[j]][2];
	//++j;
	//std::cout<< " d1[closeCtr["<<j<<"]] " << d1[closeCtr[j]] << std::endl;
	result[i++] = d1[closeCtr[j++]];
	//result[i++] = d1[closeCtr[j]];
	//result[i++] = d1[closeCtr[j]];
	//++j;
      }
  delete [] closeCtr;
  delete [] sqDist;
  //std::cout<<" [end] CImageTiff::kmeans"<<std::endl;
  return result;
}

//------------------------------------------------------------------------------
uint8* CImageTiff::simplekmeans(const uint ANbClass)
{
  //std::cout<<" [start] CImageTiff::simplekmeans"<<std::endl;
  // 1. On crée une image et on retrouve les infos qui nous intéressent
  //read(ABox, ADepth);
  regularization(10, 2, 0.1, 1);

  // 2. simplekmeans parameters
  //omp_set_num_threads(4);
  double threshold = 0.001;

  int dim = 3; // dimension
  int nPts = nbCurrentBoxPixels();

  float **objects;
  objects = (float**) malloc(nPts * sizeof(float*));
  for(int i = 0; i<nPts; ++i)
    objects[i] = (float*) malloc(3 * sizeof(float));

  IM_Pixel pix; uint i = 0; //uint j = 0;
  for(uint y=ystart(); y<ystop(); ++y)
      {
	pix.y=y;
	for(uint x=xstart(); x<xstop(); ++x)
	  {
	    pix.x = x;
	    getPixel(pix, depth());
	    objects[i][0] = (float) pix.value[0];
	    objects[i][1] = (float) pix.value[1];
	    objects[i][2] = (float) pix.value[2];
	    ++i;
	  }
      }

    // 3. Quantification avec simplekmeans
    int * membership = (int*) malloc(nPts * sizeof(int));
    float **clusters = omp_kmeans(0, objects, dim, nPts,
				  ANbClass, threshold, membership);

    free(objects[0]); free(objects);

//    // 4. On crée l'image de label à partir de la quantification
//    uint8* result = new uint8[nPts];
//    i = 0; j = 0;
//    for(uint y=ystart; y<ystop; ++y)
//      for(uint x=xstart; x<xstop; ++x)
//	{
//	  result[i++] = membership[j++];
//	}
//    free(membership);
    free(clusters[0]); free(clusters);
    //std::cout<<" [end] CImageTiff::simplekmeans"<<std::endl;
    return (uint8*) membership;
}

//------------------------------------------------------------------------------
uint8* CImageTiff::kmeansMitosis(uint8* data, uint size, uint ANbClass)
{
  //std::cout<<" [start] CImageTiff::kmeansMitosis"<<std::endl;
  int dim = 1; // dimension
  int nPts = 0;
  for(uint i=0; i<size; i=i+3)
    if(data[i]!=0 && data[i+1]!=0 && data[i+2]!=0)
      nPts++;

  //std::cout<<" nPts = "<<nPts<<std::endl;

  if(nPts==0)
    return NULL;

  KMdata dataPts(dim, nPts);//stockage des points
  KMterm term(100, 0, 0, 0, // run for 10 stages
	      0.10, 0.10, 3, // other typical parameter values
	      0.60, 10, 0.95);

  for(uint i=0, j=0; i<size; i=i+3)
    if(data[i]!=0 && data[i+1]!=0 && data[i+2]!=0)
	dataPts[j++][0] = 2*data[i] - data[i+1] - data[i+2];

  // 3. Quantification avec kmlocal
  dataPts.buildKcTree();
  KMfilterCenters ctrs(ANbClass, dataPts);
  KMlocalLloyds kmAlg(ctrs, term);
  ctrs = kmAlg.execute();
  KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()];
  double* sqDist = new double[dataPts.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist);

  // 4. On crée l'image de label à partir de la quantification
  uint8* result = new uint8[size/3];
  bzero(result, size/3*sizeof(uint8));
  for(uint i=0, j=0, k=0; i<size; i=i+3)
    {
      if(data[i]!=0 && data[i+1]!=0 && data[i+2]!=0)
	{
	  //std::cout<<"closeCtr["<<j<<"] = "<<closeCtr[j]<<std::endl;
	  //std::cout<<"centers closeCtr["<<j<<"] = "<<centers[closeCtr[j]][0]<<std::endl;
	  result[k] = closeCtr[j++]+1;
	}
      k++;
    }

  delete [] data;
  delete [] closeCtr;
  delete [] sqDist;
  //std::cout<<" [end] CImageTiff::kmeansMitosis"<<std::endl;
  return result;
}

//------------------------------------------------------------------------------
uint8* CImageTiff::kmeansRegions(uint8* data, uint size, uint ANbClass)
{
  //std::cout<<" [start] CImageTiff::kmeansRegions"<<std::endl;
  int dim = 3; // dimension
  int nPts = 0;
  for(uint i=0; i<size; i=i+3)
    if(data[i]!=0 && data[i+1]!=0 && data[i+2]!=0)
      nPts++;

  if(nPts==0)
      return NULL;

  KMdata dataPts(dim, nPts);//stockage des points
  KMterm term(100, 0, 0, 0, // run for 10 stages
	      0.10, 0.10, 3, // other typical parameter values
	      0.60, 10, 0.95);

  for(uint i=0, j=0; i<size; i=i+3)
    if(data[i]!=0 && data[i+1]!=0 && data[i+2]!=0)
      {
	dataPts[j][0] = data[i];
	dataPts[j][1] = data[i+1];
	dataPts[j][2] = data[i+2];
	++j;
      }

  // 3. Quantification avec kmlocal
  dataPts.buildKcTree();
  KMfilterCenters ctrs(ANbClass, dataPts);
  KMlocalLloyds kmAlg(ctrs, term);
  ctrs = kmAlg.execute();
  KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()];
  double* sqDist = new double[dataPts.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist);
  KMcenterArray centers=ctrs.getCtrPts();

  // 4. On crée l'image de label à partir de la quantification
  for(uint i=0, j=0; i<size; i=i+3)
    if(data[i]!=0 && data[i+1]!=0 && data[i+2]!=0)
      {
	data[i]   = centers[closeCtr[j]][0];
	data[i+1] = centers[closeCtr[j]][1];
	data[i+2] = centers[closeCtr[j]][2];
	++j;
      }

  delete [] closeCtr;
  delete [] sqDist;
  //std::cout<<" [end] CImageTiff::kmeansRegions"<<std::endl;
  return data;
}

//------------------------------------------------------------------------------
void CImageTiff::colorRegularization(int nb_iteration,
				     int pp,
				     double lambda,
				     int voisinage)
{
  //std::cout<<"[start] CImageTiff::colorRegularization in level  "<< ADepth <<std::endl;
  IM_Pixel p, q;
  int k,l;
  double X,Y,Z,sum;
  double _value;
  double val=0.,_val=0.;
  bool end=false;
  double sumChange=0.,sumK=0.;
  valarray<double> sums=valarray<double>(0.,6);

  while (nb_iteration > 0 && !end)
    {
      for (uint y = ystart(); y < ystop() ; ++y)
	{
	  p.y = y;
	  for (uint x = xstart(); x <xstop(); ++x)
	    {
	      p.x = x;
	      getPixel(p, depth());
	      sums[0]+=p.value[0];
	      sums[1]+=p.value[1];
	      sums[2]+=p.value[2];
	      sums[3]+=p.value[0]*p.value[0];;
	      sums[4]+=p.value[1]*p.value[1];;
	      sums[5]+=p.value[2]*p.value[2];;
	      X=Y=Z=_value=sum=_val=0.;
	      for(k=-voisinage; k<=voisinage; ++k)
		for(l=-voisinage; l<=voisinage; ++l)
		  {
		    q.x=p.x+l;
		    q.y=p.y+k;
		    if (isPixelInImage(q, depth()) && p!=q)
		      {
			getPixel(q, depth());
			_value=poids(p,q);
			sum  += _value;
			X += _value*q.value[0];
			Y += _value*q.value[1];
			Z += _value*q.value[2];
			_val+=_value*distance(p,q); //distance euclidienne
		      }
		  }
	      _value=(lambda+sum);
	      if (_value==0)
		_value=epsilon;

	      p.value[0] = ((lambda)*p.value[0]/_value+X/_value) < 255 ? (lambda)*p.value[0]/_value+X/_value : 255;
	      p.value[1] = ((lambda)*p.value[1]/_value+X/_value) < 255 ? (lambda)*p.value[1]/_value+X/_value : 255;
	      p.value[2] = ((lambda)*p.value[2]/_value+X/_value) < 255 ? (lambda)*p.value[2]/_value+X/_value : 255;
	      setPixelValue(p);

	      sumChange += fabs(p.value[0]-p.value[0]) + fabs(p.value[1]-p.value[1]) + fabs(p.value[2]-p.value[2]);
	      sumK += fabs(p.value[0]) + fabs(p.value[1]) + fabs(p.value[2]);
	      _val *= sqrt( SQR(p.value[0]-p.value[0]) + SQR(p.value[1]-p.value[1]) + SQR(p.value[2]-p.value[2]) );
	      val+=_val;
	    }
	}
      sumChange/=sumK;
      if (sumChange<epsilon)
	end=true;
      nb_iteration--;
    }
  //std::cout<<"[end] CImageTiff::colorRegularization in level  "<< depth() <<std::endl;
}

//------------------------------------------------------------------------------
void CImageTiff::greyRegularization(int nb_iteration,
				    int pp,
				    double lambda,
				    int voisinage)
{
  std::cout<<"CImageTiff::greyRegularization in level  "<< depth() <<std::endl;
  assert(false);
}

//------------------------------------------------------------------------------
CVolume<uint8_t>* CImageTiff::kmeansHistogram(CVolume<uint>* histo)
{
  //std::cout<<"[start] CImageTiff::kmeansHistogram \n";
  int dim = 3; // dimension

  int nPts = 0; // number of data points
  for(uint r = 0; r < histo->getSizeX(); ++r)
    for(uint g = 0; g < histo->getSizeY(); ++g)
      for(uint b = 0; b < histo->getSizeZ(); ++b)
	nPts += histo->getValue(r,g,b);

  if(nPts==0) return 0;

  //std::cout<<"[start] CImageTiff::kmeansHistogram computing kmeans on " << nPts <<" data points \n";
  KMdata dataPts(dim, nPts);//stockage des points
  KMterm term(20, 0, 0, 0, // run for 20 stages  // diminuer le nombre de passe (100 aller jusqu a 10)
	      0.10, 0.10, 3, // other typical parameter values
	      0.50, 10, 0.95);
  uint n=0;
  for(uint r = 0; r < histo->getSizeX(); ++r)
    for(uint g = 0; g < histo->getSizeY(); ++g)
      for(uint b = 0; b < histo->getSizeZ(); ++b)
	{
	  int value = histo->getValue(r,g,b);
	  for (int i=0 ; i < value ; ++i)
	    {
	      dataPts[n][0]=r;
	      dataPts[n][1]=g;
	      dataPts[n][2]=b;
	      ++n;
	    }
	}
  delete histo; histo=NULL;
  dataPts.buildKcTree();

  KMfilterCenters ctrs(dim, dataPts);
  KMlocalSwap kmAlg(ctrs, term);
  ctrs = kmAlg.execute();
  KMctrIdxArray closeCtr = new KMctrIdx[nPts];
  double* sqDist = new double[nPts];
  ctrs.getAssignments(closeCtr, sqDist);
  KMcenterArray centers = ctrs.getCtrPts();

  vector<valarray<double> > v;
  vector <double> d1, d2;
  d1.resize(3); d2.resize(3);

  for (int b=0 ; b < 3 ; b++){
    v.push_back(valarray < double> (centers[b], dim));
    d1[b]= d2[b]= ((v[v.size()-1]*v[v.size()-1]).sum());
  }
  std::sort(d2.begin(), d2.end());

  //std::cout << " == D2 == " << std::endl;
  for (int b=0 ; b < 3 ; b++)
    std::cout << d2[b] << " ";
  std::cout << std::endl;


  for (int b=0 ; b < 3 ; b++)
    d1[b] = find(d2.begin(), d2.end(),d1[b]) - d2.begin();

  //std::cout << " == D1 == " << std::endl;
  for (int b=0 ; b < 3 ; b++)
    std::cout << d1[b] << " ";
  std::cout << std::endl;

  CVolume<uint8_t>* assignment = new CVolume<uint8_t>(256,256,256);
  assignment->fill(1);
  int r,g,b;
  for ( int i = 0 ; i < nPts; ++i)
    {
      r =  dataPts[i][0];
      g =  dataPts[i][1];
      b =  dataPts[i][2];
      //move+merge classes
      //std::cout<< " d1[closeCtr["<<i<<"]] " << d1[closeCtr[i]] << std::endl;
      assignment->setValue(r,g,b, (d1[closeCtr[i]]==0) ? 1 : d1[closeCtr[i]]);
      //std::cout<< " assign("<<r<<","<<g<<","<<b<<") ="<< (int)(*assignement)(r,g,b) << std::endl;
    }

  delete[] sqDist;
  delete[] closeCtr;

  return assignment;
  //std::cout<<"[end] CImageTiff::kmeansHistogram \n";
}


//------------------------------------------------------------------------------
bool CImageTiff::isSorted()
{
  for(uint k=0; k<FNbPages-1; ++k)
    if ( (getXSize(k)  * getYSize(k)) >
	 (getXSize(k+1)* getYSize(k+1)) )
      return false;

  return true;
}

//------------------------------------------------------------------------------
void CImageTiff::printInfosAllPixels(uint APage)
{
  std::cout<<" Affichage des infos pixels de la profondeur " << APage <<std::endl;

  IM_Pixel pix;

  for(uint y = 0; y < getYSize(APage); ++y)
    {
      pix.y=y;
      for(uint x = 0; x < getXSize(APage); ++x)
	{
	  pix.x=x;
	  getPixel(pix, APage);
	  std::cout<<"Pixel ("<<x<<","<<y<<") = "<<getPixelGreyValue(pix)<<std::endl;
	}
    }
  std::cout<<std::endl;
}

//******************************************************************************

