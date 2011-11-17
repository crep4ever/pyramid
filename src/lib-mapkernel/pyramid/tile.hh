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
#ifndef TILE_HH
#define TILE_HH
//******************************************************************************
#include "point2d.hh"
#include "kernel-types.hh"
#include "volume.hh"
#include "khalimsky.hh"
#include "matrix.hh"

#include <map>
#include <stack>
#include <deque>

#include "topological-map.hh"
#include "pyramidal-region.hh"
#include "imageTiff.hh"
#include "matrix.hh"
#include "dp-maxrow.hh"
#include "heap.hh"
#include "chrono.hh"
#include "macros.hh"
//******************************************************************************

/**
 * \file tile.hh
 *
 * Class for topological tiles
 *
 * \author Romain Goffe
 */

using namespace fogrimmi;

class CImageTiff;

namespace Map2d
{
  /** 
   * \class CTile "tile.hh"
   * \brief CTile is a local topological map which corresponds
   * to a subdivision of an image
   */

  // Load/Swap related structures
  struct SDart
  {
    TDartId id;
    uint beta1;
    uint beta2;
    CDoublet doublet;
    bool fictive;
    TRegionId region;
    uint up;
    uint down;
  };

  struct SRegion
  {
    TRegionId id;
    CPoint2D  firstPixel;
    uint nbPixels;
    uint greySum;
    uint greySquareSum;
    uint up;
    uint down;
    uint greyMin;
    uint greyMax;
    uint label;
    uint father;
    uint son;
    uint brother;
    uint cc;
    TDartId representative;
    bool    infinite;
  };

  struct SProperties
  {
    uint id;
    CPoint2D  bottomRight;
    uint indexI;
    uint indexJ;
    uint indexK;
    uint up;
    uint down;
    uint nbDarts;
    uint nbRegions;
  };

  typedef std::vector<CDoublet> CEdge;

  class CArray;
  class CDart;
  class CRegion;
  class CPyramidalDart;
  class CTopologicalDart;
  
  class CTile: public CTopologicalMap
  {

  public:
    enum SystemMode {Absolute, Relative};
    //enum ProjectionMode {Standard, Dijkstra};
    //enum SubdivisionMode {ConstantTileSize, ConstantTileNumber};
    //enum SameRegionOracle {GreyValue, Threshold, Classif, Histology, Disabled};


    uint FMergeThreshold;
    TDartId FCount;
    TRegionId FRegionCount; //compteur qui ne décremente pas -> inutile ?
    uint FPixIndex;
    CMatrix<bool>* FMark;
    CMatrix<CRegion*>* FMatrixPixelRegion;
    CArray* FMatrixLignelDart;
  protected:
    // Propriétés
    uint FId; // Identifiant de la tuile
    CPoint2D FBottomRight; // Point bas/droite
    uint FWidth;
    uint FHeight;

    // Hiérachie
    CTile* FTileUp;
    CTile* FTileDown;
    
    // Swap
  public:
    struct SProperties* FProperties;
    struct SProperties* FOldProperties;
    TKhalimskyElt*  FMatrix;
    struct SDart*   FDartFields;
    struct SDart*   FOldDarts;
    struct SRegion* FRegionFields;
    struct SRegion* FOldRegions;
    bool FFirst;
    std::string FFilename;

    //Extraction
    uint8* FClassif;
    CImg<char>* FAssignment;
  public:
    // Informations nécessaires pour certains traitements
    std::vector<CDart*> FCorners; // Coins de la tuile
    std::map<TDartId, CPyramidalDart*> FMapDarts; // Association id/brin
    std::map<TRegionId, CRegion*> FMapRegions; // Association id/region
    std::map<TDartId, TDartId> FMapBeta2; // Connaitre les beta2 des brins du bord (beta2')
    uint FIndex[3]; //coordonnées i,j,k
     
    // Constructeurs
    CTile(uint AWidth, uint AHeight);

    // Destructeur
    virtual ~CTile();


    //******************************************************************************	  
    //Accesseurs
    //******************************************************************************    


    uint id() const;
    void setId (uint AId);

    uint width() const;
    void setWidth (uint AWidth);

    uint height() const;
    void setHeight (uint AHeight);

    CTile* tileUp() const;
    void setTileUp(CTile* ATile);

    CTile* tileDown() const;
    void setTileDown(CTile* ATile);
    
    uint upRatioX() const;
    uint upRatioY() const;
    CPoint2D upRatio() const;
    uint downRatioX() const;
    uint downRatioY() const;
    CPoint2D downRatio() const;

    std::vector<CDart*> corners() const;
    void setCorners (std::vector<CDart*>& ACorners);

    std::map<TDartId, CPyramidalDart*> mapDarts() const;
    void setMapDarts(std::map<TDartId, CPyramidalDart*>& AMap);

    std::map<TRegionId, CRegion*> mapRegions() const;
    void setMapRegions(std::map<TRegionId, CRegion*>& AMap);

    std::map<TDartId, TDartId> mapBeta2() const;
    void setMapBeta2(std::map<TDartId, TDartId>& AMapBeta2);

    uint index(uint APos) const;
    void setIndex (uint APos, uint AValue);

