//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Image Management
//  @ File Name : IM_Tiff.h
//  @ Date : 20/04/2007
//  @ Author : Thibaut Aubrun http://aubrun.thibaut.free.fr
//
//  @ Notes: Classe généralisant les images de type TIFF.
//         Permet de manipuler des images TIFF indépendamment de l'organisation interne de l'image (tuile, ligne, compression)


#ifndef _IM_TIFF_H
#define _IM_TIFF_H

#include "export.h"
#include "image.hh"
#include "page.h"
#include <tiffio.h>
#include <vector>
#include "projection.h"
#include "zoom.h"
#include "image-memory.h"

typedef unsigned int uint;

namespace fogrimmi
{
  /// Classe servant a manipuler les images de type tiff - specialisation de la classe Image
  class IM_EXPORT IM_Tiff : public IM_Image
  {
  public:
    /// Constructeur
    ///@param: fichier image source
    IM_Tiff(const CFile& _fileName);

    /// Constructeur par recopie
    ///@param: reference image source
    IM_Tiff(const IM_Tiff& tifSource);

    /// Constructeur a partir d'une image memoire - utile pour ecrire dans un tif une image memoire
    ///@param imgMem : image memoire source
    ///@param _fileName : Fichier image source
    IM_Tiff(IM_ImageMemory& imgMem, const CFile& _fileName);

    /// Destructeur
    ~IM_Tiff();

    /// open: ouvre toutes les pages et recupere les infos puis ouvre le tiffdirectory sur 0
    /// peut lever une exception de type CLoadingFailed
    void open();

    /// open: ouvre toutes les pages et recupere les infos puis ouvre le tiffdirectory sur page
    /// \n peut lever une exception de type CLoadingFailed
    void open(unsigned short int page);

    /// close: fermeture des pages
    void close();

    /// ouvre la prochaine page
    /// peut lever une exception de type CLoadingFailed
    void nextPage();

    /// ouvre la précédente page
    /// peut lever une exception de type CLoadingFailed
    void previousPage();

    /// lit la page courante de la boite courante
    void read();

    /// lit les donnees contenu dans la boite box de la page courante
    /// @param box : cadre de lecture
    void read(IM_Box box);

    /// lit les donnees contenu dans la boite box de la page page
    /// @param box : cadre de lecture
    /// @param page : page de lecture
    void read(IM_Box box, unsigned short int page);

    /// lit les donnees contenu dans la boite box de toutes les pages, \n les dimensions de la boite correspond à la page courante. Des projections successives sont effectuées
    /// @param box : cadre de lecture
    void readProjectivePages(IM_Box box);

    /// lit les valeurs d'un pixel de la page courante
    /// @param pix : reference du pixel a affecter
    /// @return SUCCESS or FAIL
    bool getPixelValue(IM_Pixel &pix);

    inline bool getPixelValue(IM_Pixel &pix, uint APage)
    {
      return vPages[APage]->getPixelValue(pix);
    };

    /// lit les valeurs d'un pixel de la page courante
    /// @param pix : reference du pixel a affecter
    /// @return SUCCESS or FAIL
    bool getForcedPixelValue(IM_Pixel &pix);

    /// Renvoie les donnees d'un composant pour une page donnee
    /// @param page   : index de la page
    /// @param row    : ligne
    /// @param col    : colonne
    /// @return  uint8* : buffer
    uint8* getData(uint page, uint row, uint col);

    /// Retourne toutes les donnees bruts des composants sous forme de tableau a une entree \n - conversion composants en tableau - \n attention à la capacite memoire
    void getAllDataRGB32(uint page, uint8* data);

    /// inutilise pour l'instant
    bool setPixelValue(IM_Pixel &pix);

    /// ecrit les donnees contenu dans la boite courante de la page page vers un fichier
    ///@param fileName : nom du fichier de sortie
    ///@param page : page courante
    void write(const CFile& fileName, unsigned short int page);

    /// ecrit les donnees contenu dans la boite courante de la page courante selon les proprietes tiff
    ///@param tiffP : contient les proprietes du nouveau tiff
    ///@param page : page courante
    void write(unsigned short int page, TIFF_Properties TiffP);

    /// ecrit les donnees contenu dans la boite courante de la page courante vers un fichier
    ///@param fileName : nom du fichier de sortie
    void write(const CFile& fileName);

    /// ecrit les donnees contenu dans la boite courante de la page courante selon les proprietes tiff
    ///@param tiffP : contient les proprietes du nouveau tiff
    void write(TIFF_Properties TiffP);

