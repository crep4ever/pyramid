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

//******************************************************************************
#include "image2D.hh"

#include<string>
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<stdint.h>

#include<cassert>
#include<cmath>

// Pour faire des forks...
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
using namespace Magick;
//******************************************************************************
CImage2D::CImage2D() :
        FIsOk(false),
        FImage(NULL), FMark(NULL), FUFTree(NULL),
        FDisplayMin(0), FDisplayMax(-1)
{}
//******************************************************************************
bool CImage2D::allocateData(unsigned int Ax, unsigned int Ay)
{
    delete []FImage;
    delete []FMark;

    delete []FUFTree; FUFTree = NULL; FValidUF = false;

    setXSize(Ax); setYSize(Ay);

    FMinLevel = (TLabel(-1));     // La plus grande valeur possible.
    FMaxLevel = 0; // La plus petite valeur possible.

    FImage = new TLabel[getXSize()*getYSize()];
    FMark  = new bool  [getXSize()*getYSize()];

    FIsOk = (FImage != NULL && FMark != NULL);
    if (!isOk())
    {
        cerr << "Echec d'allocation mémoire dans allocateData." << endl;
        delete []FImage; FImage = NULL;
        return false;
    }

    return true;
}
//******************************************************************************
CImage2D::CImage2D(unsigned int Ax, unsigned int Ay) :
        FImage(NULL), FMark(NULL), FUFTree(NULL),
        FDisplayMin(0), FDisplayMax(-1)

