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
#include "coverage-pixels.hh"
#include <cassert>
#include <iostream>
//*****************************************************************************
CCoveragePixelsRegion::CCoveragePixelsRegion(CImage2D * ACimage2D,
        const CPixel& APixel) :
        FImage2D(ACimage2D),
        FCurrentPixel(APixel),
        FStop(false),
        FRegionId(ACimage2D->getPixel(APixel))
{
    if (FImage2D->isMarked(FCurrentPixel))
    {
        std::cerr << "PROBLEME dans CCoveragePixelsRegion: "
        << "région déjà marqué." << std::endl;
        FStop = true;
    }
    else
    {
        FImage2D->setMark(FCurrentPixel);
    }
}
//*****************************************************************************
CCoveragePixelsRegion::~CCoveragePixelsRegion()
{}
//*****************************************************************************
void CCoveragePixelsRegion::operator++()
{
    assert(cont());

    // On commence par empiler les pixels voisin du pixel courant non
    // encore traités.
    CPixel next, findPixel;
    bool find = false;

    for (int i = FIRST_DIR; i < LAST_DIR; ++i)
    {
        if (FImage2D->existNeighboorPixel(FCurrentPixel, i))
        {
            next = FCurrentPixel.neighboor(i);
            if (FImage2D->getPixel(next) == FRegionId &&
                    !FImage2D->isMarked(next))
            {
                if (find)
                    FPixelsStack.push(next);
                else
                {
                    findPixel = next;
                    find = true;
                }

                FImage2D->setMark(next);
            }
        }
    }

    // Puis on récupère le prochain pixel si on ne l'a pas déjà trouvé.
    if (!find)
    {
        if (FPixelsStack.empty())
        {
            FStop = true;
        }
        else
        {
            FCurrentPixel = FPixelsStack.top();
            FPixelsStack.pop();
        }
    }
    else
    {
        FCurrentPixel = findPixel;
    }
}
//*****************************************************************************
CPixel CCoveragePixelsRegion::operator++(int)
{
    CPixel res = FCurrentPixel;
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoveragePixelsRegion::cont() const
{ return !FStop; }
//*****************************************************************************
CPixel CCoveragePixelsRegion::operator*() const
{ return FCurrentPixel; }
//*****************************************************************************
//*****************************************************************************
CCoverageAllPixels::CCoverageAllPixels(CImage2D * ACimage2D) :
        FImage2D(ACimage2D),
        FCurrentPixel(CPixel(0, 0)),
        FNewRegion(true),
        FIteratorRegion(NULL)
{
    FImage2D->unmarkAll();
    FIteratorRegion = new CCoveragePixelsRegion(FImage2D, FCurrentPixel);
    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire dans CCoverageAllPixels"
        << std::endl;
    }
}
//*****************************************************************************
CCoverageAllPixels::~CCoverageAllPixels()
{ delete FIteratorRegion; }
//*****************************************************************************
void CCoverageAllPixels::operator++()
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
                new CCoveragePixelsRegion(FImage2D, FCurrentPixel);
            if (FIteratorRegion == NULL)
            {
                std::cerr << "Problème d'allocation mémoire "
                << "dans CCoverageAllPixels" << std::endl;
            }

            FNewRegion = true;
        }
    }
}
//*****************************************************************************
CPixel CCoverageAllPixels::operator++(int)
{
    CPixel res = operator*();
    this->operator ++();
    return res;
}
//*****************************************************************************
bool CCoverageAllPixels::cont() const
{ return FIteratorRegion != NULL && FIteratorRegion->cont(); }
//*****************************************************************************
void CCoverageAllPixels::reinit()
{
    delete FIteratorRegion; FIteratorRegion = NULL;

    FCurrentPixel =  CPixel(0, 0);
    FNewRegion    = true;

    FImage2D->unmarkAll();
    FIteratorRegion = new CCoveragePixelsRegion(FImage2D, FCurrentPixel);
    if (FIteratorRegion == NULL)
    {
        std::cerr << "Problème d'allocation mémoire dans CCoverageAllPixels"
        << std::endl;
    }
}
//*****************************************************************************
bool CCoverageAllPixels::newRegion() const
{ return FNewRegion; }
//*****************************************************************************
CPixel CCoverageAllPixels::operator*() const
{
    assert(cont());
    return FIteratorRegion->operator*();
}
//*****************************************************************************