    /// Convenience method
    /// @return la profondeur de la tuile (voir index(2))
    uint depth() const;

    uint mergeThreshold() const;
    void setMergeThreshold( uint AMergeThreshold );

    CImageTiff* image() const;
    void setImage( CImageTiff* AImage );

    CKhalimsky* getKhalimsky() const;
    CKhalimsky* khalimsky() const;
    void setKhalimsky( CKhalimsky* AKhalimsky );

    CPyramidalRegion* getInclusionTreeRoot() const;
    void setInclusionTreeRoot(CRegion* ARegion);

    /// Accesseur sur les marques des pixels
    void setPixelMark();
    virtual TRegionId getPixel(uint x, uint y) const;
    virtual bool isPixelMarked(uint Ax, uint Ay) const;
    virtual void setPixelMark(uint Ax, uint Ay, bool AOn);
    virtual void markPixel(uint Ax, uint Ay);
    virtual void unmarkPixel(uint Ax, uint Ay);
    virtual void unmarkAllPixels();

    CPoint2D  bottomRight() const;
    void setBottomRight (const CPoint2D& APoint);

    CPoint2D  bottomLeft() const;
    CPoint2D  topRight() const;
    CPoint2D  topLeft() const;
    
    //******************************************************************************	  
    // Méthodes utilitaires
    //******************************************************************************    

    /// Supprime une région
    virtual void delRegion(CRegion* ARegion);

    /// Vide la carte associée à la tuile
    void emptyMap();

    void loadImage();

    uint xmin(const SystemMode & ASystemMode=Absolute) const;
    uint xmax(const SystemMode & ASystemMode=Absolute) const;
    uint ymin(const SystemMode & ASystemMode=Absolute) const;
    uint ymax(const SystemMode & ASystemMode=Absolute) const;

    /// @return true s'il existe une tuile Up
    bool existTileUp() const;

    /// @return true s'il existe une tuile Down
    bool existTileDown() const;

    /// Ajoute un brin au vecteur des coins de la tuile
    /// @param ADart: la brin à rajouter
    void addCorner (CDart* ADart);

    /// Return true si le fichier existe
    bool fileExist(const char* AFilename);

    IM_Box boundingBox(const SystemMode & ASystem=Absolute);

    /// Teste l'appartenance d'un pointel à la tuile
    /// @param ADoublet: un doublet désignant un pointel
    /// @param ADoublet: inclusion stricte (bord exclus)
    /// @param ASystemMode: référentiel (enum SystemMode)
    /// @return vrai si le pointel appartient à la tuile
    bool isPointelInTile(const CDoublet & ADoublet, bool AStrict, const SystemMode & ASystemMode) const;

    /// Teste l'appartenance d'un doublet à la tuile
    /// @param ADoublet: un doublet (pointel+lignel)
    /// @param ADoublet: inclusion stricte (bord exclus)
    /// @param ASystemMode: référentiel (enum SystemMode)
    /// @return vrai si le doublet appartient à la tuile
    bool isDoubletInTile(const CDoublet & ADoublet, bool AStrict, const SystemMode & ASystemMode) const;

    /// Teste l'appartenance d'un pixel à la tuile
    /// @param x: abscisse
    /// @param y: ordonnée
    /// @param ASystemMode: référentiel
    /// @return vrai si le pixel appartient à la tuile
    bool isPixelInTile(uint x, uint y, const SystemMode & ASystemMode) const;

    /// Teste l'appartenance d'un pixel au bord de la tuile
    /// @param x: abscisse
    /// @param y: ordonnée
    /// @param ASystemMode: référentiel
    /// @return vrai si le pixel appartient au bord
    bool isPixelOnBorder(uint x, uint y, const SystemMode & ASystemMode) const;

    /// Teste l'appartenance d'un pointel au bord de la tuile
    /// @param ADoublet: un doublet désignant un pointel
    /// @param ASystemMode: référentiel (enum SystemMode)
    /// @return vrai si le pointel appartient au bord
    bool isPointelOnBorder(const CDoublet & ADoublet, const SystemMode & ASystemMode) const;

    /// Teste l'appartenance d'un pointel à l'une des quatre extrémités de la tuile
    /// @param ADoublet: un doublet désignant un pointel
    /// @param ASystemMode: référentiel (enum SystemMode)
    /// @return vrai si le pointel est un des 4 coins
    bool isPointelACorner(const CDoublet & ADoublet, const SystemMode & ASystemMode) const;

    /// Teste l'appartenance d'un lignel au bord de la tuile
    /// @param ADoublet: un doublet désignant un lignel
    /// @param ASystemMode: référentiel (enum SystemMode)
    /// @return vrai si le lignel appartient au bord de la tuile
    bool isLinelOnBorder(const CDoublet & ADoublet, const SystemMode & ASystemMode) const;

    /// Récupère les deux pixels incidents à un même lignel
    /// (le lignel n'appartient pas au bord de la tuile)
    /// @param ADoublet: le doublet désignant le lignel
    /// @param APix1: le premier pixel incident
    /// @param APix2: le second pixel incident
    /// @param ASystemMode: référentiel (enum SystemMode)
    void incidentPixelsForLinel(const CDoublet & ADoublet, 
				IM_Pixel& APixel1, IM_Pixel& APixel2, 
				const SystemMode & ASystemMode);