    /// Zoom
    /// @param ratio  : coefficient multiplicateur
    IM_ImageMemory zoom(float ratioX, float ratioY, TYPE_ZOOM TZ=NN);

    /// crop and zoom
    /// @param ratio  : coefficient multiplicateu
    /// @param TZ     : Type de zoom employé
    /// @return SUCCESS or FAIL
    IM_ImageMemory cropAndZoom(float ratioX, float ratioY, TYPE_ZOOM TZ);

    /// Rotation
    /// @param angle : angle de rotation
    /// @return SUCCESS or FAIL
    bool rotate(float angle);

    // Projection
    /// effectue une projection d'une boîte englobante src sur une page src vers une page destination \n
    /// Attention cette projection ne marche correctement que si les deux pages sont tuilés, ou d'un tuilé vers strip \n
    /// \n
    /// Si l'on fait une projection d'une page strip vers une tile alors un décalage peut apparaître (un pixel ne tombe pas forcèment sur un coin de tuile)
    /// dans l'autre cas il n'y aura aucun problème \n
    /// \n
    /// @param boxSrc   : cadre de l'image source
    /// @param pageSrc  : page source de l'image
    /// @param pageDest  : page destination de l'image
    /// @return  IM_Box : cadre de lecture de la page destination
    IM_Box projection(IM_Box boxSrc, unsigned short int pageSrc, unsigned short int pageDest);

    /// Redimmensionnement
    bool rescale(IM_Image* img, IM_Box sizeBox);

    /// Surcharge de l'operateur () - rends accessible les pixels par (col, row, page)
    IM_Pixel operator()(uint col, uint row, unsigned short int page);

    // extraction des donnees selon une boîte
    /// extraire les informations contenu dans le cadre de lecture, flag permet de choisir sur quels pages executer les projections \n
    /// Cette methode est limitee par la taille du cadre de lecture.
    /// @param box  : cadre de lecture
    /// @param page : page lue
    /// @param flag : definit les pages à extraire
    /// @param fileOut : nom du fichier de sortie
    /// @return SUCCESS or FAIL
    bool extract(IM_Box box, unsigned short int page, int flag, char* fileOut);

    /// extraction des donnees selon un lignel
    /// @param lIndex : index du lignel de reference \n
    /// @param iGroup : groupe de reference (page dans le cas du tiff) \n
    void loadFromLignel(unsigned short int lIndex, unsigned short int iGroup = 0);

    /// extraction des donnees selon un lignel
    void extractFromLignel(unsigned short int lIndex, unsigned short int iGroup = 0, const CFile& fileOut="lignelOut.tif");

    /// Ajoute une image tiff a la page index. En mode 0 un simple ajout est effectue.\n
    /// En mode 1 on supprime et insert la nouvelle page.\n
    /// Note: En mode 1 appeler insert pour plusde clarte.
    /// \n
    /// @param file : tiff a ajouter \n
    /// @param index : index de page \n
    /// @param mode : 0=ajout, 1=insert \n
    void append(char *file, int index, uint mode=0);

    /// Insert une image tiff a la page index. \n
    /// Note: La page index sera supprimée.
    /// \n
    /// @param file : tiff a ajouter \n
    /// @param index : index de page \n
    void insert(char *file, int index);

    /// Remove une page index. \n
    /// Note: La page index sera supprimee.
    /// @param index : index de page
    void remove(int index);

    /// Récuperer les proprietes d'une page
    /// @param index     : index de la page
    /// @return  TIFF_Properties : proprietes tiff de la page
    TIFF_Properties getProperties(int index = 0);

    /// non implemente
    void setPage(unsigned short int index);

    /// Retourne l'index de la page actuellement en lecture
    inline unsigned short int  getCurrentPage() { return currentPage; };

    /// Retourne la largeur de la page specifie
    uint  getPageWidth(unsigned short int page);

    /// Retourne la hauteur de la page specifie
    uint  getPageHeight(unsigned short int page);

    /// Retourne vrai si les coordonnees specifie appartiennent aux dimensions de la page.
    /// @param row : coordonnee y \n
    /// @param col : coordonnee x \n
    /// @param page : page de reference \n
    /// @return SUCCESS or FAIL
    bool holdPage(uint row, uint col, unsigned short int page);

    /// Retourne vrai si les coordonnes specifie appartiennent au cadre de lecture de la page.
    /// @param row : coordonnee y
    /// @param col : coordonnee x
    /// @param page : page de reference
    /// @return SUCCESS or FAIL
    bool holdBoxPage(uint row, uint col, unsigned short int page);

