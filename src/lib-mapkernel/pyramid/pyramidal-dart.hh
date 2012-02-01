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
#ifndef PYRAMIDAL_DART_HH
#define PYRAMIDAL_DART_HH
//******************************************************************************
#include "topological-dart.hh"
#include "kernel-types.hh"
//******************************************************************************

namespace Map2d{

  /**
   * \file pyramidal-dart.hh
   * \class CPyramidalDart "pyramidal-dart.hh"
   * \author Romain Goffe
   * \brief CPyramidalDart is a dart within a pyramid with
   *  up/down relationships
   */

  class CRegion;

  class CPyramidalDart: public CTopologicalDart
  {

  protected :
    TDartId FId;
    CPyramidalDart* FDartUp;
    CPyramidalDart* FDartDown;
    CPyramidalDart* FBeta2b;
    bool FFictive;

  public :
    /// Constructeur par défaut.
    CPyramidalDart();

    /// Constructeur par recopie
    CPyramidalDart(const CPyramidalDart& ADart);

    /// Constructeur à partir d'un CTopologicalDart.
    CPyramidalDart(const CTopologicalDart& ADart);

    /** Constructeur avec initialisation des marques
     *
     *  @param AMarks Les marques telles qu'elles doivent être positionnées
     *  @return Une instance de la classe
     */
    CPyramidalDart(const std::bitset<NB_MARKS> & AMarks);

    /** Constructeur avec initialisation des marques
     *
     *  @param  ADoublet Un sommet
     *  @param  ARegion  Un pointeur vers une région
     *  @param  AMarks   Les marques telles qu'elles doivent être positionnées
     *  @return Une instance de la classe
     */
    CPyramidalDart(const CDoublet & ADoublet, CRegion* ARegion,
		   const std::bitset<NB_MARKS> & AMarks);

    /// Destructeur
    virtual ~CPyramidalDart();

    /// Accesseurs
    TDartId getId() const;
    void setId(TDartId AId);

    CPyramidalDart* getDartUp() const;
    CPyramidalDart* up() const;
    void setDartUp(CPyramidalDart* ADart);

    CPyramidalDart* getDartDown() const;
    CPyramidalDart* down() const;
    void setDartDown(CPyramidalDart* ADart);

    CPyramidalDart* getBeta2b() const;
    void setBeta2b(CPyramidalDart* ADart);

    bool isFictive() const;
    void setFictive(bool);

    CPyramidalDart* getBeta0() const;
    CPyramidalDart* getBeta1() const;
    CPyramidalDart* getBeta2() const;

    bool existDartUp();
    bool existDartDown();

    //CDoublet& getDoublet();
    //void setDoublet(const CDoublet& ADoublet);

    /// Affiche les propriétés du brin
    void print( std::ostream& AOs = std::cout ) const;
    friend std::ostream& operator<<(std::ostream&, const CPyramidalDart &);
  };

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("pyramidal-dart.icc")
//******************************************************************************
#endif // PYRAMIDAL_DART_HH
//******************************************************************************