    ///Conversion des coordonnees de la tuile fille vers la tuile mere
    CPoint2D coordinateInParent(uint x, uint y, const SystemMode & ASystemMode) const;
    CPoint2D coordinateInParent(const CPoint2D& APoint, const SystemMode & ASystemMode) const;

    ///Conversion des coordonnees de la tuile fille vers la tuile mere
    CPoint2D coordinateFromParent(uint x, uint y, SystemMode ASystemMode) const;
    CPoint2D coordinateFromParent(const CPoint2D& APoint, SystemMode ASystemMode) const;
    CPoint2D coordinateFromParent(const CDoublet& ADoublet, SystemMode ASystemMode) const;

    //******************************************************************************	  
    // Méthodes sur la carte de la tuile
    //******************************************************************************    

    /// Active/désactive un pointel 
    /// dans la matrice de Khalimsky de la carte
    /// @param ABool: allume/éteint
    /// @param ADoublet: désigne le pointel
    void setPCell(const CDoublet& ADoublet, bool ABool = true);

    /// Marque/démarque un pointel 
    /// dans la matrice de Khalimsky de la carte
    /// @param ABool: allume/éteint
    /// @param ADoublet: désigne le pointel
    void setFictivePCell(const CDoublet& ADoublet, bool ABool = true);

    /// Test pour savoir si un pointel est allumé
    /// @param ADoublet: désigne le pointel
    /// @return true si le pointel est allumé
    //bool isPCell(const CDoublet& ADoublet) const;

    /// Test pour savoir si un pointel est marqué
    /// @param ADoublet: désigne le pointel
    /// @return true si le pointel est marqué
    bool isFictivePCell(const CDoublet& ADoublet) const;

    /// Active/désactive un lignel
    /// dans la matrice de Khalimsky de la carte
    /// @param ABool: allume/éteint
    /// @param ADoublet: désigne le lignel
    void setLCell(const CDoublet& ADoublet, bool ABool = true);

    /// Marque/démarque un lignel
    /// dans la matrice de Khalimsky de la carte
    /// @param ABool: allume/éteint
    /// @param ADoublet: désigne le lignel
    void setFictiveLCell(const CDoublet& ADoublet, bool ABool = true);

    /// Test pour savoir si un lignel est allumé
    /// @param ADoublet: désigne le pointel
    /// @return true si le lignel est allumé
    //bool isLCell(const CDoublet& ADoublet) const;

    /// Test pour savoir si un lignel est marqué
    /// @param ADoublet: désigne le lignel
    /// @return true si le lignel est marqué
    bool isFictiveLCell(const CDoublet& ADoublet) const;

    //******************************************************************************	  
    // Méthodes pour le split des tuiles
    //******************************************************************************    

    /// Eclatement en brins unitaires du bord de la tuile
    void splitBorder();
    
    /// Création d'une grille de lignels d'après les ratios de changement de niveau
    /// Si la tuile est 4x plus grande que sa up, on la découpe en 4 avec 2 arêtes
    /// @return l'ensemble des lignels formant le motif de division
    void createMask(std::vector<CDoublet>& AMask);

    /// Applique un ensemble de lignels sur la géométrie d'une tuile.
    /// Les lignels/pointels du masque ne sont pas allumés, 
    /// ils sont simplement marqués comme fictifs
    /// @param AMask: l'ensemble des doublets désignant les éléments fictifs
    void applyMask(std::vector<CDoublet>& AMask);

    /// Parcours des arêtes de la tuile et insertion de sommet sur tous les
    /// pointels fictifs trouvés.
    void insertVertexOnFictivePointels();

    /// Pour chaque lignel du masque, on crée une arête élémentaire (2 brins pointant
    /// vers eux-même). Les lignels du masque sont allumés (fictifs->faux, allumés->vrai).
    /// @param AMask: l'ensemble des doublets désignant les éléments fictifs
    void createMaskUnitEdges(std::vector<CDoublet>& AMask);

    /// Parcours du masque et coutures topologiques autour des sommets
    /// @param AMask: l'ensemble des doublets désignant les éléments fictifs
    void sewMapWithMask(std::vector<CDoublet>& AMask);

    /// Opérations de coutures des brins autour d'un sommet
    /// @param APointel: le sommet
    /// @param AArray: un tableau de correspondance brins/doublets pour les brins à traiter
    /// @param AMark: la marque qui désignera les brins à supprimer (sommets de d°2)   
    void sewAroundPointel(const CDoublet& APointel, const CArray* AArray, int AMark);

    /// todo
    /// La tuile est splittée en autant de tuiles filles qu'il y a de subdivisions dans le masque
    void createSons();

    /// todo
    /// La tuile fille reçoit les brins qui correspondent à son espace géométrique dans la tuile mère
    ///@param ASon: la tuile qui reçoit les brins
    void shareDartsWithSon(CTile* ASon);
    

    //******************************************************************************	  
    // Méthodes pour la construction par extraction
    //******************************************************************************    

