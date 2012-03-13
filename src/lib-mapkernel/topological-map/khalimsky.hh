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

//*******************************************************************************
#ifndef KHALIMSKY_HH
#define KHALIMSKY_HH
//*******************************************************************************
#include <bitset>
#include <stdint.h>
#include "doublet.hh"

typedef uint8_t TKhalimskyElt;
//*******************************************************************************
/** Classe permettant de manipuler les matrices de Khalimsky (pointels et lignels
 *  associés à une image discrète 2d).
 */
//*******************************************************************************
class CKhalimsky
{
public:
  /** @name Constructeur
   *
   *  @param ASizeX, ASizeY, les dimensions de la matrice
   *  @return Une nouvelle instance NON INITIALISÉE de la classe
   */
  //@{
  CKhalimsky(TNatural ASizeX,
	     TNatural ASizeY);
  //@}

  /// @name Destructeur
  //@{
  ~CKhalimsky();
  //@}

  /// Empty: vide la matrice.
  void empty();

  /// @name Accesseurs
  //@{
  TNatural getSizeX() const;
  TNatural getSizeY() const;

  /// Getter on the matrix data
  /// @return the matrix data
  TKhalimskyElt* getMatrix() const;

  /// Setter on the matrix data
  /// @param AMatrix the matrix data
  void setMatrix(TKhalimskyElt* AMatrix);

  bool isPCell(const CDoublet& ADoublet) const;
  bool isLCell(const CDoublet& Adoublet) const;

  // bool isFictivePCell(const CDoublet& ADoublet) const;
  //bool isFictiveLCell(const CDoublet& ADoublet) const;

  void setPCell(const CDoublet& ADoublet, bool AOn = true);
  void setLCell(const CDoublet& ADoublet, bool AOn = true);

  //void setFictivePCell(const CDoublet& ADoublet, bool AOn = true);
  // void setFictiveLCell(const CDoublet& ADoublet, bool AOn = true);


  //@}

  /// getSize : retourne la taille de la matrice de Khalimsky en octets.
  long int getNumberOfByte() const;

  /** Affichage de la matrice de Khalimsky.
   *  @param withKhalymski un entier indiquant comment afficher la matrice de
   *         Khalimsky. 1 : les pointels, 2 : les lignels,
   *         4 : les surfels (et toute les combinaisons possibles.)
   *  @param coef   le coeficient d'homothetie des surfels.
   *  @param transp le coeficient de transparence des surfels.
   */
  void draw(int withKhalymski, float coef, float transp) const;

  /// @name Méthode pour normaliser l'écriture d'un doublet.
  //@{
  /** Ces fonctions permettent, étant donné un doublet quelconque, de le
   *  "normaliser" c'est à dire de le ramener toujours à la même position
   *  selon la manière dont on considère le doublet (pointel ou lignel).
   */
  CDoublet normaliseDoubletPointel(const CDoublet& ADoublet) const;
  CDoublet normaliseDoubletLinel  (const CDoublet& ADoublet) const;

  //@}

  /// @name Méthode pour calculer le degré d'un pointel
  //@{
  int degreePCell( const CDoublet & APointel ) const;
  //@}

protected:
  /// @name Méthodes auxiliaires
  //@{
  long int      getIndex(const CDoublet& ADoublet) const;
  TKhalimskyElt getKhalimskyElt(const CDoublet& ADoublet) const;
  void          setBits(long int AIndex, TKhalimskyElt AMask, bool AOn);
  //@}

private:
  /// @name Attributs privés
  //@{
  TNatural FSizeX , FSizeY;
  long int FSize;
  TNatural FDeltaX, FDeltaY;
  TKhalimskyElt* FMatrix;
  //@}
};
//******************************************************************************
#include INCLUDE_INLINE("khalimsky.icc")
//******************************************************************************
#endif // KHALIMSKY
//******************************************************************************
