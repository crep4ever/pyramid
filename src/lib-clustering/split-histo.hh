//******************************************************************************
#ifndef SPLIT_HISTO_HH
#define SPLIT_HISTO_HH
//******************************************************************************
#include "criterion-split.hh"
//******************************************************************************
namespace clustering
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
    int m_depth;

  public:
    // Constructeurs
    CSplitHisto(); //par défaut
    CSplitHisto(CRegion* ARegion, uint ADepth);

    // Destructeur
    ~CSplitHisto();

    /// Détermine si @param ARegion doit être éclatée ou non
    bool isRegionToSplit();
  };
} // namespace clustering
//******************************************************************************
#include INCLUDE_INLINE("split-histo.icc")
//******************************************************************************
#endif // SPLIT_HISTO_HH
//******************************************************************************