{
    if (allocateData(Ax, Ay))
    {
        emptyImage();
        unmarkAll();
    }
}
//******************************************************************************
CImage2D& CImage2D::operator= (const CImage2D & AImage2D)
{
    if (this == &AImage2D || !AImage2D.isOk()) return *this;

    if (allocateData(AImage2D.getXSize(), AImage2D.getYSize()))
    {
        for (unsigned int y = 0; y < getYSize(); ++y)
            for (unsigned int x = 0; x < getXSize(); ++x)
            {
                setPixel(x, y, AImage2D.getPixel(x, y));
                if (AImage2D.isMarked(x, y)) setMark(x, y);
            }

        FDisplayMin = AImage2D.FDisplayMin;
        FDisplayMax = AImage2D.FDisplayMax;
    }
    return *this;
}
//******************************************************************************
CImage2D::CImage2D(const CImage2D & AImage2D) :
        FImage(NULL), FMark(NULL), FUFTree(NULL)
{ operator=(AImage2D); }
//******************************************************************************
/*
CImage2D::CImage2D(const char * AFilename) :
        FImage(NULL), FMark(NULL), FUFTree(NULL)
{ importWithMagick(AFilename); }
*/
//******************************************************************************
CImage2D::CImage2D(const char * AFilename,
                   unsigned int AXLarg, unsigned int AYLarg) :
        FImage(NULL), FMark(NULL), FUFTree(NULL)
{ loadRaw(AFilename, AXLarg, AYLarg); }
//******************************************************************************
CImage2D::~CImage2D()
{
    delete []FImage;  FImage = NULL;
    delete []FMark;   FMark = NULL;
    delete []FUFTree; FUFTree = NULL;
}
//******************************************************************************
bool CImage2D::isOk() const
{ return FIsOk; }
//*****************************************************************************
unsigned int CImage2D::getNumberOfBytes() const
{
    return sizeof(FImage) + sizeof(FMark) + sizeof(FUFTree);
}
//******************************************************************************
unsigned int CImage2D::getXSize() const
{ return FXSize; }
//******************************************************************************
unsigned int CImage2D::getYSize() const
{ return FYSize; }
//******************************************************************************
void CImage2D::setXSize(unsigned int Ax)
{
    assert(FImage == NULL);
    FXSize = Ax;
}
//------------------------------------------------------------------------------
void CImage2D::setYSize(unsigned int Ay)
{
    assert(FImage == NULL);
    FYSize = Ay;
}
//*****************************************************************************
bool CImage2D::isPixelInImage(int Ax, int Ay) const
{
    assert(isOk());
    return Ax >= 0 && Ay >= 0 &&
           (unsigned)Ax < getXSize() && (unsigned)Ay < getYSize();
}
//------------------------------------------------------------------------------
bool CImage2D::isPixelInImage(const CPixel& APixel) const
{
    assert(isOk());
    return APixel.getX() >= 0 && APixel.getY() >= 0 &&
           (unsigned)APixel.getX() < getXSize() &&
           (unsigned)APixel.getY() < getYSize();
}
//******************************************************************************
bool CImage2D::existNeighboorPixel(const CPixel& APixel, int ADir) const
{
    assert(isPixelInImage(APixel));
    assert(ADir >= FIRST_DIR && ADir < LAST_DIR_8CONNECTED);
    return isPixelInImage(APixel.neighboor(ADir));
}
//------------------------------------------------------------------------------
bool CImage2D::isBoundaryPixelInDir(const CPixel& APixel, int ADir) const
{
    assert(isPixelInImage(APixel));
    assert(ADir >= FIRST_DIR && ADir < LAST_DIR);

    CPixel n(APixel.neighboor(ADir));

    return !isPixelInImage(n) || getPixel(n) != getPixel(APixel);
}
//------------------------------------------------------------------------------
bool CImage2D::isBoundaryPixel(const CPixel& APixel) const
{
    assert(isPixelInImage(APixel));

    for (int dir = FIRST_DIR; dir < LAST_DIR; ++dir)
    {
        if (isBoundaryPixelInDir(APixel, dir))
            return true;
    }
    return false;
}
//------------------------------------------------------------------------------
bool CImage2D::isBoundaryPixel(const CPixel& APixel, TLabel ARegion) const
{
    if (!isPixelInImage(APixel) || getPixel(APixel) != ARegion)
        return false;

    return isBoundaryPixel(APixel);
}
//******************************************************************************
TLabel CImage2D::getMinLabel() const
{ return FMinLevel; }
//------------------------------------------------------------------------------
TLabel CImage2D::getMaxLabel() const
{ return FMaxLevel; }   
//------------------------------------------------------------------------------
TLabel CImage2D::getDisplayMin() const
{ return FDisplayMin; }
//------------------------------------------------------------------------------
TLabel CImage2D::getDisplayMax() const
{ return FDisplayMax; }
//------------------------------------------------------------------------------
void CImage2D::setDisplayMin(TLabel AValue)
{
    assert(AValue < FDisplayMax);
    FDisplayMin = AValue;
}
//------------------------------------------------------------------------------
void CImage2D::setDisplayMax(TLabel AValue)
{
    assert(AValue > FDisplayMin);
    FDisplayMax = AValue;
}
//------------------------------------------------------------------------------
void CImage2D::setDisplayMinMax(TLabel AValue1, TLabel AValue2)
{
    assert(AValue1 < AValue2);
    FDisplayMin = AValue1;
    FDisplayMax = AValue2;
}
//------------------------------------------------------------------------------
void CImage2D::maximiseContrast()
{
    FDisplayMin = FMinLevel;
    FDisplayMax = FMaxLevel;
}
//******************************************************************************
Magick::ColorGray CImage2D::getPixelColor(unsigned int Ax,
        unsigned int Ay) const
{
    TLabel c = getPixel(Ax, Ay);

    if (c > FDisplayMax) c = FDisplayMax;
    if (c < FDisplayMin) c = FDisplayMin;

    c -= FDisplayMin;

    return ColorGray(c / double(FDisplayMax - FDisplayMin));
}
//------------------------------------------------------------------------------
void CImage2D::setPixelColor(unsigned int Ax, unsigned int Ay,
                             const Magick::ColorGray & AColor)
{
    TLabel c =
        static_cast<TLabel>
        (round((AColor.shade()) * (FDisplayMax - FDisplayMin)));

    setPixel(Ax, Ay, c + FDisplayMin);
}
//******************************************************************************
unsigned int CImage2D::indiceDansTab(unsigned int Ax, unsigned int Ay) const
{
    assert(isPixelInImage(Ax, Ay));

    return (Ay*getXSize()) + Ax;
}
//-----------------------------------------------------------------------------
unsigned int CImage2D::indiceDansTab(const CPixel& APixel) const
{
    assert(isPixelInImage(APixel));

    return (APixel.getY()*getXSize()) + APixel.getX();
}
//******************************************************************************
TLabel CImage2D::getPixel(unsigned int Ax, unsigned int Ay) const
{ return FImage[indiceDansTab(Ax, Ay)]; }
//-----------------------------------------------------------------------------
TLabel CImage2D::getPixel(const CPixel& APixel) const
{ return FImage[indiceDansTab(APixel)]; }
//******************************************************************************
void CImage2D::setPixel(unsigned int Ax, unsigned int Ay,
                        const TLabel & AVal)
{
    if (getPixel(Ax, Ay) != AVal)
    {
        FImage[indiceDansTab(Ax, Ay)] = AVal;

        // On conserve le label minimum et maximum (utile pour l'auto-contrast).
        if (AVal < FMinLevel) FMinLevel = AVal;
        if (AVal > FMaxLevel) FMaxLevel = AVal;

        FValidUF = false;
    }
}
//-----------------------------------------------------------------------------
void CImage2D::setPixel(const CPixel& APixel, const TLabel & AVal)
{ setPixel(APixel.getX(), APixel.getY(), AVal); }
//******************************************************************************
void CImage2D::setMarkTo(unsigned int Ax, unsigned int Ay, bool ABool)
{ FMark[indiceDansTab(Ax, Ay)] = ABool; }
//-----------------------------------------------------------------------------
void CImage2D::setMarkTo(const CPixel& APixel, bool ABool)
{ FMark[indiceDansTab(APixel)] = ABool; }
//******************************************************************************
void CImage2D::setMark(unsigned int Ax, unsigned int Ay)
{ FMark[indiceDansTab(Ax, Ay)] = true; }
//-----------------------------------------------------------------------------
void CImage2D::setMark(const CPixel& APixel)
{ FMark[indiceDansTab(APixel)] = true; }
//******************************************************************************
void CImage2D::unsetMark(unsigned int Ax, unsigned int Ay)
{ FMark[indiceDansTab(Ax, Ay)] = false; }
//-----------------------------------------------------------------------------
void CImage2D::unsetMark(const CPixel& APixel)
{ FMark[indiceDansTab(APixel)] = false; }
//******************************************************************************
bool CImage2D::isMarked(unsigned int Ax, unsigned int Ay) const
{ return FMark[indiceDansTab(Ax, Ay)]; }
//-----------------------------------------------------------------------------
bool CImage2D::isMarked(const CPixel& APixel) const
{ return FMark[indiceDansTab(APixel)]; }
//*****************************************************************************
bool CImage2D::samePixelActuLeft(unsigned int Ax, unsigned int Ay) const
{
    assert(isPixelInImage(Ax, Ay) || Ax == getXSize() || Ay == getYSize());
    if (Ay == getYSize()) return true;
    if (Ax == 0 || Ax == getXSize()) return false;
    return getPixel(Ax, Ay) == getPixel(Ax - 1, Ay);
}
//*****************************************************************************
bool CImage2D::samePixelActuUp(unsigned int Ax, unsigned int Ay) const
{
    assert(isPixelInImage(Ax, Ay) || Ax == getXSize() || Ay == getYSize());
    if (Ax == getXSize()) return true;
    if (Ay == 0 || Ay == getYSize()) return false;
    return getPixel(Ax, Ay) == getPixel(Ax, Ay - 1);
}
//******************************************************************************
void CImage2D::unmarkAll()
{
    assert(isOk());

    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            unsetMark(x, y);
}
//******************************************************************************
void CImage2D::fillImage(TLabel AIdRegion)
{
    assert(isOk());

    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            setPixel(x, y, AIdRegion);
}
//******************************************************************************
void CImage2D::emptyImage()
{ fillImage(NULL_LABEL); }
//******************************************************************************
void CImage2D::partialFillImage(const std::set<TLabel> & ASet,
                                TLabel AIdRegion)
{
    assert(isOk());

    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (ASet.find(getPixel(x, y)) != ASet.end())
                setPixel(x, y, AIdRegion);
}
//******************************************************************************
void CImage2D::partialEmptyImage(const std::set<TLabel> & ASet)
{ partialFillImage(ASet, NULL_LABEL); }
//******************************************************************************
TLabel CImage2D::moyenneX(unsigned int Ax, unsigned int Ay,
                          unsigned int An)
{
    long res = 0;
    unsigned int i = 0;
    while (i < An && i + Ax < getXSize())
    {
        res += getPixel(Ax + i, Ay);
        ++i;
    }

    return static_cast<TLabel>(res / i);
}
//******************************************************************************
TLabel CImage2D::moyenneY(unsigned int Ax, unsigned int Ay,
                          unsigned int An)
{
    long res = 0;
    unsigned int i = 0;
    while (i < An && i + Ay < getYSize())
    {
        res += getPixel(Ax, Ay + i);
        ++i;
    }

    return static_cast<TLabel>(res / i);
}
//******************************************************************************
TLabel CImage2D::moyenneXY(unsigned int Ax, unsigned int Ay,
                           unsigned int An)
{
    long res = 0;
    unsigned int i = 0;
    while (i < An && i + Ay < getYSize())
    {
        res += moyenneX(Ax, Ay + i, An);
        ++i;
    }

    return static_cast<TLabel>(res / i);
}
//******************************************************************************
void CImage2D::divideImageX(int ACoef, bool AMoy)
{
    unsigned int newXSize = (getXSize() + ACoef - 1) / ACoef;
    CImage2D im(newXSize, getYSize());
    if (!im.isOk()) return;

    TLabel new_val;

    freeUFMatrix();

    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < newXSize; ++x)
        {
            if (AMoy) new_val = moyenneX(x * ACoef, y, ACoef);
            else new_val = getPixel(x * ACoef, y);
            im.setPixel(x, y, new_val);
        }

    delete []FImage;
    FImage = im.FImage;
    im.FImage = NULL;
    setXSize(newXSize);
}
//******************************************************************************
void CImage2D::divideImageY(int ACoef, bool AMoy)
{
    unsigned int newYSize = (getYSize() + ACoef - 1) / ACoef;
    CImage2D im(getXSize(), newYSize);
    if (!im.isOk()) return;

    TLabel new_val;

    freeUFMatrix();

    for (unsigned int y = 0; y < newYSize; ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
        {
            if (AMoy) new_val = moyenneY(x, y * ACoef, ACoef);
            else new_val = getPixel(x, y * ACoef);
            im.setPixel(x, y, new_val);
        }

    delete []FImage;
    FImage = im.FImage;
    im.FImage = NULL;
    setYSize(newYSize);
}
//******************************************************************************
void CImage2D::divideImageXY(int ACoef, bool AMoy)
{
    unsigned int newXSize = (getXSize() + ACoef - 1) / ACoef;
    unsigned int newYSize = (getYSize() + ACoef - 1) / ACoef;
    CImage2D im(newXSize, newYSize);
    if (!im.isOk()) return;

    freeUFMatrix();

    TLabel new_val;

    for (unsigned int y = 0; y < newYSize; ++y)
        for (unsigned int x = 0; x < newXSize; ++x)
        {
            if (AMoy) new_val = moyenneXY(x * ACoef, y * ACoef, ACoef);
            else new_val = getPixel(x * ACoef, y * ACoef);
            im.setPixel(x, y, new_val);
        }

    delete []FImage;
    FImage = im.FImage;
    im.FImage = NULL;
    setXSize(newXSize);
    setYSize(newYSize);
}
//******************************************************************************
Magick::Image CImage2D::getImage() const
{
    assert(isOk());

    Image res;
    //res.size(Geometry(getXSize(), getYSize()));
    res.type(GrayscaleType);

    Pixels view(res);
    PixelPacket *pixels = view.get(0, 0, getXSize(), getYSize());

    unsigned int x, y;
    for (y = 0; y < getYSize();++y)
        for (x = 0; x < getXSize();++x)
            *pixels++ = getPixelColor(x, y);

    view.sync();
    return res;
}
//******************************************************************************
void CImage2D::setImage(Magick::Image & AImage)
{
    assert(isOk());
    assert(getXSize() == AImage.columns() && getYSize() == AImage.rows());

    Pixels view(AImage);
    const PixelPacket *pixels = view.getConst(0, 0, getXSize(), getYSize());

    unsigned int x, y;
    for (y = 0; y < getYSize();++y)
        for (x = 0; x < getXSize();++x)
            setPixelColor(x, y, ColorGray(*pixels++));
}
//******************************************************************************
void CImage2D::displayImage(int AZoom) const
{
    Image res(getImage());
    if (AZoom < -1 || AZoom > 1)
    {
        //if (AZoom > 1)
        //    res.zoom(Geometry(getXSize()*AZoom, getYSize()*AZoom));
        //else
        //    res.zoom(Geometry((unsigned int)(getXSize() / abs(AZoom)),
      //                     (unsigned int)(getYSize() / abs(AZoom))));
    }

    res.display();
}
//******************************************************************************
bool CImage2D::loadRaw(const char * AFilename, int AXLarg, int AYLarg)
{
// 1) On initialise la matrice
    if (!allocateData(AXLarg, AYLarg)) return false;

// 2) On lit l'image
    ifstream is;
    uint16_t val16;
    is.open(AFilename, ios::binary);
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
        {
            is.read((char *) &val16, sizeof(val16));

            setPixel(x, y, val16);
        }
    is.close();
    return true;
}
//*****************************************************************************
void CImage2D::writeRaw(const char * AFilename) const
{
    assert(isOk());

    ofstream os;
    uint16_t val16;

    os.open(AFilename, ios::out | ios::binary | ios::trunc);
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
        {
            val16 = static_cast<uint16_t>(getPixel(x, y));
            os.write((char *) &val16, sizeof(val16));
        }
    os.close();
}
//******************************************************************************
bool CImage2D::importWithMagick(const char * AFilename)
{
// 1) On lit l'image
    Image imageTemp;
    try
        { imageTemp.read(AFilename); }
    catch (Magick::Warning &warn_)
        {}
    catch (Magick::Exception &error_)
    {
        cerr << "Erreur lors de la lecture de l'image "
        << AFilename << " : " << error_.what() << endl;
        return false;
    }

// 2) On initialise la matrice
    if (!allocateData(imageTemp.columns(), imageTemp.rows()))
        return false;

// 3) On affecte cette image dans notre tableau
    setImage(imageTemp);
    return true;
}
//*****************************************************************************
void CImage2D::exportWithMagick(const char * AFilename) const
{
    assert(isOk());

    getImage().write(AFilename);
}
//*****************************************************************************
CUFTree* CImage2D::getUFTree(unsigned int Ax, unsigned int Ay) const
{
    assert(FUFTree != NULL);

    return &FUFTree[indiceDansTab(Ax,Ay)];
}
//*****************************************************************************
CUFTree* CImage2D::findUFTree(unsigned int Ax, unsigned int Ay)
{ return getUFTree(Ax, Ay)->find(); }
//*****************************************************************************
bool CImage2D::isRootUFTree(unsigned int Ax, unsigned int Ay)
{ return getUFTree(Ax, Ay) == findUFTree(Ax, Ay); }
//*****************************************************************************
void CImage2D::mergeUFTree(unsigned int Ax, unsigned int Ay,
                           unsigned int Ax2, unsigned int Ay2)
{
    if (findUFTree(Ax, Ay) != findUFTree(Ax2, Ay2))
    {
        getUFTree(Ax, Ay)->merge(getUFTree(Ax2, Ay2));
        FValidUF = false;
    }
}
//*****************************************************************************
TLabel CImage2D::getPixel(CUFTree * AUFTree) const
{
    assert(FUFTree != NULL);
    assert(AUFTree >= FUFTree);
    assert(AUFTree < FUFTree + (getXSize()*getYSize()));
    return FImage[ AUFTree-FUFTree ];
}
//*****************************************************************************
void CImage2D::allocateUFMatrix()
{
    if (FUFTree == NULL)
    {
        FUFTree = new CUFTree[getXSize() *getYSize()];
        if (FUFTree == NULL)
            cerr << "Echec d'allocation mémoire dans allocateUFMatrix."
            << endl;
    }
}
//*****************************************************************************
void CImage2D::initialiseUFMatrix(bool AForceUpdate)
{
    if (!FValidUF || AForceUpdate)
    {
        allocateUFMatrix();

        for (unsigned int y = 0; y < getYSize(); ++y)
            for (unsigned int x = 0; x < getXSize(); ++x)
            {
                if (samePixelActuLeft(x, y))
                    mergeUFTree(x, y, x - 1, y);

                if (samePixelActuUp(x, y))
                    mergeUFTree(x, y, x, y - 1);
            }

        FValidUF = true;
    }
}
//*****************************************************************************
void CImage2D::freeUFMatrix()
{
    delete []FUFTree; FUFTree = NULL;
    FValidUF = false;
}
//*****************************************************************************
unsigned int CImage2D::getNbRegions()
{
    unsigned int nbRegions = 0;

    initialiseUFMatrix();
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (isRootUFTree(x, y))
                ++nbRegions;

    return nbRegions;
}
//*****************************************************************************
void CImage2D::displayRegions()
{
    unsigned int nbRegions = 0;
    unsigned int nbPixels = 0;

    initialiseUFMatrix();
    cout << "******** Régions dans l'image ********" << endl;
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (isRootUFTree(x, y))
            {
                ++nbRegions;
                nbPixels += getUFTree(x, y)->getNbElements();
                cout << "   Région " << nbRegions << " : label "
                << getPixel(x, y) << ", "
                << getUFTree(x, y)->getNbElements() << " pixel";
                if (getUFTree(x, y)->getNbElements() > 1) cout << "s";
                cout << "." << endl;
            }
    cout << "******** Total : " << nbRegions << " régions, "
    << nbPixels << " pixels. ********" << endl;
}
//*****************************************************************************
unsigned int CImage2D::labelRegions()
{
    TLabel label = 0;

    initialiseUFMatrix();
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (isRootUFTree(x, y))
            {
                ++label;
                setPixel(x, y, label);
            }
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (!isRootUFTree(x, y))
                setPixel(x, y, getPixel(findUFTree(x, y)));

    return label;
}
//*****************************************************************************
unsigned int CImage2D::partialLabelRegions(const std::set<TLabel>& ASet)
{
    TLabel label = 0;

    initialiseUFMatrix();
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (isRootUFTree(x, y) &&
                    ASet.find(getPixel(x, y)) != ASet.end())
            {
                ++label;
                setPixel(x, y, FMaxLevel + 1);
            }
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (!isRootUFTree(x, y) &&
                    ASet.find(getPixel(x, y)) != ASet.end())
                setPixel(x, y, getPixel(findUFTree(x, y)));

    return label;
}
//*****************************************************************************
void CImage2D::displayLabels()
{
    unsigned int nbLabels = 0;
    unsigned int nbRegions = 0;
    map< TLabel, vector<unsigned int> > labels;
    unsigned int taille = 0;
    unsigned int nbPixels = 0;

    initialiseUFMatrix();
    cout << "******** Labels dans l'image ********" << endl;
    for (unsigned int y = 0; y < getYSize(); ++y)
        for (unsigned int x = 0; x < getXSize(); ++x)
            if (isRootUFTree(x, y))
            {
                labels[getPixel(x, y)].
                push_back(getUFTree(x, y)->getNbElements());
                ++nbRegions;
                nbPixels += getUFTree(x, y)->getNbElements();
            }

    for (map< TLabel, vector<unsigned int> >::iterator it = labels.begin();
            it != labels.end(); ++it)
    {
        ++nbLabels;
        taille = (*it).second.size();
        cout << "   Label " << (*it).first << " : " << taille
        << " région" << (taille > 1 ? "s" : "")
        << " : taille" << (taille > 1 ? "s" : "") << " : ";
        for (unsigned int i = 0; i < (*it).second.size(); ++i)
        {
            cout << (*it).second[i];
            if (i < (*it).second.size() - 1) cout << ", ";
        }
        cout << "." << endl;
    }
    cout << "******** Total : " << nbLabels << " labels, "
    << nbRegions << " régions, "
    << nbPixels << " pixels. ********" << endl;
    if (nbLabels == nbRegions)
        cout << " => Image bien formée." << endl;
    else
        cout << " => Image pas bien formée." << endl;
}
//*****************************************************************************