    /// Construit une nouvelle tuile avec une segementation plus fine d'après
    /// les critères définis dans les régions de la tuile parente
    ///@return la tuile créée
    void extractChildren(std::deque<CTile*>& AChildren, 
			 const SegmentationMode & ASegmentationMode,
			 const ProjectionMode & AProjectionMode,
			 const FocusAttentionMode & AFocusAttentionMode,
			 CChrono & AChrono);
    
    CDart* makeTileBorder();
    
    /// Chaîne de traitement principale de l'extraction
    /// @param ALast: dernier brin crée lors de la construction du bord
    void extractMapMainLoop( CDart* ALast, 
			     const SegmentationMode & ASegmentationMode,
			     const FocusAttentionMode & AFocusAttentionMode);

    /// Extraction de la carte topologique de la tuile fille en prenant d'après
    /// les critères définis dans les régions de la tuile parente
    /// @param ATileUp: la tuile parente
    /// @param AMatrix: une matrice permettant de connaitre la région d'appartenance 
    /// des pixels de l'image
    void extractMap(CTile* ATileUp, 
		    const SegmentationMode & ASegmentationMode,
		    const FocusAttentionMode & AFocusAttentionMode);

    bool samePixelActuLeft(IM_Pixel & APixel,
			   const SegmentationMode & ASegmentationMode,
			   const FocusAttentionMode & AFocusAttentionMode);
   
    bool samePixelActuUp(IM_Pixel & APixel, 
			 const SegmentationMode & ASegmentationMode, 
			 const FocusAttentionMode & AFocusAttentionMode);
    
    // détermine si le pixel2 doit être intégré à la région du pixel 1
    bool sameRegions(IM_Pixel & APix1, IM_Pixel & APix2, const CDoublet & ADoublet,
		     const SegmentationMode & ASegmentationMode,
		     const FocusAttentionMode & AFocusAttentionMode);

    /// Crée une structure matricielle qui permet de retrouver à quelle région
    /// appartient chaque pixel. Sorte d'image de labels.
    /// @return la matrice
    CMatrix<CRegion*>* createPixelRegionMatrix();

    CArray* createLignelDartMatrix();
  
    //void retrieveUpDownDartsFromRegion(CTile* ATileUp);
    
    CDart* createSquareFace(CDart* Alast, CDart* Aup,
			    CDoublet& ADoublet,
			    CRegion* ARegion);

    /// Assure la contrainte de causalité au niveau des brins de la région infinie
    /// @param AMatrix: matrice de correspondance entre brins et doublets dans la tuile up
    void linkInfiniteUpDown();



    //******************************************************************************	  
    // Construction par copy/burst/merge
    //******************************************************************************    
  

    /// Crée une tuile composée d'une seule région
    void createTopTile();

    /// Construit la matrice de Khalimsky d'après celle de la tuile up
    /// en dilatant si nécessaire
    /// @param: ATileUp: la tuile up
    void copyKhalimsky(CTile* ATileUp);    

    void copyDarts(CTile* ATileUp);

    void copyRegions(CTile* ATileUp);
    
    void traversePixels();
    void traversePixelsSingleRegion();

    /// Copie une tuile et relie les brins et les régions (up/down)
    CTile* createChildByCopy(const ProjectionMode & AProjectionMode);
  
    CTile* createChildrenBySplit();

    /// Division des régions d'une tuile par burst & merge. 
    /// Le critère de split indique si une 
    /// région doit être éclatée (1 région/pixel). Le critère de merge indique
    /// si les nouvelles régions créées doivent être fusionnées.
    void burstAndMerge(const FocusAttentionMode & AFocusAttentionMode,
		       const SegmentationMode & AMode);

    /// @deprecated
    /// Fusion des régions créées par le split d'après le critère de merge
    /// @param toDelete: liste des régions qui ont été éclatées lors du split
    void merge(std::stack<CRegion*>& toDelete);

    /// Supprime les brins en trop à l'intérieur de la tuile (bord exclu)
    void simplifyMap();

    /// Regarde si un brin appartient au contour géométrique de la tuile
    /// @param ADart: le brin
    /// @param AMethod: 0: topologiquement (beta2 dans la region infinie); 
    /// 1: géométriquement (doublet sur le bord)
    /// @return true si le brin est sur le bord
    bool isDartOnBorder(CTopologicalDart* ADart, int AMethod);
    
    /// Relie 2 tuiles (up/down)
    void linkTileUpDown(CTile* ATileUp, CTile* ATileDown);
        
    //******************************************************************************	  
    // Swap des tuiles sur disque dur
    //******************************************************************************    

    /// Retourne un nom de fichier sous la forme header-Ai-Aj-Ak
    void setFilename(uint Ai, uint Aj, uint Ak);
    
    /// Parse et stocke les entiers d'un fichier texte dans un vecteur
    /// @param fileName: le nom du fichier
    /// @param dest: le vecteur qui contiendra les entiers
    /// @return true si le fichier est parcouru en entier sans erreurs
    bool readInt(const std::string& fileName, std::vector<uint>& dest);
    
    /** Enregistre les brins de la tuile de coordonnées (Ai,Aj,Ak)
     *  nom du fichier: topology-Ai-Aj-Ak
     *  champs:
     *  brinId beta1Id beta2Id x y pos regionId brinUpId brinDownId
     *  rq: (x,y,pos) est le plongement du brin
     */
    void saveTopology();
    
