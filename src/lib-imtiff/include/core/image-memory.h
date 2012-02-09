//
//  @ Project : Image Management
//  @ File Name : IM_ImageMemory.h
//  @ Date : 21/09/2007
//  @ Author : Thibaut Aubrun http://aubrun.thibaut.free.fr
//
//

#if !defined(_IM_IMAGEMEMORY_H_)
#define _IM_IMAGEMEMORY_H_

#include "export.h"
#include "image.hh"



namespace fogrimmi
{

  //==========================================================
  /// Classe servant a manipuler les images en memoire
  /// Cette classe est utile pour calculer les transformations tel un zoom
  //==========================================================

  class IM_EXPORT IM_ImageMemory
  {

  public:

    //----------------------------------------------------------
    /// Constructeur par defaut
    //----------------------------------------------------------
    IM_ImageMemory(){};

    //----------------------------------------------------------
    /// Constructeur
    //----------------------------------------------------------
    IM_ImageMemory(unsigned int _width, unsigned int _height, unsigned short int _colorMode);


    //----------------------------------------------------------
    /// Constructeur par recopie
    //----------------------------------------------------------
    IM_ImageMemory(const IM_ImageMemory& _src);

    //----------------------------------------------------------
    /// Destructeur
    //----------------------------------------------------------
    ~IM_ImageMemory();

    //----------------------------------------------------------
    /// Modifie les valeurs du buffer
    //----------------------------------------------------------
    void fromData(uint8 * _data);

    //----------------------------------------------------------
    /// Retourne le mode couleur utilise (RGB = 3)
    //----------------------------------------------------------
    inline unsigned short int getColorMode(){ return colorMode; };

    //----------------------------------------------------------
    /// Retourne la largeur
    //----------------------------------------------------------
    inline unsigned int getWidth(){ return width; };
    //----------------------------------------------------------
    /// Retourne la hauteur
    //----------------------------------------------------------
    inline unsigned int getHeight(){ return height; };
    //----------------------------------------------------------
    /// Modifie la largeur
    //----------------------------------------------------------
    inline void setWidth(unsigned int _width){ width = _width; };
    //----------------------------------------------------------
    /// Modifie la hauteur
    //----------------------------------------------------------
    inline void setHeight(unsigned int _height){ height = _height; };

    //----------------------------------------------------------
    /// Modifie la valeur d'un pixel
    //----------------------------------------------------------
    void setPixelValue(const IM_Pixel &pix);


    //----------------------------------------------------------
    /// Surcharge de l'operateur () - rends accessible les pixels par (row, col, page)
    //----------------------------------------------------------
    uint8      operator()(unsigned int y, unsigned int x, unsigned short int cm);


    //----------------------------------------------------------
    /// Surcharge de l'operateur =
    //----------------------------------------------------------
    IM_ImageMemory &operator =(const IM_ImageMemory &_src);

    //----------------------------------------------------------
    /// Retourne le buffer
    //----------------------------------------------------------
    uint8* getData(){return data;};

    //----------------------------------------------------------
    /// Retourne le buffer rearrange, BGRA
    //----------------------------------------------------------
    uint8* getDataRGB32();

  private:

    /// Buffer de l'image stockee en ligne
    uint8* data;

    /// largeur
    unsigned int width;
    /// hauteur
    unsigned int height;
    /// mode couleur
    unsigned short int colorMode;

  };

  inline
    uint8 IM_ImageMemory::operator()(unsigned int y, unsigned int x, unsigned short int cm)
    {

      unsigned long int n = colorMode * (width * y + x) + cm;
      return data[n];


    }





}//namespace fogrimmi

#endif // _IM_IMAGEMEMORY_H_
