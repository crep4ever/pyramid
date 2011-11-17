/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/
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
#ifndef REGION_HH
#define REGION_HH
//******************************************************************************
#include "inline-macro.hh"
#include <bitset>
#include <list>
#include <cstdlib>
#include <stdint.h>
#include "kernel-types.hh"

#ifdef CONTOUR_SIMPLIFICATION
#include "topological-dart.hh"
#include "moments2D.hh"
#endif // CONTOUR_SIMPLIFICATION
//******************************************************************************
typedef uint64_t TSumColor;

namespace Map2d
{
  class CDart;
  class CTopologicalMap;

  #define NB_MARKS_REGION (16)

  /// Une région, c'est :
  ///   - un identifiant et des pointeurs sur père, frère et premier fils.
  ///   - un brin représentant.
  /// Ce représentant est forcément sur le bord externe de la région
  /// (à l'exeption de la région infinie qui n'a pas de bord externe).
  /// On a également des marques utiles lors de parcours.
  class CRegion
  {
    friend class CTopologicalMap;
    
  public:

    CRegion();
    CRegion( TRegionId, CRegion* AFather=NULL );
    CRegion( TRegionId, CDart*, CRegion* AFather=NULL );
    virtual ~CRegion();
  
    CRegion*     getFather    () const;
    CRegion*     getBrother   () const;
    CRegion*     getFirstSon  () const;
    CRegion*     getNextSameCC() const;
    TRegionId    getId        () const;
    unsigned int getNbPixels  () const;
    
    bool     existBrother   () const;
    bool     existFather    () const;
    bool     existSon       () const;
    bool     existNextSameCC() const;
  
    bool     firstRegionCC() const;
    
    CRegion* addBrother( TRegionId );
    void     addBrother( CRegion* );
    
    CRegion* addSon( TRegionId );
    void     addSon( CRegion*  );

    CRegion* addSameCC( TRegionId );
    void     addSameCC( CRegion*  );

    virtual bool isInfiniteRegion() const;
  
    CRegion* find( CRegion* );
    CRegion* find( TRegionId );
    bool     exist( TRegionId );

    void incNbPixels( unsigned int ANb=1 );
    void decNbPixels( unsigned int ANb=1 );
    
    // Ajoute le pixel de couleur ACoul et éventuellement représenté par la 
    // face carrée désignée par ADart à la région courante.
    virtual void addPixel( TRegionId ACoul, CDart* ADart=NULL );

    void removePixel( TRegionId ACoul );
    
    // Affiche l'arbre d'inclusion sur stdout   
    void draw();

    // Remets les informations couleur de la région à zéro
    void resetColorData();

    /// Méthodes pour utiliser les marques.
    bool getMark(int AMarkNumber) const;
    void setMark(int AMarkNumber, bool AValue);

    void   setRepresentativeDart( CDart* );
    CDart* getRepresentativeDart() const;
    
    ///@name Accesseurs sur la couleur
    //@{    
    TRegionId getColorMean() const; 
    TRegionId getColorMin()  const;
    TRegionId getColorMax()  const;             
   //@}
    
    ///@name Accesseurs sur la profondeur
    //@{    
    unsigned int getProfondeur () const;
    void         setProfondeur (unsigned int);
    //@}
    
    /// @return la "distance" entre la région this et ARegion
    ///         (c'est la distance selon le critère de fusion)
    TRegionId distanceWith(CRegion* ARegion) const;
             
    /// @return vrai ssi la région this et ARegion satisfont le 
    /// critère de fusion pour le seul AThreshold donné  
    bool canMergeWith(CRegion* ARegion, int AThreshold) const;
    
    /// @return vrai ssi la région this ou ARegion sont petites
    /// i.e. de taille <= à AThreshold.
    bool smallRegion(CRegion* ARegion, int AThreshold) const;
    
    /// Fusionne les paramètres de la région this avec la région ARegion.
    /// (ie met à jour tout les champs de this pour intégrer les autres champs)   
    void mergeWith(CRegion* ARegion);

    /// Nombre de pixels de la région (@see getNbPixels)
    uint32_t getPixelCount() const;

    /// Somme des couleurs de la région
    TSumColor getColorSum() const;

    /// Somme des couleurs au carré de la région
    TSumColor getSquaredColorSum() const;

    /// Variance de la région
    double variance() const;

    /// Erreur quadratique moyenne de la région
    double mse() const;

  public: //RG protected->public
    void delBrother   ( CRegion* );

    void delSon       ( CRegion* );

    void delNextSameCC( CRegion* );

