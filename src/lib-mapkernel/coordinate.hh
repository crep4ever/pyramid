#ifndef COORDINATE_HH
#define COORDINATE_HH
//*****************************************************************************
#include "inline-macro.hh"
#include "doublet.hh"
#include <ostream>

/**
 * La classe CCoordinate représente une coordonnée 2D entière.
 * @author Alexandre Dupas
 */
class CCoordinate
{
private:
  int FCoord[2];

public:
  // @name Constructeurs et destructeur
  // @{

  /**
   * Constructeur par défaut
   * Construction d'une nouvelle instance de la classe, initialisée
   * au sommet origine (0,0).
   *
   * @return Une nouvelle instance de la classe
   */
  CCoordinate();

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (Ax,Ay).
   *
   * @param Ax La première  coordonnée du sommet
   * @param Ay La deuxième  coordonnée du sommet
   */
  CCoordinate(int Ax, int Ay);

  /**
   * Constructeur avec initialisation
   * Construction d'une nouvelle instance de la classe, initialisée au
   * sommet de coordonnées (ATab[0],ATab[1]).
   *
   * @param ATab Un tableau des 2 coordonnées du sommet
   */
  CCoordinate(int ATab[2]);

  /**
   * Constructeur par copie
   * Construction d'une nouvelle instance de la classe, en copiant le sommet
   * passé en paramètre.
   *
   * @param AVertex le sommet à copier
   */
  CCoordinate(const CCoordinate& AVertex);

  /**
   * Constructeur prenant en paramètre un doublet.
   * Construction d'une nouvelle instance de la classe, en copiant uniquement
   * les coordonnées du pointel du doublet passé en paramètre.
   *
   * @param ADoublet le doublet à "copier"
   */
  CCoordinate(const CDoublet& ADoublet);

  /**
   * Destructeur
   */
  ~CCoordinate();

  // @}
  // @name Accesseurs
  // @{

  /**
   * Accès en lecture à la première composante du sommet.
   */
  int getX() const;

  /**
   * Accès en lecture à la deuxième composante du sommet.
   */
  int getY() const;

    /**
   * Accès en lecture à une composante du sommet.
   *
   * @param ADim Une dimension (0 ou 1)
   */
  int getCoord(int ADim) const;

  /**
   * Positionne la première composante du sommet à la valeur ANewX.
   *
   * @param ANewX Une valeur quelconque
   */
  void setX(int ANewX);

  /**
   * Positionne la deuxième composante du sommet à la valeur ANewY.
   *
   * @param ANewY Une valeur quelconque
   */
  void setY(int ANewY);

  /** Incrémente la première composante du sommet.
   *
   * @param AIncX valeur d'incrément.
   */
  void incX(int AIncX);

  /** Incrémente la deuxième composante du sommet.
   *
   * @param AIncY valeur d'incrément.
   */
  void incY(int AIncY);


  /**
   * Change la valeur de la composante ADim du sommet.
   *
   * @param ADim Une dimension (0 ou 1)
   * @param ANewCoord Une valeur quelconque
   */
  void setCoord(int ADim, int ANewCoord);

  /**
   * Affecte les deux composantes du sommet.
   *
   * @param ANewX Une valeur quelconque
   * @param ANewY Une valeur quelconque
   */
  void setXY(int ANewX, int ANewY);

  // @}
  // @name Divers opérateurs
  // @{

  CCoordinate& operator=(const CCoordinate& AVertex);

  bool operator==(const CCoordinate& AVertex) const;
  bool operator!=(const CCoordinate& AVertex) const;

  friend bool operator < (const CCoordinate& u,const CCoordinate& v);
  friend std::ostream& operator << (std::ostream& AOs,
				    const CCoordinate& ACoord);

};

// @}
// @name Constantes
// @{

/**
 * Le sommet de cordonnées (0,0).
 */
static const CCoordinate COORDINATE_ORIGIN(0,0);

/**
 * Le vecteur de composantes (1,0).
 */
static const CCoordinate COORDINATE_OX(1,0);

/**
 * Le vecteur de composantes (0,1).
 */
static const CCoordinate COORDINATE_OY(0,1);


/** BASE
 * Les trois vecteurs de base OX et OY placés dans un tableau.
 */
static const CCoordinate COORDINATE_BASE[2] = { COORDINATE_OX,
						COORDINATE_OY };

// @}

//******************************************************************************
#include INCLUDE_INLINE("coordinate.icc")
//******************************************************************************
#endif // COORDINATE_HH
//******************************************************************************
