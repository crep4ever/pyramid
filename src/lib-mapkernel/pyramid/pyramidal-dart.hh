/*
 * Copyright (C) 2008-2012, Romain Goffe <romain.goffe@gmail.com>
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

#ifndef PYRAMIDAL_DART_HH
#define PYRAMIDAL_DART_HH

#include "topological-dart.hh"
#include "kernel-types.hh"

namespace Map2d
{
  class CRegion;
}

namespace pyramid
{
  /**
   * \file pyramidal-dart.hh
   * \class CPyramidalDart
   * \brief CPyramidalDart is a dart within a top-down pyramid
   *
   * A pyramidal dart contains additionnal information such as up/down relationships.
   * The dart "up" is the parent dart in the previous level.
   * The dart "down" is the first child (also called 'representative') in the next level.
   * For example, if a dart d from level k is split in two darts (d1,d2) at level k+1 such as
   * d2=beta1(d1), the dart down of d is d1.
   */

  class CPyramidalDart: public Map2d::CTopologicalDart
  {

  private:
    TDartId m_id;           ///< dart identifier
    CPyramidalDart* m_up;   ///< dart up
    CPyramidalDart* m_down; ///< dart down
    CPyramidalDart* m_phi;  ///< opposite dart on adjacent tile
    bool m_fictive;         ///< true if the dart is incident to a fictive border

  public :
    /// Default constructor
    CPyramidalDart();

    /// Copy constructor
    /// @param dart : the dart to be copied
    CPyramidalDart(const CPyramidalDart& dart);

    /** Constructor with marks initialisation
     *  @param  doublet the first vertex of the dart's embedding
     *  @param  region  the region associated to the dart
     *  @param  marks   the marks
     *  @return an instance of pyramidal dart
     */
    CPyramidalDart(const CDoublet & doublet, Map2d::CRegion* region,
		   const std::bitset<NB_MARKS> & marks);

    /// Destructor
    virtual ~CPyramidalDart();

    /// Getter on the dart identifier
    /// @return a unique positive integer identifier
    TDartId id() const;

    /// Setter on the dart identifier
    /// @param a unique positive integer identifier
    void setId(TDartId id);

    /// Getter on the parent dart (dart up) in previous level
    /// @return the dart up
    CPyramidalDart* up() const;

    /// Setter on the parent dart (dart up) in previous level
    /// @param dart: the dart up
    void setUp(CPyramidalDart* dart);

    /// Getter on the children representative dart (dart down) in next level
    /// @return the dart down
    CPyramidalDart* down() const;

    /// Setter on the children representative dart (dart down) in next level
    /// @param the dart down
    void setDown(CPyramidalDart* dart);

    /// Getter on the opposite equivalent dart in adjacent tile
    /// Given a dart d incident to a border of a tile t and t' the adjacent tile of t along this border :
    /// - beta2(d) belongs to the infinite region of t
    /// - phi(d) belongs to t' and beta2(phi(d)) belongs to the infinite region of t'
    /// @return phi(d)
    CPyramidalDart* phi() const;

    /// Setter on the opposite equivalent dart in adjacent tile
    /// Given a dart d incident to a border of a tile t and t' the adjacent tile of t along this border :
    /// - beta2(d) belongs to the infinite region of t
    /// - phi(d) belongs to t' and beta2(phi(d)) belongs to the infinite region of t'
    /// @param dart: phi(d)
    void setPhi(CPyramidalDart* dart);

    /// Getter on the fictive property of a dart
    /// A dart is fictive if and only if it is incident to a fictive border
    /// Fictive borders may only be present on tiles' borders
    /// @return true if the dart is fictive, false otherwise
    bool isFictive() const;

    /// Setter on the fictive property of a dart
    /// A dart is fictive if and only if it is incident to a fictive border
    /// Fictive borders may only be present on tiles' borders
    /// @param value: true if the dart is fictive, by default, this property is false
    void setFictive(bool value);

    /// Getter on the image of dart by operator beta0
    /// beta0 is the inverse operator of beta1 : beta0(beta1(d)) = beta1(beta0(d)) = d
    /// @return beta0(d)
    CPyramidalDart* getBeta0() const;

    /// Getter on the image of dart by operator beta1
    /// beta1 access the following dart of the same face, turning clockwise
    /// @return beta1(d)
    CPyramidalDart* getBeta1() const;

    /// Getter on the image of dart by operator beta2
    /// beta2 access the opposite dart of the same edge
    /// @return beta2(d)
    CPyramidalDart* getBeta2() const;

    /// Indicates if there is a dart up
    /// Note that all darts in the top level of the pyramid do not have parents
    /// @return true if there exists a dart up, false otherwise
    bool existUp();

    /// Indicates if there is a dart down
    /// Note that all darts in the bottom level of the pyramid do not have children
    /// @return true if there exists a dart down, false otherwise
    bool existDown();

    /// Display the dart's properties
    /// @param os: the output stream
    void print( std::ostream& os = std::cout ) const;

    friend std::ostream& operator<<(std::ostream&, const CPyramidalDart &);
  };

} // namespace pyramid
#include INCLUDE_INLINE("pyramidal-dart.icc")
#endif // PYRAMIDAL_DART_HH
