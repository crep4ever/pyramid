//
//  @ Project : Image Management
//  @ File Name : IM_Lignel.h
//  @ Date : 16/05/2007
//  @ Author : Thibaut Aubrun http://aubrun.thibaut.free.fr

#if !defined(_IM_LIGNEL_)
#define _IM_LIGNEL_

#include <vector>

#include "image.hh"


namespace fogrimmi
{
  //==========================================================
  /// Classe de gestion de lignel - lignel = ligne entre deux pixels \n \n
  /// Cette classe definit un ensemble de lignels représentant une forme quelconque dont les coordonnées sont reliees dans le sens anti-horaire
  /// pour chaque lignel deux pixels adjadcents sont necessaires
  //==========================================================
  class IM_Lignel
  {
  public:

    //----------------------------------------------------------
    /// Constructeur
    //----------------------------------------------------------
    IM_Lignel(unsigned int _index=0, unsigned short int _iGroup = 0, void* _uData = NULL);

    //----------------------------------------------------------
    /// Destructeur
    //----------------------------------------------------------
    ~IM_Lignel();

    //----------------------------------------------------------
    /// Ajoute deux pixels pour former un lignel
    //----------------------------------------------------------
    void          addLignel(IM_Pixel &first, IM_Pixel &second);

    //----------------------------------------------------------
    /// Retourne le premier pixel (pixel exterieur au contour)
    //----------------------------------------------------------
    //first
    IM_Pixel      getFirst(unsigned int lIndex);

    //----------------------------------------------------------
    /// Retourne le deuxieme pixel (pixel interieur au contour)
    //----------------------------------------------------------
    //second
    IM_Pixel      getSecond(unsigned int lIndex);

    //----------------------------------------------------------
    /// Surchage de l'operateur =, permet de transformer une liste de lignel en deque
    //----------------------------------------------------------
    void          operator = ( std::list<lignel> &lPixel);

    //----------------------------------------------------------
    /// Retourne une boite englobant tous les lignels
    //----------------------------------------------------------
    IM_Box        getMinMaxBox();

    //----------------------------------------------------------
    /// Retourne les donnees utilisateurs stockees dans uData
    //----------------------------------------------------------
    void*         getUserData(){ return uData; };

    //----------------------------------------------------------
    /// Modifier les donnees utilisateurs
    //----------------------------------------------------------
    inline void          setUserData(void* _uData){ uData = _uData; };

    //----------------------------------------------------------
    /// Retourne l'index de group
    //----------------------------------------------------------
    inline unsigned short int getGroup(){ return iGroup; };

    //----------------------------------------------------------
    /// Change l'index de group
    //----------------------------------------------------------
    inline void          setGroup(unsigned short int _iGroup){ iGroup = _iGroup; };


    //----------------------------------------------------------
    /// Sauvegarde les lignels dans un fichier binaire - attention a la portabilite - preferer saveTextLignel
    //----------------------------------------------------------
    bool          saveBinLignel     ( const std::string &fileBin = "defaultBinLignel.txt");

    //----------------------------------------------------------
    /// Sauvegarde les lignels dans un fichier texte
    //----------------------------------------------------------
    bool          saveTextLignel     ( const std::string &fileBin = "defaultBinLignel.txt");

    //----------------------------------------------------------
    /// Sauvegarde les lignels dans un fichier xml
    //----------------------------------------------------------
    bool          saveXMLLignel     ( const std::string &fileXML = "defaultXMLLignel.xml" );

    //----------------------------------------------------------
    /// Retourne le nombre de lignels
    //----------------------------------------------------------
    inline unsigned short int getSize(){ return vLignel.size(); };

    //----------------------------------------------------------
    /// Resize l'ensemble de lignel, Si size < taille actuel alors les lignels suivant sont effaces
    //----------------------------------------------------------
    inline void setSize(unsigned short int size){ vLignel.resize(size); };//vLignels.



  private:


    //----------------------------------------------------------
    /// Identifiant de la classe Lignel
    //----------------------------------------------------------
    unsigned int                  index;


    //----------------------------------------------------------
    /// Deque de lignels
    //----------------------------------------------------------
    std::vector<lignel>           vLignel;

    //----------------------------------------------------------
    /// Donnees utilisateurs, utile pour stocker des informations diverses
    //----------------------------------------------------------
    void*                         uData;

    //----------------------------------------------------------
    /// Identifiant de groupe (page pour un tiff)
    //----------------------------------------------------------
    unsigned short int            iGroup;


  };

  inline
    IM_Pixel IM_Lignel::getFirst(unsigned int lIndex)
    {
      return vLignel[lIndex].first;
    };

  inline
    IM_Pixel IM_Lignel::getSecond(unsigned int lIndex)
    {
      return vLignel[lIndex].second;

    };

}//namespace fogrimmi

#endif
