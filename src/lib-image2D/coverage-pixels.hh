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

//*****************************************************************************
#ifndef COVERAGE_PIXELS_HH
#define COVERAGE_PIXELS_HH
//*****************************************************************************
#include "image2D.hh"
#include "pixel.hh"
#include <stack>
//*****************************************************************************

/**
 * Itérateur des pixels ayant la même étiquette que le pixel initial.
 * ATTENTION : Pour que cette méthode marche, il faut que les marques
 * dans l'image2D soient toutes au préalable à faux (sinon certains
 * pixels ne seront pas parcourus).
 */
class CCoveragePixelsRegion
{
public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ACimage2D une image.
     * @param APixel    le pixel de départ.
     */
    CCoveragePixelsRegion(CImage2D * ACimage2D, const CPixel& APixel);

    /// Destructeur.
    ~CCoveragePixelsRegion();

    //@}

    /// Itérateur préfixe. Passe au pixel suivant.
    void operator++();

    /**
     * Itérateur postfixe.
     * @return Le pixel avant incrémentation.
     */
    CPixel operator++(int);

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
    CPixel operator*() const;

protected:
    /// Pointeur sur l'image
    CImage2D * FImage2D;

    /// Pile des pixels que l'on doit traiter et parcourir.
    std::stack<CPixel> FPixelsStack;

    /// Pixel en cours.
    CPixel FCurrentPixel;

    /// Flag de fin de parcours.
    bool FStop;

    /// L'identifiant de la région que l'on parcours.
    TLabel FRegionId;
};
//*****************************************************************************
//*****************************************************************************
/**
 * Itérateur de toute les pixels de toute les régions de l'image.
 */
class CCoverageAllPixels
{
public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ACimage2D une image.
     */
    CCoverageAllPixels(CImage2D * ACimage2D);

    /// Destructeur.
    ~CCoverageAllPixels();

    //@}

    /// Itérateur préfixe. Passe au pixel suivant.
    void operator++();

    /**
     * Itérateur postfixe.
     * @return Le pixel avant l'incrémentation.
     */
    CPixel operator++(int);

    /**
     * Drapeau d'arrêt du parcours.
     * @return <code>true</code> si il reste des pixels qui n'ont pas été
     * parcourus, <code>false</code> sinon.
     */
    bool cont() const;

    /// Réinitialisation du parcours.
    void reinit();

    /**
     * Test si l'on a changé de région ou pas.
     * @return <code>true</code> si le pixel courant est dans la même
     *         région que le pixel précédent (ou si c'est le premier pixel)
     *         <code>false</code> sinon.
     */
    bool newRegion() const;

    /**
     * Déréférencement de l'iterateur en pixel.
     * @return Le pixel actuel.
     */
    CPixel operator*() const;

protected:
    /// Pointeur sur l'image
    CImage2D * FImage2D;

    /// Pixel en cours.
    CPixel FCurrentPixel;

    /// Flag de nouvelle région
    bool FNewRegion;

    /// Le parcours de la région courante.
    CCoveragePixelsRegion* FIteratorRegion;
};
//*****************************************************************************
#endif // COVERAGE_PIXELS_HH
//*****************************************************************************