    /** Enregistre les régions de la tuile de coordonnées (Ai,Aj,Ak)
     *  nom du fichier: tree-Ai-Aj-Ak
     *  champs:
     *  regionId x y nbPixels colorSum sum regionUpId regionDownId 
     *  colMin colMax fatherId sonId brotherId NextSameCCId representativeId infinite
     *  rq:
     *   - (x,y) est le first pixel de la région
     *   - colorSum: somme des niveaux de gris des pixels
     *   - sum: somme des carrés des niveaux de gris des pixels
     *   - colMin/colMax: niveau de gris min/max de la région
     *   - infinite: 1 si région infinie, 0 sinon
     */
    void saveTree();

    /** Enregistre les propriétés de la tuile de coordonnées (Ai,Aj,Ak)
     *  nom du fichier: tile-Ai-Aj-Ak
     *  champs:
     *  tileId tileUpId tileDownId
     */
    void saveProperties();

    void loadProperties();

    /// Crée les instances des régions de la tuile (Ai,Aj,Ak)
    void loadEmptyTree();

    /// Crée les instances des brins de la tuile (Ai,Aj,Ak)
    void loadEmptyTopology();

    /// Renseigne les champs des brins
    void loadTopology();
    
    /// Renseigne les champs des régions    
    void loadTree();

    /// Enregistre la tuile courante dans ./output/tile-i-j-k
    void write();

    /// Charge une tuile en mémoire depuis un fichier présent dans ./output/tile-i-j-k
    void load(uint Ai, uint Aj, uint Ak);

    //******************************************************************************	  
    // Méthodes sur les brins
    //******************************************************************************    
    
    /**
     *  Crée un tableau de FSizeDartArray CDart.
     *  Cette méthode doit être redéfinie à chaque fois que l'on
     *  surcharge la classe brin.
     */
    virtual CDart* newTabDarts();

    /**
     *  Détruit le tableau de FSizeDartArray CDart.
     *  Cette méthode doit être redéfinie à chaque fois que l'on
     *  surcharge la classe brin.
     */
    virtual void deleteTabDarts(CDart* ATabDart);
    
    /// Récupère le Aj ème brin du Ai ème tableau
    virtual CDart* getDart(uint Ai, uint Aj) const;
    
    /**
     *  Crée une nouvelle instance de CPyramidalDart sans plongement.
     *  @return Un nouveau brin de type CPyramidalDart
      */
    virtual CDart* addMapDart();
 

    // Copie le contenu du brin ADartSrc dans le brin ADartDest
    virtual void copyDart(CDart* ADartDest, CDart* ADartSrc);
    
    /// Crée une nouvelle instance de CPyramidalDart, dont le plongement est
    /// donné par ADoublet.
    /// @param  ADoublet Un sommet
    /// @param  ARegion Un pointeur vers une région
    /// @return Un nouveau brin de type CPyramidalDart
    virtual CTopologicalDart* addMapDart(const CDoublet & ADoublet, CRegion* ARegion);

    /// Accesseur sur le brin du niveau supérieur
    /// @param ADart le brin
    /// @return le brin up
    CPyramidalDart* getDartUp(CDart* ADart) const;
  
    /// Accesseur sur le brin du niveau supérieur
    /// @param ADart le brin
    /// @param le brin up
    void setDartUp(CDart* ADart, CDart* ADartUp);
  
    /// Accesseur sur le brin du niveau inférieur
    /// @param ADart le brin
    /// @return le brin down
    CPyramidalDart* getDartDown(CDart* ADart) const;
  
    /// Accesseur sur le brin du niveau inférieur
    /// @param ADart le brin
    /// @param le brin down
    void setDartDown(CDart* ADart, CDart* ADartDown);
  
    /// Relie 2 brins dans les 2 sens.
    void linkDartUpDown(CDart* ADartUp, CDart* ADartDown);

    /// Retourne le doublet associé au brin.
    /// @param ADart le brin
    /// @return Le doublet.
    CDoublet& getDoublet(CDart* ADart) const;
    CDoublet& doublet(CDart* ADart) const;

    /// Donne un plongement géométrique à un brin
    /// @param ADart le brin
    /// @param ADoublet le plongement.
    void setDoublet(CDart* ADart, const CDoublet& ADoublet);


    //******************************************************************************	  
    //Opérations sur les régions
    //******************************************************************************

    /// Crée une seule région minimale (1seul brin) de la taille de la carte
    /// @param AWidth: la largeur de la carte
    /// @param AHeight: la heuteur de la carte
    void createSingleRegion(uint AWidth, uint AHeight);

    /// Crée une seule région carrée composée de 4 brins de la taille de la carte
    void splitSingleRegion(uint AWidth, uint AHeight);

    /// Crée une nouvelle instance de la classe CPyramidalRegion
    /// @return Un nouvelle région, de type CPyramidalRegion
    virtual CRegion* newRegion();
  	
    /// Supprime toutes les régions de la carte
    void removeAllRegions();

    /// Ajoute une région sur la carte
    /// @return la région insérée  	        
    CPyramidalRegion* addMapRegion();
    
