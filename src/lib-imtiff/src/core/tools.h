//
//  @ Project : Image Management
//  @ File Name : IM_Tools.h
//  @ Date : 24/09/2007
//  @ Author : Thibaut Aubrun http://aubrun.thibaut.free.fr
//
//

#ifndef _IM_TOOLS_H_
#define _IM_TOOLS_H_

#include <tiffio.h>
#include "definition.h"
#include "file.h"
#include "sax-parser.h"
#include "dom-parser.h"
#include <fstream>

namespace fogrimmi
{
  //----------------------------------------------------------
  /// Outils de manipulation
  //----------------------------------------------------------
  class IM_EXPORT IM_Tools
  {
  public:
    //----------------------------------------------------------
    /// Constructeur
    //----------------------------------------------------------
    IM_Tools();
    //----------------------------------------------------------
    /// Destructeur
    //----------------------------------------------------------
    ~IM_Tools(){};

    //----------------------------------------------------------
    /// recompose une image a partir de donnees stockees dans un fichier XML: noms images + position
    //----------------------------------------------------------
    bool recomposeByXML(const CFile& fileXML);

    //----------------------------------------------------------
    /// Ecris une image
    //----------------------------------------------------------
    void write(TIFF* tifDest, const CFile& imgName);

    //----------------------------------------------------------
    /// Ouverture en ecriture
    //----------------------------------------------------------
    bool openWrite(TIFF_Properties &tiffP);


    //----------------------------------------------------------
    /// construit un tif tuile multi page a partir d'une image de base
    //----------------------------------------------------------
    void constructMultiPage(const CFile &fileIn, const CFile &fileOut, unsigned int _nbPages, float ratio, TYPE_ZOOM TZ = NN);




    //----------------------------------------------------------
    /// Split les pages du tiff a n pages dans n fichier commencant par prefix
    //----------------------------------------------------------
    void           split(char* fileN, char* prefix);

    //----------------------------------------------------------
    /// Copie une page vers un fichier
    //----------------------------------------------------------
    void           copyOnePage(unsigned int page, char* fileN, char* prefix);

    //----------------------------------------------------------
    /// Ajoute une page a la fin du tiff
    //----------------------------------------------------------
    void          append(const char* fileName, char* fileAdd);

  private:

    //----------------------------------------------------------
    /// Fonction de nommage incrementale
    //----------------------------------------------------------
    void           newfilename(void);

    //----------------------------------------------------------
    /// Lance la copie des composants
    //----------------------------------------------------------
    int            tiffcp(TIFF* in, TIFF* out);

    //----------------------------------------------------------
    /// Copie les strips
    //----------------------------------------------------------
    int            cpStrips(TIFF* in, TIFF* out);

    //----------------------------------------------------------
    /// Copie les tiles
    //----------------------------------------------------------
    int            cpTiles(TIFF* in, TIFF* out);

    //----------------------------------------------------------
    /// Copie les tags
    //----------------------------------------------------------
    void           cpFields(TIFF* in, TIFF *out);

  };

} //fogrimmi

#endif //_IM_TOOLS_H_
