//******************************************************************************
#ifndef MOMENTS2D
#define MOMENTS2D
//******************************************************************************
#ifdef TBB_ACTIVE
#include "tbb/spin_mutex.h"
#endif // TBB_ACTIVE
//******************************************************************************
namespace Map2d
{
class CRegion;
class CDart;
class CTopologicalMap;
}
//******************************************************************************
#define MOMENT_ORDERS 2
//******************************************************************************
class CMoments2D
{
public:
   CMoments2D(Map2d::CRegion *ARegion);

    ~CMoments2D();

    void setMap(Map2d::CTopologicalMap* AMap);    
    
    /// Ajoute le pixel représenté par laface carrée désignée par ADart
    void addPixel(Map2d::CDart *ADart);
       
   /// Test if a vertex can be removed and update the internal moments
   /// APercentage is the percentage of acceptable modification
   /// @return true iff the removal is possible and in this case update
   /// the parameters. Otherwise return false
    bool vertexRemoval(Map2d::CDart* ADart, unsigned int APercentage);

    CMoments2D& operator += (const CMoments2D& AMoment);   

#ifdef TBB_ACTIVE
public:   
   /// Un mutex pour protéger l'arête des accès concurents.
   tbb::spin_mutex FMutex;
#endif // TBB_ACTIVE

private:
   double getEdgeMoment0_0(Map2d::CDart* ADart);
   double getEdgeMoment1_0(Map2d::CDart* ADart);
   double getEdgeMoment0_1(Map2d::CDart* ADart);
   double getEdgeMoment1_1(Map2d::CDart* ADart);
   double getEdgeMoment2_0(Map2d::CDart* ADart);
   double getEdgeMoment0_2(Map2d::CDart* ADart);
    
  /// Calcule les moments des régions associées à ADart et à
  /// beta2(ADart) si l'on supprime le sommet incident à ADart.
  /// Ces moments sont calculés et affectés à AMomentReg1 et
  /// AMomentReg2
  void updateMomentsForVertexRemoval ( Map2d::CDart* ADart,
				       CMoments2D& AMoments2,
				       double AMomentReg1
				       [MOMENT_ORDERS+1][MOMENT_ORDERS+1],
				       double AMomentReg2
				       [MOMENT_ORDERS+1][MOMENT_ORDERS+1] );

private:
  /// La carte d'appartenance
  Map2d::CTopologicalMap* FMap;
  
  /// La région à laquelle ce moment correspond
  Map2d::CRegion* FRegion;    
  
  /// La matrice 2D contenant les moments de la région courante.
  double FMoments[MOMENT_ORDERS+1][MOMENT_ORDERS+1];

  /// La matrice 2D contenant les moments originaux, contant durant la
  /// simplification des contours.
  double FInitialMoments[MOMENT_ORDERS+1][MOMENT_ORDERS+1];
};
//******************************************************************************
#endif // MOMENTS2D
//******************************************************************************