    /// Crée une nouvelle instance de CPyramidalRegion, en utilisant les
    /// propriétés d'une région
    /// @param ARegion: la région
    /// @return Une nouvelle région de type CPyramidalRegion    
    CPyramidalRegion* addMapRegion(CRegion* ARegion);

    /// Ajoute la région infinie à la carte
    /// @return la région infinie  	        
    CInfinitePyramidalRegion* addMapInfiniteRegion();

    /// Accesseur sur la région d'un brin
    /// @param ADart: le brin
    /// @return la région
    CPyramidalRegion* getRegion(CDart* ADart) const;

    /// Accesseur sur la région du niveau supérieur
    /// @param ARegion la région
    /// @return la région up
    CPyramidalRegion* getRegionUp(CRegion* ARegion) const;
  	
    /// Accesseur sur la région du niveau supérieur
    /// @param ARegion la région
    /// @param la région up
    void setRegionUp(CRegion* ARegion, CRegion* ARegionUp);
    
    /// Accesseur sur la région du niveau inférieur
    /// @param ARegion la région
    /// @return la région down
    CPyramidalRegion* getRegionDown(CRegion* ARegion) const;
  	
    /// Accesseur sur la région du niveau inférieur
    /// @param ARegion la région
    /// @param la région down
    void setRegionDown(CRegion* ARegion, CRegion* ARegionDown);
   
    /// Relie 2 régions dans les 2 sens.
    /// @param ARegion la région up
    /// @param ARegion la région down
    void linkRegionUpDown(CRegion* ARegionUp, CRegion* ARegionDown);
  	
    /// Accesseur sur le nombre de régions de la carte
    /// @param ANbRegion: le nombre de régions
    void setNbRegions(uint ANbRegions);

    void setNbDarts(uint ANbDarts);
  	
    /// Supprime une liste de régions
    /// @param AList: la liste des régions
    void deleteRegionList(std::deque<CRegion*>& AList);
    
    /// Ajoute un pixel à la région
    /// @param ARegion: la région
    /// @param x,y: les coordonnées image du pixel à ajouter
    virtual void addPixel( IM_Pixel& APixel, CRegion* ARegion);
    //void addPixel2( uint8* AData, CRegion* ARegion );
    
    /// Retourne les coordonnées de la boite englobante d'une région
    /// @param ARegion: la région
    /// @param tableau de 4 entiers (xmin,ymin,xmax,ymax)
    void getRegionBoundingBox(CDart* ADart, std::vector<uint>& ATab);

  	
    //******************************************************************************	  
    //Opérations sur la carte
    //******************************************************************************   	


    /// @return true si la carte dispose d'un niveau supérieur
    bool existMapUp();
 	  
    /// @return true si la carte dispose d'un niveau inférieur 	  
    bool existMapDown();
    
    /// @return l'espace mémoire occupé par la carte    
    unsigned long int getMemoryForMap() const;
    
    /// @return l'espace mémoire occupé par l'arbre d'inclusion de la carte    
    unsigned long int getMemoryForInclusionTree() const;

    /// @return l'espace mémoire occupé par la tuile d'image chargée
    unsigned long int getMemoryForImageTile() const;
    

    /**
     *  Méthode permettant de vider la carte ainsi que de détruire les
     *  structures annexes. La méthode removeAllDarts est appelée (pour
     *  garder les marques utilisées). Puis l'image, la matrice de
     *  Khalimsky et l'arbre d'inclusion sont détruit.
     */
    virtual void empty();

    //******************************************************************************	  
    //Opérations de split
    //******************************************************************************    


    /// Return true si la région doit être éclatée
    /// @param ARegion: la région à tester
    /// @param ALevel: le niveau de la région, permet de récupérer les seuils appropriés
    bool isRegionToSplit(CRegion* ARegion, const FocusAttentionMode & AFocusAttentionMode);

    /// Insère un sommet sur l'arête d'un brin
    /// @param ADart: le brin
    CDart* insertVertexOnEdge(CDart* ADart);

    /// Insère un sommet sur l'arête d'un brin au niveau d'un doublet
    /// @param ADart: le brin
    /// @param ADoublet: le doublet
    CDart* insertVertexOnEdge(CDart* ADart, const CDoublet& ADoublet);

    /// Insère un sommet sur l'arête d'un brin au niveau d'un doublet
    /// Ne pas appeler directement !
    /// @param ADart: le brin
    /// @param ADoublet1: le doublet
    /// @param ADoublet2: le doublet d'avant
    CDart* insertVertexOnEdge(CDart* ADart, const CDoublet& ADoublet1, const CDoublet& ADoublet2);

    /// Insère une arête pendante au niveau d'un brin
    /// @param ADart: le brin
    CDart* insertEdge(CDart* ADart);    

    /// Découpe l'arête correspondant d'un brin de façon unitaire
    /// en suivant son plongement
    /// @param ADart: le brin
    void splitEdge(CDart* ADart);

    /// Découpe toutes les arêtes de la carte en arêtes unitaires
    void splitAllEdges();

    /// Découpe toutes les arêtes d'une région en arêtes unitaires
    /// @param ADart: le brin désignant la région à découper
    void splitAllEdgesRegion(CRegion* ARegion);
  
