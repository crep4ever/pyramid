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
#include "coverage-pixels-jou-tsai.hh"
#include <cassert>
#include <iostream>
using namespace std;
//*****************************************************************************
CCoveragePixelsRegionJouTsai::
CCoveragePixelsRegionJouTsai(CImage2D * ACimage2D,
                             const CPixel& APixel,
                             CArraySpans * AArraySpans) :
        FImage2D(ACimage2D),
        FCurrentPixel(APixel),
        FStop(false),
        FRegionId(ACimage2D->getPixel(APixel)),
        FStartSpan(APixel),
        FEndSpan(APixel),
        FFirstDir(true),
        FArraySpans(AArraySpans)
{
    if (FImage2D->isMarked(FCurrentPixel))
    {
        std::cerr << "PROBLEME dans CCoveragePixelsRegionJouTsai: "
        << "région déjà marqué." << std::endl;
        FStop = true;
    }
    else
    {
        FImage2D->setMark(FCurrentPixel);
    }
}
//*****************************************************************************
CCoveragePixelsRegionJouTsai::~CCoveragePixelsRegionJouTsai()
{ }
//*****************************************************************************
inline
void CCoveragePixelsRegionJouTsai::checkSeed(unsigned int Ay,
        unsigned int ALeftX,
        unsigned int ARightX)
{
    bool prevState = false;
    bool inSpan    = false;

    for (unsigned int i = ALeftX; i <= ARightX; ++i)
    {
        inSpan = (FImage2D ->getPixel(i, Ay) == FRegionId &&
                  !FImage2D->isMarked(i, Ay));

        if (prevState != inSpan)
        {
            if (inSpan)   // We have found a new seed
                FPixelsStack.push(CPixel(i, Ay));

            prevState = inSpan;
        }
    }
}
//*****************************************************************************
inline
void CCoveragePixelsRegionJouTsai::checkSpan(unsigned int Ay,
        unsigned int ALeftX,
        unsigned int ARightX)
{
    if (FArraySpans == NULL)
    {
        checkSeed(Ay, ALeftX, ARightX);
    }
    else
    {
        unsigned int intersection;
        int res = FArraySpans->checkList(Ay, ALeftX, ARightX,
                                         intersection);

        if (res == 1 || res == 5) // checkseed in full interval
            checkSeed(Ay, ALeftX, ARightX);
        else if (res == 3) // checkseed in left interval
            checkSeed(Ay, ALeftX, intersection);
        else if (res == 4) // checkseed in right interval
            checkSeed(Ay, intersection , ARightX);
    }
}
//*****************************************************************************
void CCoveragePixelsRegionJouTsai::operator++()
{
    assert(cont());

    // On commence par essayer de continuer le span courant.
    if (FFirstDir) FCurrentPixel.incX();
    else           FCurrentPixel.incX(-1);

    if (FImage2D ->isPixelInImage(FCurrentPixel) &&
            FImage2D ->getPixel(FCurrentPixel) == FRegionId &&
            !FImage2D->isMarked(FCurrentPixel))
    {
        // Ici on continue le span
        FImage2D->setMark(FCurrentPixel);

        if (FFirstDir)  FEndSpan.incX();
        else            FStartSpan.incX(-1);
    }
    else
        // We need either to change the direction,
        // or to take another seed for changing the current span
    {
        if (FFirstDir)
        {
            FFirstDir     = false;
            FCurrentPixel = FStartSpan;
            operator++();
        }
        else
        {
            if (FArraySpans != NULL)
                FArraySpans->pushFront(FCurrentPixel.getY(),
                                       FStartSpan.getX(),
                                       FEndSpan.getX());


            // Here, we have just finished to fill a span, thus we need to
            // test and push seeds around the span depending on
            // the current configuration
            if (FCurrentPixel.getY() > 0)
                checkSpan(FCurrentPixel.getY() - 1,
                          FStartSpan.getX(), FEndSpan.getX());

            if ((unsigned int)FCurrentPixel.getY() < FImage2D->getYSize() - 1)
                checkSpan(FCurrentPixel.getY() + 1,
                          FStartSpan.getX(), FEndSpan.getX());

            // Now we take the next seed
            // i.e. the next unmarked pixel in the stack
            do
            {
                if (FPixelsStack.empty())
                {
                    FStop = true;
                }
                else
                {
                    FCurrentPixel = FPixelsStack.top();
                    FPixelsStack.pop();

                    assert(FImage2D ->getPixel(FCurrentPixel) == FRegionId);
                }
            }
            while (cont() &&
                    FImage2D->isMarked(FCurrentPixel));

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
//*****************************************************************************
CPixel CCoveragePixelsRegionJouTsai::operator++(int)
{
    CPixel res = FCurrentPixel;
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoveragePixelsRegionJouTsai::cont() const
{ return !FStop; }
//*****************************************************************************
CPixel CCoveragePixelsRegionJouTsai::operator*() const
{ return FCurrentPixel; }
//*****************************************************************************
//*****************************************************************************
CCoverageAllPixelsJouTsai::
CCoverageAllPixelsJouTsai(CImage2D * ACimage2D, bool AWithArray) :
        FImage2D(ACimage2D),
        FCurrentPixel(CPixel(0, 0)),
        FNewRegion(true),
        FIteratorRegion(NULL),
        FArraySpans(NULL)

{
    if (AWithArray)
        FArraySpans = new CArraySpans(FImage2D->getYSize());

    FImage2D->unmarkAll();

    FIteratorRegion =
        new CCoveragePixelsRegionJouTsai(FImage2D, FCurrentPixel, FArraySpans);
    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire "
        << "dans CCoverageAllPixelsJouTsai" << std::endl;
    }
}
//*****************************************************************************
CCoverageAllPixelsJouTsai::~CCoverageAllPixelsJouTsai()
{
    delete FIteratorRegion;
    delete FArraySpans;
}
//*****************************************************************************
void CCoverageAllPixelsJouTsai::operator++()
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
                new CCoveragePixelsRegionJouTsai(FImage2D, FCurrentPixel,
                                                 FArraySpans);
            if (FIteratorRegion == NULL)
            {
                std::cerr << "Problème d'allocation mémoire "
                << "dans CCoverageAllPixelsJouTsai" << std::endl;
            }
            FNewRegion = true;
        }
    }
}
//*****************************************************************************
CPixel CCoverageAllPixelsJouTsai::operator++(int)
{
    CPixel res = operator*();
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoverageAllPixelsJouTsai::cont() const
{ return FIteratorRegion != NULL && FIteratorRegion->cont(); }
//*****************************************************************************
void CCoverageAllPixelsJouTsai::reinit()
{
    delete FIteratorRegion; FIteratorRegion = NULL;

    FCurrentPixel =  CPixel(0, 0);
    FNewRegion    = true;

    FImage2D->unmarkAll();
    FIteratorRegion = new CCoveragePixelsRegionJouTsai(FImage2D,
            FCurrentPixel,
            FArraySpans);
    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire "
        << "dans CCoverageAllPixelsJouTsai" << std::endl;
    }
}
//*****************************************************************************
bool CCoverageAllPixelsJouTsai::newRegion() const
{ return FNewRegion; }
//*****************************************************************************
CPixel CCoverageAllPixelsJouTsai::operator*() const
{
    assert(cont());
    return FIteratorRegion->operator*();
}
//*****************************************************************************
