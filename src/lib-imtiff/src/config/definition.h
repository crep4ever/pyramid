//
//
//  @ Project : Image Management
//  @ File Name : IM_Definition.h
//  @ Date : 21/09/2007
//  @ Author : Thibaut Aubrun http://aubrun.thibaut.free.fr
//
// Definition des structures et des types enumerés
//

#ifndef IM_DEFINITION_H
#define IM_DEFINITION_H


#include <sstream>
#include <tiffio.h>

#ifdef _WINDOW_
  #pragma warning(disable:4996)
#endif

namespace fogrimmi
{

  /// Structure - gestion d'un pixel
  struct IM_Pixel
  {
    /// coordonnee X
    unsigned int       x;
    /// Coordonnee Y
    unsigned int       y;
    /// valeurs RGB
    uint8              value[3];
    /// Page contenant le pixel en x et y
    unsigned short int page;
    /// Data utilisateur
    void*              uData;

    /// Constructeur par defaut
    IM_Pixel()
    {
      value[0] = 0;
      value[1] = 0;
      value[2] = 0;
    };

    /// Constructeur a partir de coordonnees \n
    /// [in] _x : coordonnee x \n
    /// [in] _y : coordonnee y \n
    /// [in] _page : page de reference \n
    IM_Pixel(unsigned int _x, unsigned int _y, unsigned short int _page = 0)
    {
      x = _x;
      y = _y;
      page = _page;
      value[0] = 0;
      value[1] = 0;
      value[2] = 0;
    };


    /// Constructeur a partir de coordonnees et de donnees utilisateur \n
    /// [in] _x : coordonnee x \n
    /// [in] _y : coordonnee y \n
    /// [in] _uData : donnees utilisateur \n
    /// [in] _page : page de reference \n
    IM_Pixel(unsigned int _x, unsigned int _y, void* _uData, unsigned short int _page = 0)
    {
      x = _x;
      y = _y;
      page = _page;
      value[0] = 0;
      value[1] = 0;
      value[2] = 0;
      uData = _uData;
    };

    /// permet de modifier les donnees attachees au pixel \n
    /// [in] _uData : donnees utilisateur \n
    void setUserData(void* _uData){ uData = _uData; };


    bool operator == (const IM_Pixel& APixel) const {return x == APixel.x && y == APixel.y;};
    bool operator != (const IM_Pixel& APixel) const {return !operator== ( APixel );};
    
  };

  /// orientation - deprecated
  enum ePixDir
  {
      top = 0,
      left,
      bottom,
      right
  };

  /// definition d'un lignel unitaire
  typedef std::pair<IM_Pixel, IM_Pixel>  lignel;

  /// structure representant un cadre de lecture
  struct IM_Box
  {
    /// coin gauche superieur en X (inclu)
    float  XTop;
    /// coin gauche superieur en Y (inclu)
    float  YTop;
    /// coin droit inferieur en X (exclu)
    float  XBtm;
    /// coin droit inferieur en Y (exclu)
    float  YBtm;

    /// Permet d'afficher facilement sur une sortie quelconque les valeurs du cadre \n
    /// [retval] string : Renvoie la chaine de caractere contenant les coordonnees
    std::string PrintIt()
    {
      std::ostringstream oss;
      oss << "XTop: "<< XTop << " YTop: " << YTop << " XBtm: " << XBtm << " YBtm: " << YBtm;
      return oss.str();
    }
  };

  /// Etat de lecture de l'automate lisant un fichier texte contenant des lignels
  enum eSectionLignel
  {
    GENERAL = 0,
    GROUP,
    INDEX,
    NONE
  };

    /// Structure gerant une tuile a copier lors d'un clonage
   struct component{
      /// constructeur
      component(){};
			//~component(){ free(strip); };
      /// constructeur par copie
      component(const component &_vect){firstTime = _vect.firstTime;};
      ///contient les valeurs des pixels \n a remplacer par une allocation dynamique
      uint8* strip;//[240*240*3];
      ///Identifie la tuile si nous l'avons déjà parcourue ou non
      bool firstTime;
    };


  /// Type enum definissant l'organisation de la page
  enum TIFFORGANISATION
  {
    TILE = 0,
    STRIP = 1
  };

  /// Type de zoom employe
  enum TYPE_ZOOM
  {
    /// zoom au plus proche voisin
    NN = 0,
    /// zoom bilineaire
    BL = 1,
    /// zoom bicubique
    BC = 2
  };


  /// Definition des proprietes d'un tiff
  struct TIFF_Properties
  {
    /// Constructeur par defaut
    TIFF_Properties()
    {
      tif = NULL;
      name = NULL;
      tiffDescription = "No description";
    }

    /// Reference pointer ouvert par libtiff
    TIFF* tif;
    /// nom du fichier tiff
    char* name;

    /// largeur du tiff
    unsigned int width;
    /// hauteur du tiff
    unsigned int height;
    /// largeur d'une tuile
    unsigned int componentWidth;
    /// hauteur d'une tuile
    unsigned int componentHeight;
    /// bits par sample
    unsigned int bps;
    /// mode couleur
    unsigned int colorMode;
    /// photometric config
    unsigned int photomet;
    /// planar config
    unsigned int planar;
    /// type de compression
    unsigned int compression;
    /// qualite du jepg
    unsigned int jpeg_q;
    /// color map
    unsigned int jpeg_cm;
    /// Resolution a utiliser de preference en X et Y pour une application graphique
    float                   xRes, yRes;
    /// Mesure utilise pour les resolution en X et Y
    unsigned short int      resUnit;
    /// Description de l'image
    std::string             tiffDescription;
    /// Organisation des pixels dans la page
    TIFFORGANISATION        pixOrganisation;
    /// Type de donnees contenues dans ce sous fichier
    unsigned short int fileType;

    /// true if tiff properties is valid
    bool isValid()
    {
      if(width<=0 || height <= 0 || componentWidth>=width || componentHeight>=height)
        return false;

      if(name == NULL)
        return false;

      return true;
    }
  };



}//namespace fogrimmi
#endif // DEFINITION_H
