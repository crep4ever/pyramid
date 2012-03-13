/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/
 *
 * This file is part of lib-mapkernel
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
#ifndef DOUBLET_HH
#define DOUBLET_HH
//******************************************************************************
#include "inline-macro.hh"
#include <iostream>
#include <cassert>
#include "alt-stdint.hh"
//******************************************************************************
typedef uint16_t TNatural;
typedef uint8_t  TLinel;

//lignels:
enum
{
  XPOS = 0, YNEG, XNEG, YPOS,
};

//******************************************************************************
class CDoublet
{
public:
  /// @name Constructeurs
  //@{

  /** Constructeur par défaut.
   *  Crée une instance de la classe, correspondant à l'origine.
   */
  CDoublet();

  /** Constructeur par recopie
   *
   *  @param ADoublet un triplet a recopier
   */
  CDoublet(const CDoublet & ADoublet);

  /** Opérateur d'affectation.
   *
   *  @param ADoublet un doublet a recopier
   */
  CDoublet& operator = (const CDoublet & ADoublet);

  /** Opérateur d'égalité
   *
   *  @param ADoublet un doublet a comparer
   */
  bool operator == (const CDoublet & ADoublet) const;

  /** Opérateur de différence
   *
   *  @param ADoublet un doublet a comparer
   */
  bool operator != (const CDoublet & ADoublet) const;

  /** Opérateur inférieur
   *
   *  @param ADoublet un doublet a comparer
   */
  bool operator < (const CDoublet & ADoublet) const;

  /** Constructeur avec initialisation des coordonnées uniquement
   *     ((Ax,Ay), x,y)
   *  @param Ax, Ay Les coordonnées entières du doublet
   */
  CDoublet(TNatural Ax, TNatural Ay);

  /** Constructeur avec initialisation
   *
   *  @param Ax, Ay     Les coordonnées entières du doublet
   *  @param ALinel     Le type de lignel

   */
  CDoublet(TNatural Ax, TNatural Ay, const TLinel& ALinel);
  //@}

  /// @name Destructeur
  ~CDoublet();

  /// @name Accesseurs
  //@{
  TNatural getX() const;
  TNatural getY() const;

  TNatural incX(int ANb = 1);
  TNatural incY(int ANb = 1);

  void setX(const TNatural& Ax);
  void setY(const TNatural& Ay);

  void setDoublet(const TNatural& Ax, const TNatural& Ay,
		  const TLinel& ALinel);

  TLinel getLinel() const;
  void   setLinel(const TLinel& ALinel);

  bool samePointel(const CDoublet& ADoublet) const;

  void setPointel (const CDoublet& ADoublet);
  void setPointel (const TNatural& Ax, const TNatural& Ay);

  //@}

  /// @name Méthodes pour récupérer un prochain élément
  //@{

  /// Pour récupérer l'autre pointel incident au même linel
  CDoublet getNextPointel() const;

  /// Idem que getNextPointel() mais ici on modifie le doublet courant.
  void setNextPointel();

  /// Pour récupérer le prochain lignel en tournant autour du pointel dans le
  /// sens anti-trigonométrique
  CDoublet getNextLinel() const;

  /// Idem que getNextLinel() mais ici on modifie le doublet courant.
  /// @see getNextLinel()
  void setNextLinel();

  /// Pour récupérer le prochain lignel en tournant autour du pointel dans le
  /// sens trigonométrique
  CDoublet getPrevLinel() const;

  /// Idem que getPrevLinel() mais ici on modifie le doublet courant.
  /// @see getPrevLinel()
  void setPrevLinel();

  //@}

  /**
   * @param  AStream  Le flot dans lequel afficher le doublet
   * @param  ADoublet Le doublet à afficher
   * @return Le flot utilisé pour l'affichage
   */
  friend std::ostream& operator<<(std::ostream& AStream,
				  const CDoublet& ADoublet);

  // Donne le lignel opposé
  static TLinel getOppositeLinel( TLinel L );
  // Donne le lignel orthogonal dans le sens anti-trigo
  static TLinel getOrthogonalLinel( TLinel L );
  // Donne le lignel orthogonal dans le sens trigo
  static TLinel getOtherOrthogonalLinel( TLinel L );

  static bool isPos( TLinel L );
  static bool isNeg( TLinel L );
  static bool isX( TLinel L );
  static bool isY( TLinel L );

private:
  /// @name Attributs privés
  //@{
  TNatural Fx, Fy; //Pointel
  TLinel   FL; // contient 1 lignel
  //@}
};
//******************************************************************************
inline TLinel CDoublet::getOppositeLinel( TLinel L )
{ return L>=2 ? (L-2) : (L+2); }
inline TLinel CDoublet::getOrthogonalLinel( TLinel L )
{ return L<3 ? (L+1) : (0); }
inline TLinel CDoublet::getOtherOrthogonalLinel( TLinel L )
{ return L>0 ? (L-1) : (3); }
inline bool CDoublet::isPos( TLinel L )
{ return L%3==0; }
inline bool CDoublet::isNeg( TLinel L )
{ return L%3!=0; }
inline bool CDoublet::isX( TLinel L )
{ return L%2==0; }
inline bool CDoublet::isY( TLinel L )
{ return L%2!=0; }
//******************************************************************************
#include INCLUDE_INLINE("doublet.icc")
//******************************************************************************
#endif // DOUBLET_HH
//******************************************************************************