    /// Modificateurs
    void setFather    ( CRegion*  );
    void setBrother   ( CRegion*  );
    void setFirstSon  ( CRegion*  );
    void setNextSameCC( CRegion*  );
    void setId        ( TRegionId );

  public: //RG: protected -> public
    /// Identifiant de la région
    TRegionId FId;
    
    /// Les pointeurs sur les voisins
    CRegion* FFather;     // Le père
    CRegion* FFirstSon;   // Le premier fils
    CRegion* FBrother;    // Le frère
    CRegion* FNextSameCC; // La liste des régions dans la même CC

    /// Le brin représentant
    CDart* FRepresentativeDart;

    /// Le nombre de pixel de la région
    unsigned int FNbPixels;
    
    /// Valeur des marques booléennes du brin
    std::bitset<NB_MARKS_REGION> FMarks;

    /// Profondeur dans l'arbre d'inclusion
    unsigned int FProfondeur;
    
    /// Somme des couleurs des pixels de la region
    TSumColor FColorSum;

    /// Somme des carrés des couleurs des pixels de la region
    TSumColor FSquaredColorSum;

    /// Couleur min de la région
    TRegionId FColorMin;
       
    /// Couleur max de la région   
    TRegionId FColorMax;
    
#ifdef CONTOUR_SIMPLIFICATION
public:   
    CMoments2D FMoments;
#endif // CONTOUR_SIMPLIFICATION   
  };

  /// Un itérateur pour parcourir les fils "représentants" d'une région,
  /// cad un fils par composante connexe incluse.
  class CDynamicCoverageCCSons
  {
  public:  
    CDynamicCoverageCCSons( CRegion* ARegion );
    ~CDynamicCoverageCCSons();
    
    CRegion* operator ++ ();
    CRegion* operator ++ ( int );
    
    bool cont() const;
    
    CRegion* operator -> () const;
    CRegion* operator *  () const;
    
  protected:
    CRegion* FRegionActu;
  };
  
  /// Un itérateur pour parcourir tout les fils d'une région.
  class CDynamicCoverageSons : public CDynamicCoverageCCSons
  {
  public:  
    CDynamicCoverageSons( CRegion* ARegion );
    ~CDynamicCoverageSons();
    
    CRegion* operator ++ ();
    CRegion* operator ++ ( int );
    
    bool cont() const;
    
    CRegion* operator -> () const;
    CRegion* operator *  () const;
    
  protected:
    CRegion* FNextCCActu;
  };
  
  
  /// Un itérateur pour parcourir les première régions de chaque composante
  /// connexe.
  class CDynamicCoverageCCRegions
  { 
  public:  
    CDynamicCoverageCCRegions( CRegion* ARegion );
    ~CDynamicCoverageCCRegions();
  
    CRegion* operator ++ ();
    CRegion* operator ++ ( int );

    bool cont() const;
    
    CRegion* operator -> () const;
    CRegion* operator *  () const;
    
  protected:
    std::list<CRegion*> FRegionsToVisit;
    CRegion*            FRegionActu;
  };

  /// Un itérateur pour parcourir les régions de l'arbre ARegion.
  class CDynamicCoverageRegions : public CDynamicCoverageCCRegions
  { 
  public:  
    CDynamicCoverageRegions( CRegion* ARegion );
    ~CDynamicCoverageRegions();
  
    CRegion* operator ++ ();
    CRegion* operator ++ ( int );

    bool cont() const;
    
    CRegion* operator -> () const;
    CRegion* operator *  () const;
    
  protected:
    CRegion* FNextCCActu;
  };

  /// Un itérateur pour parcourir les première régions de chaque composante
  /// connexe de la carte AMap.
  class CDynamicCoverageAllCCRegions : public CDynamicCoverageCCRegions
  {
  public:
    CDynamicCoverageAllCCRegions( CTopologicalMap* AMap );
    ~CDynamicCoverageAllCCRegions();
  };

  /// Un itérateur pour parcourir toute les régions de la carte AMap.
  class CDynamicCoverageAllRegions : public CDynamicCoverageRegions
  {
  public:
    CDynamicCoverageAllRegions( CTopologicalMap* AMap );
    ~CDynamicCoverageAllRegions();
  };

  /// La classe région infinie. La seule différence concerne la méthode
  /// isInfiniteRegion qui va désormais retourner vrai.
  class CInfiniteRegion : public CRegion
  {
  public:
    CInfiniteRegion();
    virtual ~CInfiniteRegion();
    
    virtual bool isInfiniteRegion() const;
  };
  
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("region.icc")
//******************************************************************************
#endif // REGION_HH
//******************************************************************************
