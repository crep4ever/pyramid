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
#ifndef IMAGETIFF_HH
#define IMAGETIFF_HH
//******************************************************************************
#include "tiff.h"
#include <string>
#include "image2D.hh"
#include "macros.hh"
#include "inline-macro.hh"
#include "CImg.h"
using namespace cimg_library;
//******************************************************************************

/// Type pour stocker la valeur d'un pixel. 
/// Utiliser forcément un non-signé sous peine de problèmes.
typedef uint16_t TLabel;

static const double epsilon = 1e-6;
static const double JND=sqrt(3);

class KMdata;

class CImageTiff : public CImage2D, public fogrimmi::IM_Tiff
{
public:
  enum ColorMode{ Grey, RGB, RGBA };
  enum GreyConvertMode{ Basic, Gamma, Natural };
 
  
  bool FIsOk;    // Vrai si l'image est valide
  uint FNbPages; // Le nombre de pages dans l'image
  uint* FPages ; // Tableau des pages du tif (utile pour le tri)
  uint FColorMode;
  uint FDepth; //current depth

  std::string FFilename;


  /// Constructeur d'une image ImTiff à partir d'un fichier tif.
  // @param AFilename: le nom du fichier
  CImageTiff( const std::string & AFilename );

  /// TODO
  CImageTiff( fogrimmi::IM_ImageMemory& imgMem, const fogrimmi::CFile& _fileName );

  /// Destructeur : détruit les tableaux.
  virtual ~CImageTiff();

  //******************************************************************************
  // Méthodes sur l'image
  //******************************************************************************

  std::string filename(const std::string & AExt = "tif") const;
  void setFilename(const std::string & AFilename);

  /// Accesseur sur la propriété tiff du colorMode pour savoir
  /// si l'image est codée sur 8bits (n&b) ou 24bits(rgb)
  /// @return nombre de canaux de 8bits utilisés
  int colorMode() const;

  uint depth() const;
  void setDepth(uint ADepth);

  /// @param ADepth le niveau d'un tif multipages
  /// @return la largeur de l'image
  uint getXSize(uint ADepth) ;
  
  /// Similaire à getXSize
  /// @param ADepth le niveau d'un tif multipages
  /// @return la largeur de l'image
  uint width(uint ADepth) ;

  /// @return la hauteur de l'image
  uint getYSize(uint ADepth) ;
  
  /// Similaire à getYSize
  /// @param ADepth le niveau d'un tif multipages
  /// @return la largeur de l'image
  uint height(uint ADepth);
  
  /// Lit et charge en mémoire une partie de l'image
  /// @param ABox le cadre de lecture
  /// @param ADepth la page d'un tif multipages
  void read(const fogrimmi::IM_Box & ABox, int ADepth=-1);
     
  /// Lit un pixel d'un tif potentiellement multipages
  /// @param Ax: abscisse
  /// @param Ay: ordonnée
  /// @param ADepth: la page
  /// @return le pixel
  void getPixel(fogrimmi::IM_Pixel & APixel, uint ADepth);

  /// Accesseur sur le niveau de gris d'un pixel
  /// @param APixel: le pixel dont on récupère la valeur
  /// @param AMethod: méthode de conversion
  /// - 0: conversion basique
  /// - 1: correction gamma
  /// - 2: couleurs naturelles
  /// @return le niveau de gris (0-255)
  TLabel getPixelGreyValue( const fogrimmi::IM_Pixel& APixel, GreyConvertMode AMode=Basic) const;

  void openPage(uint ADepth);

  //******************************************************************************
  // Méthodes sur les tif multi-pages
  //******************************************************************************


  /// Tri par bulles des pages d'un tiff multi-pages de la plus faible 
  /// à la plus forte résolution
  void sortPages();

  /// Interpolation des pages d'un tiff multi-pages pour obtenir
  /// des ratios entiers entre les niveaux
  /// @return le tif multipages redimensionné
  CImageTiff* interpolatePagesWithMagick();

  //todo: interpolation en utilisant la lirairie ImTiff
  CImageTiff* interpolatePagesWithImTiff(uint ANbLevel);
  
  //todo: redimensionnent d'une image en utilisant la librairie ImTiff
  CImageTiff* rescale(uint ADepth, uint AWidth, uint AHeight);

  /// Conversion des pages "stripped" (en lignes) vers "tiled"
  /// (en tuiles) d'un tif multipages
  /// @param ATileWidth:  la largeur souhaitée des tuiles
  /// @param ATileHeight: la hauteur souhaitée des tuiles
  /// @return le tif multipages tuilé
  CImageTiff* tilePages(uint ATileWidth=240, uint ATileHeight=240);