    /// Découpe une région en régions unitaires d'un pixel en insérant toutes les arêtes
    /// @param ADart: le brin désignant la région
    /// @param AList enqueue les brins désignant les arêtes ajoutées
    void splitRegion(CDart* ADart, std::deque<CDart*>& AList);
    void splitRegion2(CDart* ADart, std::deque<CDart*>& AList);
  
    /// Crée les nouvelles régions après un splitRegion()
    /// @param AList: la liste des brins correspondant à toutes les arêtes
    /// insérées lors du splitRegion
    /// @param ARegion: la région mère qui a été éclatée lors du splitRegion
    /// @param AOracle: le critère de merge dont doivent hériter les nouvelles régions
    void createNewRegions(CRegion* ARegion,
			  std::deque< CDart* >& ADartList,
			  std::deque< CPyramidalRegion* >& ARegionList);
    
    /// Chaîne une liste de régions pour le calcul de l'arbre d'inclusion
    /// @param AList: une liste de régions
    void createChainRegionList(std::deque<CPyramidalRegion*>& AList);
  
    /// Prépare la liste une liste de régions avant un relabel
    /// @param ARegion la région à partir de laquelle on doit insérer
    /// @param ANewRegionList la liste des régions
    void updateRegionList(CRegion* ARegion,
			  std::deque<CPyramidalRegion*>& ANewRegionList);
 
   
    //******************************************************************************	  
    //Opérations de merge
    //******************************************************************************        


    /// Critère de merge entre deux régions issues du même parent
    /// @param ARegion1: région 1
    /// @param ARegion2: région 2
    /// @param ASegmentationMode: critère de segmentation
    /// @return true si les 2 régions doivent être fusionnées
    bool isRegionToMerge(CRegion* ARegion1, CRegion* ARegion2, 
			 const SegmentationMode & ASegmentationMode=Thresholds);

    /// Parcours tous les brins de la carte pour voir pour chaque brin d s'il 
    /// faut fusionner region(d) avec region(beta2(d)).
    void merge();

    /// Parcours une liste de brins pour voir pour chaque brin d s'il 
    /// faut fusionner region(d) avec region(beta2(d)).
    void merge(std::deque<CDart*>& toMerge,
 	       const SegmentationMode & ASegmentationMode);
      
    /// Suppression d'un sommet lors d'un merge
    void mergeVertexRemoval( CDart* ADart, int AMarkNumber );
    void extractVertexRemoval( CDart* ADart );
  
    /// Suppression d'une arête lors d'un merge
    void mergeEdgeRemoval( CDart* ADart, int AMarkNumber );


   //******************************************************************************	  
    //Opérations de contrôle et de vérification
    //******************************************************************************


    /// Vérifie que tous les brins de la carte ont un plongement géométrique
    bool checkGeometry();
	  
    /// Vérifie que la région de @param ADart est carrée et unitaire
    /// (géométrie + topologie)
    bool checkSquareRegion(CDart* ADart) const;

    /// @return true si les brins d'un niveau sont correctement reliés
    /// au niveau précédent.
    bool checkDartLinks();

    /// @return true si les régions d'un niveau sont correctement reliées
    /// au niveau précédent.
    bool checkRegionLinks();

    /// @return true si tous les brins parcourus dans la carte appartiennent bien 
    /// à cette carte (algo en n2, couteux)
    bool checkDarts();

    /// @return true si le nombre de régions présentes dans l'arbre d'inclusion
    /// est égal au compteur du nombre de régions créées
    bool checkRegions();

    /// @return true si le nombre de régions présentes dans la liste chainée des régions
    /// est égal au compteur du nombre de régions créées
    bool checkNbChainRegions();
  
    /// Vérifie la liste chaînée des régions avant le calcul de l'arbre d'inclusion
    // TODO: à reprendre
    bool isChainRegionListOk();
  
    /// Affiche la liste chaînée des régions
    void printChainRegionList();
  
    /// Affiche l'identifiant des régions contenues dans @param AList
    void printRegionList(std::deque<CPyramidalRegion*>& AList);

    /// Affiche les caractéristiques de toutes les régions d'un niveau
    void printInfosAllRegions();

    /// Affiche les caractéristiques de tous les brins d'un niveau
    void printInfosAllDarts();
	
    /// Affiche quelques caractéristiques d'un niveau
    void printInfosMap();
	
    /// Affiche l'arbre d'inclusion des régions 
    void printInclusionTree();
	
    /// Affiche les propriétés d'une tuile
    void print();

    /// Affiche les brins marquant les coins de la tuile
    void printCorners();

    /// Affiche la map id/brin
    void printMapDarts();

    /// Affiche la map id/region
    void printMapRegions();

    /// Affiche la map id/id du beta2 pour les brins du bord
    void printMapBeta2();

    /// Vérifie que la tuile ATile est correcte
    bool isOk();
    
    /// Vérifie la causalité depuis la tuile up
    /// @return true si la contrainte est respectée
    bool checkHierarchy();

    bool checkLignelDartMatrix(CVolume<CDart*>* AMatrix);

    /// @return la mémoire occupée par le modèle de tuile topo
    unsigned long int memory() const;

    /// @return la mémoire occupée uniquement par la classe tuile
    unsigned long int getMemoryForTile() const;

