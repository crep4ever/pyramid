/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
 * Copyright (C) Guillaume Damiand, guillaume.damiand@liris.cnrs.fr
 *               http://liris.cnrs.fr/guillaume.damiand/
 *
 * This file is part of lib-mapkernel
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

//******************************************************************************
#ifndef EDGE_HH
#define EDGE_HH
//******************************************************************************
namespace Map2d
{
  class CDart;

  class CEdge
  {
    friend class CTopologicalMap;

  public:

    CEdge() : FGradient(0), FNbLinels(0)
    {}

    ~CEdge()
    {}

  protected:
    /// Le gradient de l'arête
    double FGradient;

    /// Le gradient de la dernière arête sauvée
    double FGradientSaved;

    /// Le nombre de linels de l'arête
    unsigned int FNbLinels;

    /// Le nb de linels de la dernière arête sauvée
    unsigned int FNbLinelsSaved;

  public:
    void save()
    {
      FGradientSaved = FGradient;
      FNbLinelsSaved = FNbLinels;
    }

    void restore()
    {
      FGradient = FGradientSaved;
      FNbLinels = FNbLinelsSaved;
    }

    double getGradient() const
    { return FGradient; }

    double getSavedGradient() const
    { return FGradientSaved; }

    void setGradient(double AValue)
    { FGradient=AValue; }

    void incGradient( double ANb=1 )
    { FGradient+= ANb; }

    unsigned int getNbLinels() const
    { return FNbLinels; }

    unsigned int getSavedNbLinels() const
    { return FNbLinelsSaved; }

    void setNbLinels(unsigned int ANb)
    { FNbLinels=ANb; }

    void incNbLinels( int ANb=1 )
    {
      assert(ANb>0 || FNbLinels>=(unsigned int)-ANb);
      FNbLinels+= ANb;
    }
  };
} // namespace Map2d
//******************************************************************************
#endif // EDGE_HH
//******************************************************************************