    /// Affiche les tags de la page
    /// @param page : index de page
    void printTagsPage(unsigned short int page);

    /// Recompose une image à partir des lignels \n
    /// L'algorithme d'ecriture repose sur le fait que dès que nous
    /// atteignons une nouvelle tuile dans l'image de sortie et que la tuile
    /// x-1;y-1 existe, alors nous pouvons ecrire x-1;y-1. Si xMax; y-2 existe
    /// et que nous sommes a x=0 alors ecrire la tuile.
    /// @param page     : index de page
    /// @param width    : largeur de la nouvelle image
    /// @param height   : hauteur de la nouvelle image
    /// @param maxSize  : critere de surface exprime en pixels
    /// @param fileOut  : nom de l'image de sortie
    void cloneFromLignel(float width, float height, uint maxSize, unsigned short int page = 0, std::string fileOut = "cloneFromLignel.tif" );

    /// Renvoie le cadre referent de la page de destination d'un pixel d'une page source \n
    /// @param x        : coordonnee en X sur la page source
    /// @param y        : coordonnee en Y sur la page source
    /// @param pageSrc  : index de la page source
    /// @param pageDest : index de la page destination
    /// @return  IM_Box : cadre referent
    IM_Box getPixelReferent(unsigned x, unsigned y, unsigned short int pageSrc, unsigned short int pageDest);

    /// Décharge les donnees de la page. Faire appel à read avant reutilisation
    /// @param page : index de la page
    void unLoad(unsigned short int page);

    /// Projection de lignel d'une page à une autre
    /// @param lIndex     : index du lignel
    /// @param iGroupSrc  : index du groupe , ie de la page source
    /// @param iGroupDest : index du groupe , ie de la page de destination
    void projectionLignel(unsigned short int lIndex, unsigned short int iGroupSrc, unsigned short int iGroupDest);

    /// Retourne la boite de lecture courante de la page
    inline IM_Box getCurrentReadBox(uint page){ return vPages[page]->getCurrentReadBox(); };

    /// Definit la boite de lecture de la page
    inline bool setCurrentReadBox(IM_Box box, uint page){ vPages[page]->setCurrentReadBox(box); return true;};

    /// Si une page est une macro image ou un label alors renvoie faux \n
    /// Tips: isPage appel open(page)
    /// @param page : index de la page
    /// @return  TRUE OR FALSE \n
    bool isPage(unsigned short int page);

    /// Retourne le nombres de pages
    inline unsigned short int getNbPages(){ return nbPages; };

    /// Retourne true si la page est deja lue
    inline bool isLoaded(unsigned short int page)
    {
      return vPages[page]->isLoaded();
    };

    /// Reservation memoire pour le conteneur de composant \n
    /// Permet de reserver une plage memoire - utile en cas d'accès temps réel ou rapide
    /// @param page     : index de page
    /// @param width  : largeur
    /// @param height : hauteur
    void reserve(unsigned short int page, int width, int height);

    /// Arret de tout traitement sur un tiff
    void abort();

    /// Retourne vrai si l'image est organisee en tuile
    /// @param page : index de la page \n
    /// @return Vrai sur page est organisee en tuile \n
    bool isTiled(uint page);

  private:
    /// Pointeur sur le TIFF en lecture de la librairie libtiff
    TIFF* tif;

    /// Vecteur de pages
    std::vector<IM_Page*> vPages;

    /// Compteur de pages
    unsigned short int    nbPages;

    /// Indice la page actuellement en lecture
    unsigned short int    currentPage;

    // Copy une page d'image dans un fichier
    /// Utilser la classe IM_Tools pour faire une simple copie
    /// @deprecated
    /// @param _TiffP   : Proprietes du tiff de sortie - utile pour faire des conversions de compression \n
    /// @param page     : index de la page \n
    void copyPageToFile(TIFF_Properties _TiffP, unsigned short int page);
  };

  inline
    IM_Pixel IM_Tiff::operator()(uint col, uint row, unsigned short int page)
    {
      IM_Pixel pix(col, row);
      if(!vPages[page]->getPixelValue(pix))
	getForcedPixelValue(pix);
      return pix;
    }

  inline
    uint IM_Tiff::getPageWidth(unsigned short int page)
  {
    return vPages[page]->getWidth();
  }

  inline
    uint IM_Tiff::getPageHeight(unsigned short int page)
  {
    return vPages[page]->getHeight();
  }
}//namespace fogrimmi

#endif  //_IM_TIFF_H
