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
#ifndef CONTOUR_HH
#define CONTOUR_HH
//******************************************************************************
namespace ImaGene { class GridCurve; }
//******************************************************************************
namespace Map2d
{
  class CDart;

  /// Un contour, c'est :
  ///   - un brin représentant.
  ///   - des attributs
  class CContour
  {
    friend class CTopologicalMap;

  public:

    CContour() : FDart(NULL)
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_LINEL
	       , FNbLinels(0)
#elif DEFORMABLE_METHOD==CONTOUR_ENERGY_MLP
	       , FLengthMLP(0)
#elif DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
	       , FLengthMLP(0)
	       , FDynamicMLP(NULL)
	       , FExternalContour(false)
#endif // DEFORMABLE_METHOD
    {}

    CContour( CDart* ADart ) : FDart(ADart)
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_LINEL
	       , FNbLinels(0)
#elif DEFORMABLE_METHOD==CONTOUR_ENERGY_MLP
	       , FLengthMLP(0)
#elif DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
	       , FLengthMLP(0)
	       , FDynamicMLP(NULL)
	       , FExternalContour(false)
#endif // DEFORMABLE_METHOD
    {}

    ~CContour()
    {}

    void   setDart( CDart* ADart )
    { FDart = ADart; }

    CDart* getDart() const
    { return FDart; }

    /// Return the energy saved with this contour.
    /// This is not necessarilly the current energy of the contour if
    /// there are some modifications since the last save.
    double getEnergy() const
    {
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_LINEL
      return static_cast<double>(FNbLinels);
#elif DEFORMABLE_METHOD==CONTOUR_ENERGY_MLP  || DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
      return FLengthMLP;
#else
      return 0.0;
#endif // DEFORMABLE_METHOD
    }

  protected:
    /// Un brin du contour
    CDart* FDart;

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_LINEL
  protected:
    /// Le nombre de lignels du contour actuel
    unsigned int FNbLinels;

  public:
    unsigned int getNbLinels() const
    { return FNbLinels; }

    void setNbLinels(unsigned int AValue)
    { FNbLinels=AValue; }

    void incNbLinels( int ANb=1 )
    {
      assert(ANb>0 || FNbLinels>=(unsigned int)-ANb);
      FNbLinels+= ANb;
    }
#endif // DEFORMABLE_METHOD

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_MLP  || DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
  protected:
    double FLengthMLP;
  public:
    double getLengthMLP() const
    { return FLengthMLP; }

    void setLengthMLP( double AValue )
    {
      assert( AValue>=0 );
      FLengthMLP = AValue;
    }

#endif // DEFORMABLE_METHOD

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
  protected:
    ImaGene::GridCurve* FDynamicMLP;
  public:
    ImaGene::GridCurve* getDMLP() const
    { return FDynamicMLP; }
    void setDMLP(ImaGene::GridCurve* ADMlp)
    { FDynamicMLP=ADMlp; }
    bool FExternalContour;
#endif // DEFORMABLE_METHOD
  };

} // namespace Map2d
//******************************************************************************
#endif // CONTOUR_HH
//******************************************************************************
