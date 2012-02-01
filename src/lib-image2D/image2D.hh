/*
 * libImage2D : Librairie de manipulation d'une image 2D.
 * Copyright (C) 2008, Guillaume Damiand, guillaume.damiand@liris.cnrs.fr
 *               http://liris.cnrs.fr
 *
 * This file is part of libImage2D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//******************************************************************************
#ifndef IMAGE2D_HH
#define IMAGE2D_HH

#include <stdint.h>

#include <vector>
#include <set>
#include <Magick++.h>

#include "unionFind.hh"
#include "pixel.hh"

/// Type pour stocker la valeur d'un pixel. Utiliser forcément un non-signé
/// sous peine de problème.
typedef uint16_t TLabel;

/// Valeur utilisée pour vider l'image.
const TLabel NULL_LABEL = ( TLabel )0;

//******************************************************************************
/**
 * CImage2D une classe permettant de charger, manipuler et visualiser une image
 * 2D. La valeur de chaque pixel est codée par un TLabel (voir ::TLabel).
 * Les pixels sont stockés dans un tableau 1D représentant la matrice
 * "applatie". Une matrice de booléen permet de marquer les pixels.
 * Le format par défaut est de type raw16 (format binaire, un pixel sur 2
 * octets), mais il est possible d'importer/exporter à n'importe quel format
 * d'image acceptés par libimagemagick.
 * Par contre ATTENTION : pour l'affichage, l'import ou l'export d'images,
 * il faut limiter le nombre de niveaux de gris entre 0.0 et 1.0.
 * Pour cela, les 2 variables FDisplayMin et FDisplayMax permettent de fixer
 * une plage d'intérêt, les valeurs < ou > sont seuillées (mais uniquement
 * à l'affichage) Par défaut ces valeurs valent 0 et valeur max du type.
 * La convertion label<->niveau de gris se fait par application linéaire entre
 * [FDisplayMin,FDisplayMax] et [0,1]
 */
class CImage2D
{
public:
  /// Constructeur d'une image vide et non valide.
  CImage2D();

  /// Constructeur d'une image de taille (Ax,Ay) initialisée à NULL_LABEL.
  CImage2D( unsigned int Ax, unsigned int Ay );

  /// Constructeur par recopie.
  CImage2D( const CImage2D & );

  /// Constructeur en lisant l'image AFilename avec imagemagick
  /// Le fichier correspondant doit donc être d'un format connu.
  CImage2D( const char * AFilename );

  /**
   * Constructeur en lisant l'image AFilename de type raw16.
   * Ce format ne contient pas la taille de l'image, elle est
   * donnée par (AXLarg,AYLarg)
   */
  CImage2D( const char * AFilename, unsigned int AXLarg,
	    unsigned int AYLarg );

  /// Destructeur : détruit les tableaux.
  virtual ~CImage2D();

  /**
   * Opérateur de recopie d'une image. *this reçoit une copie de AImage2D.
   *  Si AImage2D n'est pas ok, ne fait rien. Ne recopie pas la matrice
   *  union-find.
   * @return *this après la copie.
   */
  CImage2D& operator= ( const CImage2D& AImage2D );

  /// @return vrai ssi l'image est valide (allouée et taille>0).
  bool isOk() const;

  unsigned int getXSize() const; ///< @return la taille de l'image en X
  unsigned int getYSize() const; ///< @return la taille de l'image en Y

  /// @return la taille mémoire occupée en octets
  unsigned int getNumberOfBytes() const;

  /// @return vrai ssi le pixel Ax,Ay est dans l'image.
  bool isPixelInImage(int Ax, int Ay) const;
  bool isPixelInImage(const CPixel& APixel) const;

  /// Pour tester si le voisin d'un pixel donné existe dans l'image courante.
  /// Cette fonction est la seule fonctionnant sur le 18-voisinage.
  bool existNeighboorPixel(const CPixel&, int ADir) const;

  /// Teste si le pixel donné est un pixel de bord ou non
  /// dans la direction ADir.
  bool isBoundaryPixelInDir(const CPixel& APixel, int ADir) const;

