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

//*****************************************************************************
#ifndef TRAVERSAL_REGION_PIXELS_HH
#define TRAVERSAL_REGION_PIXELS_HH
//*****************************************************************************
#include "point2d.hh"
#include <stack>
//*****************************************************************************
namespace pyramid
{
  /**
   * Itérateur des pixels d'une région.
   */
  class CTile;
  class CPyramidalRegion;
  class CTraversalRegionPixels
  {
  public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ATile Référence aux données topologiques de la carte.
     * @param ARegion Référence à la région que l'on veut parcourir.
     */
    CTraversalRegionPixels(CTile* ATile, CPyramidalRegion* ARegion);

    /**
     * Destructeur.
     */
    virtual ~CTraversalRegionPixels();

    //@}

    /**
     * Réinitialisation du parcours.
     */
    void reinit();

    /**
     * Itérateur préfixe. Passe au pixel suivant.
     */
    void operator++();

    /**
     * Itérateur postfixe.
     * @return Le pixel suivant.
     */
    const CPoint2D & operator++(int);

    /**
     * Drapeau d'arrêt du parcours.
     * @return <code>true</code> si il reste des pixels qui n'ont pas été
     * parcourus, <code>false</code> sinon.
     */
    bool cont() const;

    /**
     * Déréférencement de l'iterateur en pixel.
     * @return Le pixel actuel.
     */
    const CPoint2D & operator*();

  protected:
    /// Pointeur sur les données topologique.
    CTile *      FTile;
    /// Pointeur sur la région parcourue.
    CPyramidalRegion *              FRegion;
    /// Pile des pixels que l'on doit traiter et parcourir.
    std::stack<CPoint2D>     FPixelsStack;
    /// Pixel en cours.
    CPoint2D            FCurrentPixel;
    /// Flag de fin de parcours.
    bool                   FStop;
  };

} // namespace pyramid
//*****************************************************************************
#endif // TRAVERSAL_REGION_PIXELS_HH
//*****************************************************************************
