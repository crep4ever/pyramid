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
#include "coverage-pixels-fishkin-barsky.hh"
#include <cassert>
#include <iostream>
using namespace std;
//*****************************************************************************
CCoveragePixelsRegionFishkinBarsky::
CCoveragePixelsRegionFishkinBarsky(CImage2D * ACimage2D,
                                   const CPixel& APixel) :
        FImage2D(ACimage2D),
        FCurrentPixel(APixel),
        FStop(false),
        FRegionId(ACimage2D->getPixel(APixel)),
        FStartSpan(APixel),
        FEndSpan(APixel),
        FFatherStartSpan(APixel),
        FFatherLg(0),
        FFirstDir(true)
{
    if (FImage2D->isMarked(FCurrentPixel))
    {
        std::cerr << "PROBLEME dans CCoveragePixelsRegionFishkinBarsky: "
        << "région déjà marqué." << std::endl;
        FStop = true;
    }
    else
    {
        FImage2D->setMark(FCurrentPixel);
    }
}
//*****************************************************************************
CCoveragePixelsRegionFishkinBarsky::~CCoveragePixelsRegionFishkinBarsky()
{ }
//*****************************************************************************
inline
void CCoveragePixelsRegionFishkinBarsky::
storeShadows(const CPixel & AMin, int ALg1,
             const CPixel & AFatherMin, int ALg2)
{
    CPixel tmp(AFatherMin);

    if (AMin.getX() < AFatherMin.getX() - 1)
    {
        tmp.setX(AMin.getX());
        FSpanStack.push(CSpanWithFather(tmp, AMin,
                                        AFatherMin.getX() - 1 - AMin.getX()));
    }

    if (AMin.getX() + ALg1 > AFatherMin.getX() + ALg2 + 1)
    {
        CPixel fatherStart(AMin);
        fatherStart.setX(AFatherMin.getX() + ALg2 + 1);

        tmp.setX(AFatherMin.getX() + ALg2 + 1);
        FSpanStack.push(CSpanWithFather(tmp, fatherStart,
                                        AMin.getX() + ALg1 -
                                        fatherStart.getX()));
    }
}
//*****************************************************************************
inline
bool CCoveragePixelsRegionFishkinBarsky::findNextSeed()
{
    bool found = false;

    while (!found &&
            FCurrentPixel.getX() - FFatherStartSpan.getX() < (int)FFatherLg)
    {
        found = (FImage2D ->getPixel(FCurrentPixel) == FRegionId &&
                 !FImage2D->isMarked(FCurrentPixel));

        if (!found) FCurrentPixel.incX();
    }

    return found;
}
//*****************************************************************************
void CCoveragePixelsRegionFishkinBarsky::operator++()
{
    assert(cont());

    // On commence par essayer de continuer le span courant.
    if (FFirstDir) FCurrentPixel.incX(-1);
    else             FCurrentPixel.incX( + 1);

    if (FImage2D ->isPixelInImage(FCurrentPixel) &&
            FImage2D ->getPixel(FCurrentPixel) == FRegionId &&
            !FImage2D->isMarked(FCurrentPixel))
    {
        // Ici on continue le span
        FImage2D->setMark(FCurrentPixel);

        if (FFirstDir) FStartSpan.incX(-1);
        else             FEndSpan.incX();
    }
    else
        // We need either to change the direction,
        // or to take another seed for changing the current span
    {
        if (FFirstDir)
        {
            FCurrentPixel = FEndSpan;
            FFirstDir     = false;
            operator++();
        }
        else
        {
            // std::cout<<"Span : "<<FStartSpan<<" -> "<<FEndSpan<<std::endl;

            unsigned int lg = FEndSpan.getX() - FStartSpan.getX();

            // On empile d'abord les ombres dans les 2 directions, en évitant
            // de revenir d'ou on vient (dans la direction du père)
            if (FStartSpan.getY() > 0 &&
                    FStartSpan.getY() - 1 != FFatherStartSpan.getY())
                FSpanStack.push(CSpanWithFather(FStartSpan.bottom(),
                                                FStartSpan, lg));

            if ((unsigned int)FStartSpan.getY() < FImage2D->getYSize() - 1 &&
                    FStartSpan.getY() + 1 != FFatherStartSpan.getY())
                FSpanStack.push(CSpanWithFather(FStartSpan.top(),
                                                FStartSpan, lg));

            // Maintenant on va tester si on doit empiler un ou 2 spans
            // dans la direction du père (cas du L, U et W cf article)
            storeShadows(FStartSpan, lg, FFatherStartSpan, FFatherLg);

            // On cherche un point de reprise en restant dans l'ombre du père.
            if (findNextSeed())
            {
                // Cas ou on repart d'un span dans l'ombre du père
                FImage2D->setMark(FCurrentPixel);

                FStartSpan = FCurrentPixel;
                FEndSpan  = FCurrentPixel;
                FFirstDir  = false;
            }
            else
            {
                // Now we take the next seed i.e. the next unmarked
                // pixel in the stack
                bool ok = false;
                do
                {
                    if (FSpanStack.empty())
                    {
                        FStop = true;
                    }
                    else
                    {
                        CSpanWithFather span(FSpanStack.top());
                        FSpanStack.pop();

                        FCurrentPixel    = span.FPixel;

                        FFatherStartSpan = span.FFatherPixel;
                        FFatherLg        = span.FFatherLg;

                        ok = findNextSeed();
                    }
                }
                while (cont() && !ok);


                if (cont())
                {
                    FImage2D->setMark(FCurrentPixel);
                    FStartSpan = FCurrentPixel;
                    FEndSpan   = FCurrentPixel;
                    FFirstDir  = true;
                }
            }
        }
    }
}
//*****************************************************************************
CPixel CCoveragePixelsRegionFishkinBarsky::operator++(int)
{
    CPixel res = FCurrentPixel;
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoveragePixelsRegionFishkinBarsky::cont() const
{ return !FStop; }
//*****************************************************************************
CPixel CCoveragePixelsRegionFishkinBarsky::operator*() const
{ return FCurrentPixel; }
//*****************************************************************************
//*****************************************************************************
CCoverageAllPixelsFishkinBarsky::
CCoverageAllPixelsFishkinBarsky(CImage2D * ACimage2D) :
        FImage2D(ACimage2D),
        FCurrentPixel(CPixel(0, 0)),
        FNewRegion(true),
        FIteratorRegion(NULL)
{
    FImage2D->unmarkAll();

    FIteratorRegion =
        new CCoveragePixelsRegionFishkinBarsky(FImage2D, FCurrentPixel);

    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire dans "
        << "CCoverageAllPixelsFishkinBarsky" << std::endl;
    }
}
//*****************************************************************************
CCoverageAllPixelsFishkinBarsky::~CCoverageAllPixelsFishkinBarsky()
{
    delete FIteratorRegion;
}
//*****************************************************************************
void CCoverageAllPixelsFishkinBarsky::operator++()
{
    assert(cont());

    FIteratorRegion->operator++();
    FNewRegion = false;

    if (!FIteratorRegion->cont())
    {
        delete FIteratorRegion; FIteratorRegion = NULL;

        TPixelCoord x = FCurrentPixel.getX();
        TPixelCoord y = FCurrentPixel.getY();

         while ((unsigned int)y < FImage2D->getYSize() &&
                    FImage2D->isMarked(FCurrentPixel))
            {
                while ((unsigned int)x < FImage2D->getXSize() &&
                        FImage2D->isMarked(FCurrentPixel))
                {
                    ++x;
                    FCurrentPixel.setX(x);
                }
                if ((unsigned int)x == FImage2D->getXSize())
                {
                    x = 0;
                    ++y;
                    FCurrentPixel.set(x, y);
                }
            }

        if ((unsigned int)y != FImage2D->getYSize()) 
            // Sinon c'est la fin du parcours
        {
            FIteratorRegion =
                new CCoveragePixelsRegionFishkinBarsky(FImage2D, FCurrentPixel);
            if (FIteratorRegion == NULL)
            {
                std::cerr << "Problème d'allocation mémoire "
                << "dans CCoverageAllPixelsFishkinBarsky" << std::endl;
            }
            FNewRegion = true;
        }
    }
}
//*****************************************************************************
CPixel CCoverageAllPixelsFishkinBarsky::operator++(int)
{
    CPixel res = operator*();
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoverageAllPixelsFishkinBarsky::cont() const
{ return FIteratorRegion != NULL && FIteratorRegion->cont(); }
//*****************************************************************************
void CCoverageAllPixelsFishkinBarsky::reinit()
{
    delete FIteratorRegion; FIteratorRegion = NULL;

    FCurrentPixel =  CPixel(0, 0);
    FNewRegion    = true;

    FImage2D->unmarkAll();
    FIteratorRegion = new CCoveragePixelsRegionFishkinBarsky(FImage2D,
            FCurrentPixel);
    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire dans "
        << "CCoverageAllPixelsFishkinBarsky" << std::endl;
    }
}
//*****************************************************************************
bool CCoverageAllPixelsFishkinBarsky::newRegion() const
{ return FNewRegion; }
//*****************************************************************************
CPixel CCoverageAllPixelsFishkinBarsky::operator*() const
{
    assert(cont());
    return FIteratorRegion->operator*();
}
//*****************************************************************************