  /// Teste si le pixel donné est un pixel de bord ou non dans une des
  /// directions possibles.
  bool isBoundaryPixel(const CPixel&) const;

  /// Retourne si le pixel donné est au bord de la région donnée ou non.
  bool isBoundaryPixel(const CPixel&, TLabel) const;

  /// @return la valeur du pixel (Ax,Ay).
  TLabel getPixel( unsigned int Ax, unsigned int Ay ) const;
  TLabel getPixel( const CPixel& APixel ) const;

  /// Affecte le pixel (Ax,Ay) avec la valeur Ac
  void setPixel( unsigned int Ax, unsigned int Ay,
		 const TLabel & AVal );
  void setPixel( const CPixel& APixel, const TLabel & AVal );

  /// Marque le pixel (Ax,Ay) : le booléen est mit à ABool.
  void setMarkTo( unsigned int Ax, unsigned int Ay, bool ABool );
  void setMarkTo( const CPixel& APixel, bool ABool );

  /// Marque le pixel (Ax,Ay) : le booléen est mit à true.
  void setMark( unsigned int Ax, unsigned int Ay );
  void setMark( const CPixel& APixel );

  /// Démarque le pixel (Ax,Ay) : le booléen est mit à faux.
  void unsetMark( unsigned int Ax, unsigned int Ay );
  void unsetMark( const CPixel& APixel );

  /// @return vrai ssi le pixel (Ax,Ay) est marqué.
  bool isMarked( unsigned int Ax, unsigned int Ay ) const;
  bool isMarked( const CPixel& APixel ) const;

  /// Démarque tout les pixels de l'image.
  void unmarkAll();

  /// @return vrai ssi le pixel (Ax,Ay) à le même label
  /// que son voisin gauche (en tenant compte d'uné région infinie
  /// tout autour de l'image)
  bool samePixelActuLeft(unsigned int Ax,unsigned int Ay) const;

  /// @return vrai ssi le pixel (Ax,Ay) à le même label
  /// que son voisin du dessus (en tenant compte d'uné région infinie
  /// tout autour de l'image)
  bool samePixelActuUp(unsigned int Ax,unsigned int Ay) const;

  /// Remplit toute l'image avec la valeur AIdRegion
  void fillImage( TLabel AIdRegion );

  /// Remplit toute l'image avec la valeur NULL_LABEL
  void emptyImage();

  /// Affecte le label de tout les pixels ayant un label se
  /// trouvant dans ASet avec la valeur AIdRegion.
  void partialFillImage( const std::set<TLabel> & ASet,
			 TLabel AIdRegion );

  /// Affecte le labelde tout les pixels ayant un label se
  /// trouvant dans ASet avec la valeur NULL_LABEL.
  void partialEmptyImage( const std::set<TLabel> & ASet );

  /// @return La valeur moyenne des An pixels en partant de (Ax,Ay)
  /// dans la direction x.
  TLabel moyenneX( unsigned int Ax, unsigned int Ay,
		   unsigned int An );

  /// @return La valeur moyenne des An pixels en partant de (Ax,Ay)
  /// dans la direction y.
  TLabel moyenneY( unsigned int Ax, unsigned int Ay,
		   unsigned int An );

  /// @return La valeur moyenne des An*An pixels en partant de (Ax,Ay)
  /// dans les 2 directions x et y.
  TLabel moyenneXY( unsigned int Ax, unsigned int Ay,
		    unsigned int An );

  /// Divise la taille de l'image par ACoef en X. Si AMoy est true,
  /// le pixel de la nouvelle image est une moyenne des ACoef
  /// pixels de l'ancienne.
  void divideImageX( int ACoef, bool AMoy = true );

  /// Divise la taille de l'image par ACoef en Y. Si AMoy est true,
  /// le pixel de la nouvelle image est une moyenne des ACcoef
  /// pixels de l'ancienne.
  void divideImageY( int ACoef, bool AMoy = true );

  /// Divise la taille de l'image par coef en X et Y. Si AMoy est true,
  /// le pixel de la nouvelle image est une moyenne des ACoef²
  /// pixels de l'ancienne.
  void divideImageXY( int ACoef, bool AMoy = true );