  /// k-means algorithm on current image's box
  /// uses SimpleKmeans if openmp is supported and falls back on KMLocal if not
  /// @param ANbClass : the number of clusters
  /// @return a table with the clustering result
  uint8* kmeans(uint ANbClass);

  void kmeansHistogram(CImg<float>* histo, CImg<char>* assignement);

  /// k-means algorithm with KMLocal library
  /// @param ANbClass : the number of clusters
  /// @return a table with the clustering result
  uint8* kmeansKMLocal(uint ANbClass);

  /// k-means algorithm on a part of image with simplekmeans library
  /// @param ANbClass : the number of clusters
  /// @return a table with the clustering result
  uint8* simplekmeans(const uint ANbClass);

  float** omp_kmeans(int, float**, int, int, int, float, int*);

  float euclid_dist_2(int numdims, float *coord1, float *coord2);

  int find_nearest_cluster(int numClusters, int numCoords, float  *object, float **clusters);

  // Labelise à partir d'une classification
  // La classif donne le pixel représentatif d'une classe, que l'on convertit
  // en labels entiers
  void moveClasses(uint8* data, uint size, uint dim=3);

  // tous les pixels de data et de label ACLass2 deviennent des pixels de label ACLass1
  void mergeClasses(int AClass1, int AClass2, uint8* data, uint size);

  //fait un kmeans "partiel" sur les regions d'interet
  uint8* kmeansRegions(uint8* data, uint size, uint ANbClass);

  //fait un kmeans de niveaux de gris à partir d'un excess red sur les regions d'interet
  uint8* kmeansMitosis(uint8* data, uint size, uint ANbClass);

  uint getNbTilesWidth (uint ATileWidth, uint ADepth);
  uint getNbTilesHeight(uint ATileHeight, uint ADepth);

  //******************************************************************************
  // Méthodes utilitaires
  //******************************************************************************


  /// Vérifie que les pages sont triées de la plus faible 
  /// à la plus forte résolution
  /// @return true si le tif multipages est trié
  bool isSorted();

  /// @return vrai ssi l'image est valide (allouée et taille>0).
  bool isOk() const;

  /// @return vrai ssi le pixel Ax,Ay est dans l'image.
  bool isPixelInImage(uint Ax, uint Ay, uint ADepth) ;
  bool isPixelInImage(const fogrimmi::IM_Pixel & APixel, uint ADepth) ;

  /// @return la taille mémoire occupée en octets
  uint getNumberOfByte() const;

  /// Affiche le niveau de gris de tous les pixels de l'image
  void printInfosAllPixels(uint ADepth=0);

  /// Décharge toutes les pages du tiff
  void unload();

  // Récupère tous les pixels de la box de la page dans le tableau data
  uint8* getData();
  
  // ajout d'un canal alpha artificiel pour qt
  uint8* getDataQT();
  
  /// Read pixel data from a box and stores it in a structure
  /// that may be used by km-local
  /// @param data : the data structure where pixel data is storedw
  void getKmeansData(KMdata& data);
    
  /// Read pixel value supposing that pixels are stored as (24bits) RGB values
  /// @param data : the data structure where pixel data is stored
  void getKmeansDataRGB(KMdata& data);

  /// Read pixel value supposing that pixels are stored as (8bits) grey values
  /// @param data : the data structure where pixel data is stored
  void getKmeansDataGrey(KMdata& data);

  /// Vérifie que la profondeur demandée n'excède pas le nombre de pages dans le tif
  /// si c'est le cas, on retourne la résolution la plus importante
  /// @param ADepth un niveau de la pyramide
  /// @return l'index de la page correspondant au niveau demandé
  uint pageFromDepth(uint ADepth) const;

  inline bool isOk(){return true;}


  double distance(const fogrimmi::IM_Pixel & p, const fogrimmi::IM_Pixel & q);
  double poids(const fogrimmi::IM_Pixel & p, const fogrimmi::IM_Pixel & r);
  void regularization(int nb_iteration, int pp,double lambda, int voisinage);
  void colorRegularization(int nb_iteration, int pp,double lambda, int voisinage);
  void greyRegularization(int nb_iteration, int pp,double lambda, int voisinage);

public:
  
  // Cadre de lecture
  uint xstart() const;
  uint xstop() const;
  uint ystart() const;
  uint ystop() const;
  fogrimmi::IM_Box currentBox() const;
  void setCurrentBox(const fogrimmi::IM_Box & box);

  ///@return the number of pixels contained in the current box
  uint nbCurrentBoxPixels() const;

private:
  fogrimmi::IM_Box m_currentBox;
};

#include INCLUDE_INLINE("imageTiff.icc")
#include INCLUDE_INLINE("omp_kmeans.icc")

//******************************************************************************
//******************************************************************************

#endif // IMAGETIFF_HH
