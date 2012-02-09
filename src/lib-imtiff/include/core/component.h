//
//
//  @ Project : Image Management
//  @ File Name : IM_Component.h
//  @ Date : 21/09/2007
//  @ Author : Thibaut Aubrun http://aubrun.thibaut.free.fr
//
//

#if !defined(_IM_COMPONENT_H)
#define _IM_COMPONENT_H

#include <tiffio.h>
#include "image.hh"
#include <vector>
namespace fogrimmi
{
  //==========================================================
  /// Gestion en memoire d'une partie d'image
  //==========================================================
  class IM_Component
  {
  public:

    //----------------------------------------------------------
    /// Destructeur
    //----------------------------------------------------------
    virtual ~IM_Component(){};

    //----------------------------------------------------------
    /// Read a tile (cols x rows) and save it in memory
    //----------------------------------------------------------
    virtual bool read(TIFF& tif, unsigned short int cols, unsigned short int rows, unsigned short int colorMode=3) = 0;

    //----------------------------------------------------------
    /// Write a tile (cols x rows) and save it in memory
    //----------------------------------------------------------
    virtual bool write(TIFF &tif, unsigned short int sizeH, unsigned int sizeW = 0) = 0;


    //----------------------------------------------------------
    /// Retrieve a pixel value at specified location
    //----------------------------------------------------------
    virtual void getPixelValue(IM_Pixel &pix,unsigned short int cols, unsigned short int rows, unsigned short int colorMode=3) = 0;

    //----------------------------------------------------------
    /// retourne le buffer
    //----------------------------------------------------------
    virtual uint8* getData(){return buffer; };


    //----------------------------------------------------------
    /// Modifie la valeur d'un pixel
    //----------------------------------------------------------
    virtual void setPixelValue(const IM_Pixel &pix, unsigned short int sizeH, unsigned int sizeW = 0, unsigned short int colorMode=3) = 0;


    //----------------------------------------------------------
    /// Modifie en une tuile entiere
    //----------------------------------------------------------
    virtual void addData(uint8* _data) = 0;

    //----------------------------------------------------------
    /// Replace un composant
    //----------------------------------------------------------
    inline void setPosition(unsigned short int _col, unsigned short _row){ col = _col; row = _row; };





  protected:

    ///Indice de colonne dans l'image
    unsigned short int col;
    ///Indice de ligne dans l'image
    unsigned short int row;
    ///Buffer
    uint8* buffer;
  };
}//namespace fogrimmi
#endif  //_IM_COMPONENT_H

