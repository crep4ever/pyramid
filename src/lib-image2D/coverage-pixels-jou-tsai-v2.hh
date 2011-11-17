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
#ifndef COVERAGE_PIXELS_JOU_TSAI_V2_HH
#define COVERAGE_PIXELS_JOU_TSAI_V2_HH
//*****************************************************************************
#include "image2D.hh"
#include "pixel.hh"
#include <stack>
//*****************************************************************************
/// Classe permettant de stocker un span de direction X en donnant les
/// coordonnées du premier pixel et la longueur
class CSpan
{
public:
    CSpan(const CPixel& APixel, unsigned int ALg) :
            FPixel(APixel),
            FLg(ALg)
    {}

    CSpan(const CSpan& ASpan) :
            FPixel(ASpan.FPixel),
            FLg(ASpan.FLg)
    {}

    /// Variables membres.
    CPixel       FPixel;
    unsigned int FLg;
};
//*****************************************************************************
/**
 * Itérateur des pixels ayant la même étiquette que le pixel initial
 * en utilisant la méthode de Jou et Tsai in "A fast 3D seed-filling
 * algorithm" modifié pour diminuer le nombre de pixels testés.
 * ATTENTION : Pour que cette méthode marche, il faut que les marques
 * dans l'image2D soient toutes au préalable à faux (sinon certains
 * pixels ne seront pas parcourus).
 */
class CCoveragePixelsRegionJouTsaiV2
{
public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ACimage2D une image.
     * @param APixel    le pixel de départ.
     */
    CCoveragePixelsRegionJouTsaiV2(CImage2D *    ACimage2D,
                                   const CPixel& APixel);

    /// Destructeur.
    ~CCoveragePixelsRegionJouTsaiV2();

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
    /*
     * Check each pixel of the father span, trying to find a new seed.
     */
    bool findNextSeed();

    /// Push APixel in the stack of seed, with the length of the father
    /// span computed by using FStartSpan and FEndSpan
    void pushPixel(const CPixel& APixel);

protected:
    /// Pointeur sur l'image
    CImage2D * FImage2D;

    /// Pile des seeds que l'on doit traiter.
    std::stack<CSpan> FSpanStack;

    /// Pixel en cours.
    CPixel FCurrentPixel;

    /// Flag de fin de parcours.
    bool FStop;

    /// L'identifiant de la région que l'on parcours.
    TLabel FRegionId;

    /// Start and end of the current span
    CPixel FStartSpan, FEndSpan;

    // Start and length of the father span
    CPixel FFatherStartSpan;
    unsigned int FFatherLg;

    /// Bool which is true if we run through in the first direction (x+1),
    /// false otherwise (x-1).
    bool FFirstDir;
};
//*****************************************************************************
//*****************************************************************************
/**
 * Itérateur de toute les pixels de toute les régions de l'image.
 */
class CCoverageAllPixelsJouTsaiV2
{
public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ACimage2D une image.
     */
    CCoverageAllPixelsJouTsaiV2(CImage2D * ACimage2D);

    /// Destructeur.
    ~CCoverageAllPixelsJouTsaiV2();

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
    CCoveragePixelsRegionJouTsaiV2* FIteratorRegion;
};
//*****************************************************************************
#endif // COVERAGE_PIXELS_JOU_TSAI_V2_HH
//*****************************************************************************