  /// Charger l'image AFilename au format raw (la taille de l'image
  /// est donnée par AXLarg, AYLarg)
  /// @return vrai ssi le chargement c'est bien passé, faux sinon
  bool loadRaw( const char * AFilename, int AXLarg , int AYLarg );

  /// Ecrire l'image courante dans le fichier AFilename au format raw
  void writeRaw( const char * AFilename ) const;

  /// Alloue la matrice union-find. Si la matrice est déjà allouée, ne
  /// fait rien. La matrice est stockée comme l'image sous forme
  /// applatie (un tableau 1D).
  void allocateUFMatrix();

  /// Calcule la matrice union-find de l'image.
  /// Chaque case contient un pointeur vers une autre case, this
  /// pour les racines des arbres. Deux pixels sont dans le même arbre
  /// ssi ils sont dans la même composante 4-connexe de pixels ayant
  /// même labels (i.e. même région).
  /// Si le booléen FValidUF est vrai ne fait rien car normalement la
  /// matrice UF est valide, cad allouée et correspond à l'image.
  /// @param AForceUpdate pour forcer la mise à jour, même si le booléen
  /// FValidUF est vrai.
  void initialiseUFMatrix( bool AForceUpdate = false );

  /// Libère la matrice union-find et met le pointeur FUFTree à NULL.
  void freeUFMatrix();

  /// @return l'arbre UF contenue dans la case Ax, Ay (sans faire de
  /// find). Si la matrice union-find n'a pas été initialisée,
  /// retourne NULL.
  CUFTree* getUFTree( unsigned int Ax, unsigned int Ay ) const;

  /// @return l'arbre UF contenue dans la case Ax, Ay (en faisant un
  /// find). Si la matrice union-find n'a pas été initialisée,
  /// retourne NULL.
  CUFTree* findUFTree( unsigned int Ax, unsigned int Ay );

  /// @return vrai ssi l'arbre UF contenue dans la case Ax, Ay
  /// est racice d'un arbre
  bool isRootUFTree( unsigned int Ax, unsigned int Ay );

  /** Fusionne l'arbre UF contenant le pixel (Ax,Ay) avec celui contenant
   * le pixel (Ax2, Ay2). Ne fais rien si la matrice union-find n'a pas
   * été initialisée.
   */
  void mergeUFTree( unsigned int Ax, unsigned int Ay,
		    unsigned int Ax2, unsigned int Ay2 );

  /// @return la valeur du pixel associé à l'arbre AUFTree.
  TLabel getPixel( CUFTree* AUFTree ) const;

  /// @return Le nombre de régions de l'image, c'est à dire le
  /// nombre de composantes 4-connexe avec même label.
  unsigned int getNbRegions();

  /// Pour chaque région, affiche son label et sa taille (nb de pixels).
  void displayRegions();

  /// Pour chaque label de l'image, affiche les régions ayant ce label
  /// et leurs tailles. Si chaque ensemble contient une seule région,
  /// alors l'image est bien formée.
  void displayLabels();

  /// Etiquette les régions : les pixels de chaque région seront
  /// étiquetés avec une même valeur, et deux pixels de régions
  /// différentes auront des étiquettes différentes.
  /// @return le nombre de régions.
  unsigned int labelRegions();

  /// Etiquette les régions ayant un label se trouvant dans ASet.
  /// Les nouveaux labels sont pris à partir du FMaxLevel, donc il se
  /// peut que l'étiquetage soit faux s'il n'y a pas assez de label
  /// disponibles.
  /// @return le nombre de régions re-étiquetés.
  unsigned int partialLabelRegions( const std::set<TLabel> & ASet );

  /// Toutes les fonctions suivantes sont utiles pour afficher l'image,
  /// ou importer/exporter dans un format image classique. Dans ces
  /// cas, nous avons besoin d'associer une couleur aux niveaux de gris
  /// et il y a plusieurs manière de faire.
  ///@{
  /// Accesseurs pour la couleur (classe Magick++) des pixels.
  Magick::ColorGray getPixelColor( unsigned int Ax,
				   unsigned int Ay ) const;
  void setPixelColor( unsigned int Ax, unsigned int Ay,
		      const Magick::ColorGray & );

