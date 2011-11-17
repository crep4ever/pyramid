/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
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
#ifndef PYRAMIDAL_REGION_HH
#define PYRAMIDAL_REGION_HH
//******************************************************************************
#include "region.hh"
#include "point2d.hh"
//******************************************************************************

namespace Map2d
{
  /** 
   * \file pyramidal-region.hh
   * \class CPyramidalRegion "pyramidal-region.hh"
   * \author Romain Goffe
   * \brief CPyramidalRegion is a region within a pyramid with 
   *  up/down relationships
   */

  class CCriterionSplit;
  class CCriterionMerge;

  class CPyramidalRegion: public CRegion{

  public:
    enum Label {Background, Tissue, DarkTissue, BrightTissue, Lesion, Cancer, Stroma, Mitosis, InSitu, Invalid};
    
  protected:
    CPyramidalRegion* FRegionUp; // pointeur sur la région ascendante
    CPyramidalRegion* FRegionDown; // pointeur sur la région descendante dont le brin représentant est l'image down du représentant de sa région up
    
    int FLabel;
    
    /// Coordonnées du premier pixel de la région (haut/gauche)
    CPoint2D  FFirstPixel;

  public:
    CPyramidalRegion();                                // Constructeur par défaut
    CPyramidalRegion(const CRegion& ARegion);          // Constructeur à partir d'une région
    CPyramidalRegion(const CPyramidalRegion& ARegion); // Constructeur par recopie: copie des paramètres de la région, on ne copie pas les relations father/brother/firstson ...
    virtual ~CPyramidalRegion(); // Destructeur

    virtual CDart* getRepresentativeDart() const;
    virtual void setRepresentativeDart(CDart* ADart);
    
    // Accesseurs
    CPyramidalRegion* getRegionUp()   const;
    CPyramidalRegion* regionUp()   const;
    CPyramidalRegion* getRegionDown() const;
    CPyramidalRegion* regionDown() const;
	
    void setRegionUp    (CPyramidalRegion* ARegion);
    void setRegionDown  (CPyramidalRegion* ARegion);

    int label() const;
    void setLabel(const Label & ALabel);
    
    CPyramidalRegion* getBrother()    const;
    CPyramidalRegion* getFather()     const;
    CPyramidalRegion* getFirstSon()   const;
    CPyramidalRegion* getNextSameCC() const;
  
    TRegionId         getGreyMin()   const;
    void setGreyMin    (TRegionId AColorMin);

    TRegionId         getGreyMax()   const;
    void setGreyMax    (TRegionId AColorMax);  

    TRegionId getGreySum() const;
    void setGreySum (TRegionId AGrey);

    unsigned int getGreySquareSum() const; 
    void setGreySquareSum (unsigned int ASum);

    unsigned int getNbPixels() const;
    void setNbPixels(unsigned int ANbPix);

    CPoint2D getFirstPixel() const;
    CPoint2D firstPixel() const;
    void setFirstPixel(const CPoint2D& APixel);

    CPoint2D lastPixel() const;
    void setLastPixel(const CPoint2D& APixel);

    //******************************************************************************	  
    //Opérations sur la région
    //******************************************************************************    
    virtual CRegion* addSon( TRegionId );
    virtual CRegion* addBrother( TRegionId );
    virtual CRegion* addSameCC( TRegionId );
    virtual void     addPixels(CRegion* ARegion);
    virtual void addPixel( TRegionId ACoul, CDart* ADart=NULL );
    void addGrey(TRegionId AGrey);
    TRegionId getAverageGrey() const;  
    
    void mergeWith(CPyramidalRegion* ARegion);

    //******************************************************************************	  
    //Opérations de contrôle et de vérification
    //******************************************************************************    

    /// Teste l'existence d'une région up
    /// @return true si la région a une up
    bool existRegionUp();

    /// Teste l'existence d'une région down
    /// @return true si la région a une down
    bool existRegionDown();

    /// Affiche les caractéristiques d'une région
    void print();

    /// Vérifie des contraintes sur la région
    /// @return true si la région est valide
    bool isOk();
	
  };

  /// La classe région infinie. La seule différence concerne la méthode
  /// isInfiniteRegion qui va désormais retourner vrai.
  class CInfinitePyramidalRegion : public CPyramidalRegion{
  
  public:
    CInfinitePyramidalRegion();
    virtual ~CInfinitePyramidalRegion();

    bool isInfiniteRegion() const;
  };

  //******************************************************************************
  //******************************************************************************

  class CLessRegionFirstPixel : public std::binary_function<CPyramidalRegion*, CPyramidalRegion*, bool>
  {
    public :
      bool operator()( CPyramidalRegion* region1, CPyramidalRegion* region2 ) 
        {  
          return region1->getFirstPixel() < region2->getFirstPixel(); 
        }
   }; 

} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("pyramidal-region.icc")
//******************************************************************************
#endif // PYRAMIDAL_REGION_HH
//******************************************************************************
