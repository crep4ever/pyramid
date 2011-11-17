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
#ifndef TOPOLOGICAL_DART_HH
#define TOPOLOGICAL_DART_HH
//******************************************************************************
#include "dart.hh"
#include "doublet.hh"
#include <vector>
//******************************************************************************
namespace Map2d
{
  class CRegion;

  class CTopologicalDart: public CDart
  {
  public:
    // @name Constructeurs et destructeur
    //@{
    
    /** Constructeur par défaut
     *
     *  @return Une instance de la classe
     */
    CTopologicalDart();

    /// Initialise le brin comme s'il venait d'être construit.
    void init(const CDoublet & ADoublet, CRegion* ARegion);

    /// Copy les informations du brin ADart
    void copy( CTopologicalDart* ADart );
    
    /// Destructeur
    virtual ~CTopologicalDart();

    //@}

    //@{
    // @name Accesseurs

    /// Récupération du doublet.
    CDoublet& doublet();
    void setDoublet(const CDoublet & ADoublet);

    /// Récupération de la région d'appartenance du brin.
    CRegion* getRegion() const;

    /// Modification de la région d'appartenance du brin.
    void setRegion(CRegion*);
    //@}

    /// Affichage d'un brin.
    void print( std::ostream& AOs ) const;
    friend std::ostream& operator<<(std::ostream&, const CTopologicalDart &);

  protected:
    // @name Attributs
    //@{

    /// Le doublet "géométrique" associé au brin.
    CDoublet FDoublet;

    /// La région d'appartenance du brin.
    CRegion* FRegion;
    
    //@}
  };

} // namespace GMap2d
//******************************************************************************
#include INCLUDE_INLINE("topological-dart.icc")
//******************************************************************************
#endif // TOPOLOGICAL_DART_HH
//******************************************************************************
