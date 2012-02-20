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
#ifndef DEFORMABLE_PARTITION_HH
#define DEFORMABLE_PARTITION_HH
//******************************************************************************
#include "topological-map.hh"
#include "topological-dart.hh"
#include "priority-queue.hh"
#include "doublet.hh"
#include "contour.hh"
#include "edge.hh"

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_MLP || DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
#include "ImaGene/dgeometry2d/FreemanChain.h"
#endif

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
#include "ImaGene/dgeometry2d/GridCurve.h"
#endif

//******************************************************************************
typedef CPriorityQueue< Map2d::CRegion*, double > TPartitionQueue;
//******************************************************************************
class CDeformablePartition
  {
  public:
    /** Default contructor.
     * @param AMap the topological map to deform.
     * @param ARegionW the weight of the region energy
     * @param AContourW the weight of the contour energy
     */
    CDeformablePartition(Map2d::CTopologicalMap* AMap, double ARegionW=0.1,
                         double AContourW=250/*,double AEdgeW=300*/);
    
    /** Destructor
     */
    ~CDeformablePartition();

    /** Minimization of the partition energy by taking each time the best move.
     *
     * Energy minimization process by using a priority queue of best move
     * for each region.
     */
    void partitionMinimizeEnergyBestGain();

    /** Minimization of the partition energy by taking each time
     *  the first positive gain.
     *
     * Energy minimization process by using a priority queue of best move
     * for each region.
     *
     * @return true if there are still some deformations todo after AStop
     *         iterators, false otherwise.
     */
    bool partitionMinimizeEnergyFirstGain(unsigned int AStop=500);

    /// Return the global energy of the whole partition.
    double getEnergy() const;

  protected:
    /// @return the edge associated to the given dart.
    Map2d::CEdge* getEdge(Map2d::CDart* ADart) const;

    /// set the edge associated to the fiven dart.
    void setEdge(Map2d::CDart* ADart, Map2d::CEdge* AEdge);
    
    /// @return the contour associated to the given dart.
    Map2d::CContour* getContour(Map2d::CDart* ADart) const;

    /// set the contour associated to the fiven dart.
    void setContour(Map2d::CDart* ADart, Map2d::CContour* AContour);
    
    /** Accesseur sur l'énergie de la région donnée
     * @param ARegion la région
     */
    double getRegionEnergy( Map2d::CRegion * ARegion ) const;

    /** Accesseur sur l'énergie de la région incidente au brin
     * @param ADart le brin
     */
    double getRegionEnergy( Map2d::CDart * ADart ) const;
    
    /** Evaluate the energy associated to a region.
     * @param ARegion the region
     * @return energy associated with the region
     */
    double computeRegionEnergy( Map2d::CRegion * ARegion ) const;

    /** Evaluate the region energy associated to a dart.
     * @param ADart the dart
     * @return energy associated with the region of the dart
     */
    double computeRegionEnergy( Map2d::CDart * ADart ) const;

    /** Accesseur sur l'énergie du contour incident au brin
     * @param ADart le brin
     */
    double getContourEnergy( Map2d::CDart * ADart ) const;
    
    /** Evaluate the contour energy associated to a dart.
     * 
     * @param ADart the dart
     * @return energy associated with the contour incident to the dart
     */
    double computeContourEnergy( Map2d::CDart * ADart ) const;

    /** Accesseur sur l'énergie de l'arête incidente au brin
     * @param ADart le brin
     */
    double getEdgeEnergy( Map2d::CDart * ADart ) const;
    
    /** Accesseur sur l'énergie sauvée de l'arête incidente au brin
     * @param ADart le brin
     */
    double getSavedEdgeEnergy( Map2d::CDart * ADart ) const;
    
    /** Evaluate the edge energy associated to a dart.
     * 
     * @param ADart the dart
     * @return energy associated with the edge incident to the dart
     */
    double computeEdgeEnergy( Map2d::CDart * ADart ) const;

    /** Compute the global energy of the whole map.
     *  if ATrace is true draw the energy of each element (for debug)
     *  @return the energy.
     */
    double computeGlobalEnergy(bool ATrace=false) const;    

    /** Compute the energy gain for the given move.
     *
     * @param ADart dart incident to the edge
     * @param ADoublet a doublet
     * @param AIte un iterateur sur le DMLP correspondant à ADoublet
     *        uniquement dans le cas DMLP.
     * @return the energy gain of the move
     */
    double computeEnergyGain(Map2d::CDart * ADart,
                             const CDoublet & ADoublet
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
                             , ImaGene::GridCurve::Visitor & AIte
                             , ImaGene::GridCurve::Visitor & AIte2
#endif
                             );

    /** Apply the best deformation of the given region.
     *  Update the best move of each modified region.
     *
     * @param APq the priority queue of region's best moves
     * @param ARegion the region containing the best move
     */
    void applyBestDeformation( TPartitionQueue & APq,
			       Map2d::CRegion * ARegion );    

    /** Apply the first positive deformation of the given region.
     *  Update the first positive move of each modified region.
     *
     * @param ARegion the region containing the move
     */
    void applyFirstDeformation( Map2d::CRegion * ARegion );    

#if DEFORMABLE_METHOD==CONTOUR_ENERGY_MLP || DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
    /** Calcule la chaîne de Freeman associée au contour incident au brin
     *
     * @param ADart un brin
     *
     * @return code de Freeman associé au contour (orbite beta1)
     */
    ImaGene::FreemanChain freemanChainFromFace( Map2d::CDart * ADart ) const;
#endif
    
  protected:
    /// Initialize the contour energy of the contour linked with the dart.
    /// @param the dart
    double initializeContourEnergy( Map2d::CDart * ADart );

    /// Add the given linel in the energy of the edge associated with ADart.
    void addLinelInEdge( Map2d::CDart * ADart, const CDoublet & ALinel );

    /// Remove the given linel in the energy of the edge associated with ADart.
    void removeLinelInEdge( Map2d::CDart * ADart, const CDoublet & ALinel );

    /// Initialize the edge energy of the edge linked with the dart.
    /// @param the dart
    double initializeEdgeEnergy( Map2d::CDart * ADart );

    /// Initialize the energy of the whole partition.
    /// @return the global energy
    double partitionInitializeEnergy();

    /// Put in the queue the best move for each region.
    /// The direct index FIndex is used to keep an iterator on the queue.
    void putBestDeformationsInQueue(TPartitionQueue& AQueue);

    /// Delete the contours linked with darts, possibly the iterators
    /// linked with the direct info FIndex.
    void deleteAllocatedElements();

    /// Update the best move for the given region.
    /// Tst each possible flip, and keep in the region the best one.
    /// @return the gain of the best move.
    double updateMaximumGainForRegion( Map2d::CRegion* ARegion );
    
    /// Update the first positive move for the given region.
    /// Test each possible flip, and keep in the region the first positive.
    /// @return the gain of the first positive move.
    double updateFirstGainForRegion( Map2d::CRegion* ARegion );

    /// Test if the current deformation of ARegion is already valid
    /// ie it is a simple deformation with positive gain, and it is not
    /// between ARegion and ARegion2 or ARegion3, the two modified regions.
    bool firstDeformationValid( Map2d::CRegion* ARegion,
                                Map2d::CRegion* ARegion2,
                                Map2d::CRegion* ARegion3 );

    /// @return the first region having a positive deformation.
    /// NULL if no such region exists.
    Map2d::CRegion* findFirstPositiveDeformation();

    /** Bascule le pixel et modifie le plongement de l'arête déformée
     *
     * @param ADart brin de l'arête déformée
     * @param ALinel doublet désignant un pixel
     * @param ARevert bool indiquant s'il faut stocker le flip inverse (true)
     *
     * @pre le pixel désigné est IM-Simple
     *
     * @return le delta d'énergie due à ce flip
     */
    double deformationApply( Map2d::CDart * ADart,  const CDoublet & ALinel, 
#if DEFORMABLE_METHOD==CONTOUR_ENERGY_DMLP
                             ImaGene::GridCurve::Visitor& AIte,
                             ImaGene::GridCurve::Visitor& AIte2,
#endif
                             bool ARevert );

    /** Défait le dernier flip de pixel.
     * @pre le dernier deformationApply a été appelé avec ARevert==true.
     */
    void deformationRevert();

    /// Compute the weight length of the MLP associated to the given contour.
    double computeMLPLengthGradient(Map2d::CDart * ADart) const;

  public:
    double getOmegaRegion() const
    { return FOmegaRegion; }
    double getOmegaContour() const
    { return FOmegaContour; }

  private:
    Map2d::CTopologicalMap* FMap; ///< The topological map.

    double FOmegaRegion;  ///< the weight of the region energy
    double FOmegaEdge;    ///< the weight of the edge energy
    double FOmegaContour; ///< the weight of the contour energy
    double FEnergy;       ///< The energy of the current map
    int    FIndex;        ///< The direct info used (-1 if no use of DI)

    Map2d::CDart*   FRevertDart;
    CDoublet FRevertDoublet;
    Map2d::CRegion* FCurrentRegion;
};
//******************************************************************************
#include INCLUDE_INLINE("deformable-partition.icc")
//******************************************************************************
#endif // DEFORMABLE_PARTITION_HH
//******************************************************************************
