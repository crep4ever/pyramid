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
#ifndef COVERAGE_PIXELS_JOU_TSAI_HH
#define COVERAGE_PIXELS_JOU_TSAI_HH
//*****************************************************************************
#include "image2D.hh"
#include "pixel.hh"
#include <stack>
//*****************************************************************************
class CListNode
{
    friend class CArraySpans;
public:
    CListNode() : FNextNode(NULL)
    {}

    CListNode(unsigned int ALeftX, unsigned int ARightX, CListNode* ANext) :
            FLeftX(ALeftX),
            FRightX(ARightX),
            FNextNode(ANext)
    {}

private:
    unsigned int FLeftX, FRightX;
    CListNode*   FNextNode;
};
//-----------------------------------------------------------------------------
class CArraySpans
{
public:
    CArraySpans() : FListHead(NULL)
    {}

    CArraySpans(unsigned int ASizeY)
    {
        FListHead = new CListNode*[ASizeY];
        for (unsigned int i = 0; i < ASizeY; ++i)
                FListHead[i] = NULL;
    }

    void pushFront(unsigned int Ay,
                   unsigned int ALeftX, unsigned int ARightX)
    {
        FListHead[Ay] = new CListNode(ALeftX, ARightX, FListHead[Ay]);
    }

    // Check the status of the span ([ALeftX,ARightX] Ay).
    // Return 1,2,3,4 or 5 depending the case (see original paper)
    // and in cases 3 and 4, AIntersection (which is an output paramer)
    // contains the intersection of the test span with the found span.
    unsigned int checkList(unsigned int Ay,
                           unsigned int ALeftX, unsigned int ARightX,
                           unsigned int& AIntersection)
    {
        CListNode* actu = FListHead[Ay];
        while (actu != NULL)
        {
            if (actu->FLeftX <= ALeftX)
            {
                if (actu->FRightX >= ARightX)
                    return 2;
                else if (actu->FRightX > ALeftX)
                {
                    AIntersection = actu->FLeftX;
                    return 4;
                }
                else
                    return 1;
            }
            else
            {
                if (actu->FLeftX <= ARightX)
                {
                    if (actu->FRightX >= ARightX)
                    {
                        AIntersection = actu->FRightX;
                        return 3;
                    }
                    else
                        return 5;
                }
                else return 1;
            }

            actu = actu->FNextNode;
        }
        return 1;
    }

private:
    CListNode** FListHead;
};
//-----------------------------------------------------------------------------
/**
 * Itérateur des pixels ayant la même étiquette que le pixel initial
 * en utilisant la méthode de Jou et Tsai in "A fast 3D seed-filling
 * algorithm".
 * ATTENTION : Pour que cette méthode marche, il faut que les marques
 * dans l'image2D soient toutes au préalable à faux (sinon certains
 * pixels ne seront pas parcourus).
 */
class CCoveragePixelsRegionJouTsai
{
public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ACimage2D une image.
     * @param APixel    le pixel de départ.
     */
    CCoveragePixelsRegionJouTsai(CImage2D *    ACimage2D,
                                 const CPixel& APixel,
                                 CArraySpans * AArraySpans = NULL);

    /// Destructeur.
    ~CCoveragePixelsRegionJouTsai();

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
     * Check each pixel of the span given by [ALeftX, ALeftY] in Ay,
     * and push in the stack FPixelsStack one seed by future span.
     */
    void checkSeed(unsigned int Ay,
                   unsigned int ALeftX, unsigned int ARightX);

    /*
     * Test the configuration of the span given by [ALeftX, ALeftY] in
     * Ay, and depending of this configuration, call checkSeed only onto
     * the part of the span to scan.
     */
    void checkSpan(unsigned int Ay,
                   unsigned int ALeftX, unsigned int ARightX);

protected:
    /// Pointeur sur l'image
    CImage2D * FImage2D;

    /// Pile des seeds que l'on doit traiter.
    std::stack<CPixel> FPixelsStack;

    /// Pixel en cours.
    CPixel FCurrentPixel;

    /// Flag de fin de parcours.
    bool FStop;

    /// L'identifiant de la région que l'on parcours.
    TLabel FRegionId;

    /// Start and end of the current span
    CPixel FStartSpan, FEndSpan;

    /// Bool which is true if we run through in the first direction (x+1),
    /// false otherwise (x-1).
    bool FFirstDir;

    /// The array of spans projected in XY plane
    CArraySpans * FArraySpans;
};
//*****************************************************************************
//*****************************************************************************
/**
 * Itérateur de toute les pixels de toute les régions de l'image.
 */
class CCoverageAllPixelsJouTsai
{
public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ACimage2D une image.
     */
    CCoverageAllPixelsJouTsai(CImage2D * ACimage2D,
                              bool AWithArray = true);

    /// Destructeur.
    ~CCoverageAllPixelsJouTsai();

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
    CCoveragePixelsRegionJouTsai* FIteratorRegion;

    /// The array of spans projected in XY plane
    CArraySpans* FArraySpans;
};
//*****************************************************************************
#endif // COVERAGE_PIXELS_JOU_TSAI_HH
//*****************************************************************************