    /// Affiche la mémoire de la tuile + carte + inclusion tree + khalimsky
    void printInfosMemory();

    /** Affiche les infos "brins" qu'on va sauvegarder
     * @param ADarts: la structure
     * @param ASize: sa taille
     */
    void printTopoStruct(struct SDart* ADarts,   uint ASize);

    /// Affiche les infos "régions" qu'on va sauvegarder
    /// @param ADarts: la structure
    /// @param ASize: sa taille
    void printTreeStruct(struct SRegion* ARegions, uint ASize);

    /// Affiche les caractéristiques de la tuile qu'on va sauvegarder
    /// @param ADarts: la structure
    /// @param ASize: sa taille
    void printPropertiesStruct(struct SProperties* AProperties);

    // on est obligé de redéfinir le unionRegionRoot pour trier
    // les régions par first pixel et pas par id
    int unionRegionRoot(CRegion* ARegion1, CRegion* ARegion2);

    // on est obligé de redéfinir le relabelDarts pour les
    //relations up/down
    void relabelDarts();
    
    void toSVG();

    //******************************************************************************	  
    // Dijkstra (DK)
    //******************************************************************************

    void projection(const ProjectionMode & AProjectionMode);
    void projectionStandard();
    void projectionDijkstra();

    /// Calcule la différence de niveau de gris entre 2 pixels séparés
    // par un doubler
    /// @param  ADoublet : le doublet
    /// @return le gradient
    uint gradient(const CDoublet & ADoublet, const SystemMode & ASystem);
    float gradientInverse(const CDoublet & ADoublet, const SystemMode & ASystem);

    /// calcule l'ensemble des positions potentielles d'un pointel
    /// du niveau k lorsqu'il est projeté dans k+1. 
    /// @param APointel : le pointel du niveau k
    /// @return le vecteur des posisitions potentielles au niveau k+1
    std::vector<CDoublet> reductionWindow(const CDoublet & APointel);

    /// DK: affectation d'un poids à tous les noeuds (doublets) entre ADoublet1 et ADoublet2
    /// @param ADoublet1 : point de départ
    /// @param ADoublet2 : point d'arrivée
    /// @param AHeap : le tas contenant tous les noeuds 
    /// @return last node
    CDkDoublet* findNodes(const CEdge & AEdge, Heap & AHeap);
    
    ///Récupère les voisins d'un doublet étant à une distance d'un lignel
    ///@param ADoublet : le doublet
    ///@return les voisins
    std::vector<CDkDoublet*> findNeighbors(CDkDoublet* ADoublet, const Heap & AHeap);
    
    ///On met à jour la distance entre le noeud initial et ANode2
    ///en regardant s'il faut passer par ANode1
    ///@param ANode1 : le point de passage
    ///@param ANode2 : le noeud à considérer
    void updateDistance(CDkDoublet* ANode1, CDkDoublet* ANode2);
    
    ///Donne le noeud précédent
    ///@param ANode : le noeud
    ///@return le noeud précédent
    CDkDoublet* findPrev(CDkDoublet* ANode);
      
    float weight(CDkDoublet* ANode1, CDkDoublet* ANode2);
    
    uint projectPath(const CEdge & AEdge);

    void removeDanglingEdges();

    ///ajoute une nouvelle arête à la liste des arêtes
    ///@param AVector la liste des arêtes
    void addNewEdge(std::vector<CEdge> & AVector);

    ///Ajoute un doublet sur la derniere arete de la liste
    ///@param AVector : la liste d'arêtes
    ///@param ADoublet : le doublet
    void addDoublet(std::vector<CEdge> & AVector, const CDoublet & ADoublet);

    void assignClassifLabelsToRegions(const FocusAttentionMode & AFocusAttentionMode);
    CPyramidalRegion::Label intToLabel(const uint label);

    void histogram(CImg<float>* histo);

    unsigned int mergeRegions(bool (CRegion::*AMethod)(CRegion*,int)const, int AThreshold);
    unsigned int removeSmallRegions(unsigned int AThreshold);

    //fait un tableau de la taille de l'ensemble des tuiles filles
    //comportant les valeurs des pixels au niveau k+1
    //des pixels appartenant à des régions d'intérêt au niveau k    
    uint8* regionsData();
    
    /// enregistre la bounding box d'une region sous forme d'une image png
    void exportRegionToPng(CPyramidalRegion* ARegion, const std::string & ADirname);

    /// selectionne plus finement quelles sont les régions correspondant 
    /// à des figures de mitoses parmis
    /// les régions dont le label est CPyramidalRegion::Mitosis
    /// et les enregistre en png dans output
    void saveMitosis();

    /// Supprime les boucles de la carte
    void removeLoops();

    void projectionDijkstraConstant();

    bool oracle(const IM_Pixel & APixel1, const IM_Pixel & APixel2, 
		const SegmentationMode & ASegmentationMode);

  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("tile.icc")
#include INCLUDE_INLINE("tile-extract.icc")
#include INCLUDE_INLINE("tile-projection.icc")
#include INCLUDE_INLINE("tile-burst.icc")
//******************************************************************************
#endif // TILE_HH
//******************************************************************************
