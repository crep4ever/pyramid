//******************************************************************************
#ifndef SPLIT_HISTO_HH
#define SPLIT_HISTO_HH
//******************************************************************************
#include "criterion-split.hh"
//******************************************************************************
namespace Map2d
{
  //class CSplitGrey;
  /** 
   * Détermine si l'on doit éclater une région d'après les seuils de merge
   * utilisés ensuite pour la fusion:
   * si gris max - gris min < seuil de merge , on n'éclate pas la région 
   */

  class CRegion;
  class CSplitHisto : public CCriterionSplit
  {
    int FDepth;

  public:
    // Constructeurs
    CSplitHisto(); //par défaut
    CSplitHisto(CRegion* ARegion, uint ADepth);
  
    // Destructeur
    ~CSplitHisto();
  
    /// Détermine si @param ARegion doit être éclatée ou non
    bool isRegionToSplit();
  };
} // namespace Map2d
//******************************************************************************
#include INCLUDE_INLINE("split-histo.icc")
//******************************************************************************
#endif // SPLIT_HISTO_HH
//******************************************************************************
