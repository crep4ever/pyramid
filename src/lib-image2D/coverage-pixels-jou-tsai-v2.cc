/*
 * libImage2D : Librairie de manipulation d'une image 3D.
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
#include "coverage-pixels-jou-tsai-v2.hh"
#include <cassert>
#include <iostream>
using namespace std;
//*****************************************************************************
CCoveragePixelsRegionJouTsaiV2::
CCoveragePixelsRegionJouTsaiV2(CImage2D * ACimage2D,
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
        std::cerr << "PROBLEME dans CCoveragePixelsRegionJouTsaiV2: "
        << "région déjà marqué." << std::endl;
        FStop = true;
    }
    else
    {
        FImage2D->setMark(FCurrentPixel);
    }
}
//*****************************************************************************
CCoveragePixelsRegionJouTsaiV2::~CCoveragePixelsRegionJouTsaiV2()
{ }
//*****************************************************************************
inline
bool CCoveragePixelsRegionJouTsaiV2::findNextSeed()
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
inline
void  CCoveragePixelsRegionJouTsaiV2::pushPixel(const CPixel& APixel)
{
    FSpanStack.push(CSpan(APixel, 1 + FEndSpan.getX() - FStartSpan.getX()));
}
//*****************************************************************************
void CCoveragePixelsRegionJouTsaiV2::operator++()
{
    assert(cont());

    // On commence par essayer de continuer le span courant.
    if (FFirstDir) FCurrentPixel.incX(-1);
    else           FCurrentPixel.incX( + 1);

    if (FImage2D ->isPixelInImage(FCurrentPixel) &&
            FImage2D ->getPixel(FCurrentPixel) == FRegionId &&
            !FImage2D->isMarked(FCurrentPixel))
    {
        // Ici on continue le span
        FImage2D->setMark(FCurrentPixel);

        if (FFirstDir) FStartSpan.incX(-1);
        else           FEndSpan.incX();
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
            if (FStartSpan.getY() > 0)
                pushPixel(FStartSpan.bottom());

            if ((unsigned int)FStartSpan.getY() < FImage2D->getYSize() - 1)
                pushPixel(FStartSpan.top());

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
                        CSpan span(FSpanStack.top());
                        FSpanStack.pop();

                        FCurrentPixel = span.FPixel;

                        FFatherStartSpan = FCurrentPixel;
                        FFatherLg = span.FLg;

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
CPixel CCoveragePixelsRegionJouTsaiV2::operator++(int)
{
    CPixel res = FCurrentPixel;
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoveragePixelsRegionJouTsaiV2::cont() const
{ return !FStop; }
//*****************************************************************************
CPixel CCoveragePixelsRegionJouTsaiV2::operator*() const
{ return FCurrentPixel; }
//*****************************************************************************
//*****************************************************************************
CCoverageAllPixelsJouTsaiV2::
CCoverageAllPixelsJouTsaiV2(CImage2D * ACimage2D) :
        FImage2D(ACimage2D),
        FCurrentPixel(CPixel(0, 0)),
        FNewRegion(true),
        FIteratorRegion(NULL)
{
    FImage2D->unmarkAll();

    FIteratorRegion =
        new CCoveragePixelsRegionJouTsaiV2(FImage2D, FCurrentPixel);

    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire dans "
        << "CCoverageAllPixelsJouTsaiV2" << std::endl;
    }
}
//*****************************************************************************
CCoverageAllPixelsJouTsaiV2::~CCoverageAllPixelsJouTsaiV2()
{
    delete FIteratorRegion;
}
//*****************************************************************************
void CCoverageAllPixelsJouTsaiV2::operator++()
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
                new CCoveragePixelsRegionJouTsaiV2(FImage2D, FCurrentPixel);
            if (FIteratorRegion == NULL)
            {
                std::cerr << "Problème d'allocation mémoire "
                << "dans CCoverageAllPixelsJouTsaiV2" << std::endl;
            }
            FNewRegion = true;
        }
    }
}
//*****************************************************************************
CPixel CCoverageAllPixelsJouTsaiV2::operator++(int)
{
    CPixel res = operator*();
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoverageAllPixelsJouTsaiV2::cont() const
{ return FIteratorRegion != NULL && FIteratorRegion->cont(); }
//*****************************************************************************
void CCoverageAllPixelsJouTsaiV2::reinit()
{
    delete FIteratorRegion; FIteratorRegion = NULL;

    FCurrentPixel =  CPixel(0, 0);
    FNewRegion    = true;

    FImage2D->unmarkAll();
    FIteratorRegion = new CCoveragePixelsRegionJouTsaiV2(FImage2D,
            FCurrentPixel);
    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire dans "
        << "CCoverageAllPixelsJouTsaiV2" << std::endl;
    }
}
//*****************************************************************************
bool CCoverageAllPixelsJouTsaiV2::newRegion() const
{ return FNewRegion; }
//*****************************************************************************
CPixel CCoverageAllPixelsJouTsaiV2::operator*() const
{
    assert(cont());
    return FIteratorRegion->operator*();
}
//*****************************************************************************