  /// Accesseurs pour les valeurs min et max des labels présent dans l'image
  TLabel getMinLabel() const;
  TLabel getMaxLabel() const;

  /// Accesseurs pour les valeurs min et max des labels à afficher
  /// utilisées uniquement pour l'affichage ou import/export
  TLabel getDisplayMin() const;
  TLabel getDisplayMax() const;

  void setDisplayMin( TLabel );
  void setDisplayMax( TLabel );
  void setDisplayMinMax( TLabel, TLabel );

  /// Pour calculer automatiquement la plage de labels à afficher
  /// en fonction de l'image.
  void maximiseContrast();

  /// @return l'image actuelle sous forme d'image Magick++
  Magick::Image getImage() const;

  /// Affecte Image (au format Magick++) comme image courante
  void setImage( Magick::Image & );

  /// Affichage de l'image avec un zoom fixé.
  /// Si AZoom>1, l'image est multiplié par AZoom,
  /// Si AZoom<-1, l'image est divisé par |AZoom|.
  void displayImage( int AZoom = 1 ) const;

  /// Charger l'image AFilename au format "image" (png, tif, gif, jpg...)
  /// @return vrai ssi le chargement c'est bien passé, faux sinon
  bool importWithMagick( const char * AFilename );

  /// Ecrire l'image courante dans le fichier AFilename. Le type
  /// d'image dépend de l'extension du fichier passé en paramètre.
  void exportWithMagick( const char * AFilename ) const;

private:

  /**
   * Fonction calculant l'indice correspondant au pixel donné
   * @param Ax position du pixel en x
   * @param Ay position du pixel en y
   * @return l'indice du pixel (Ax,Ay) dans le tableau 1D.
   */
  unsigned int indiceDansTab( unsigned int Ax, unsigned int Ay ) const;
  unsigned int indiceDansTab( const CPixel& APixel ) const;

  /**
   * Fonction allouant les 2 tableaux représentant les matrices (pixels et
   * booléens). Désalloue préalablement les matrices existantes
   * si besoin.
   *  Ax taille de la nouvelle image en x
   *  Ay taille de la nouvelle image en y
   */
  bool allocateData( unsigned int Ax, unsigned int Ay );

  /// Affecte la largeur de l'image à Ax
  void setXSize( unsigned int Ax );

  /// Affecte la hauteur de l'image à Ay
  void setYSize( unsigned int Ay );

  unsigned int FXSize; ///< Taille de l'image en x
  unsigned int FYSize; ///< Taille de l'image en y

  bool FIsOk; ///< Booléen : vrai si l'image est "valide", faux sinon

  TLabel * FImage; ///< Le tableau représentant la matrice de pixels.
  bool      * FMark; ///< Le tableau de booléens pour marquer les pixels.
  CUFTree   * FUFTree; ///< Le tableau contenant la matrice union-find.
  ///< Par défaut à NULL, alloué par computeUFMatrix.

  /** Booléen à vrai ssi la matrice UFFind est initialisée et
   * correspond à l'image. Ce booléen est mit à faux lorsqu'on modifie un
   * pixel de l'image ou si l'on fait des fusions dans les arbres UF.
   */
  bool FValidUF;

  TLabel FMinLevel; ///< Label minimum présent dans l'image.
  TLabel FMaxLevel; ///< Label maximum de l'image (pour autocontrast).

  /// label minimum et maximum que l'on souhaite utiliser pour les
  /// fonctions utilisant imageMagick. Les labels < et > sont seuillés.
  /// Ensuite on fait une correspondance linéaire entre
  /// [FDisplayMin,FDisplayMax] et [0.1]
  ///@{
  TLabel FDisplayMin;
  TLabel FDisplayMax;
  ///@}
};
//*****************************************************************************
#endif // IMAGE2D_HH
//******************************************************************************
