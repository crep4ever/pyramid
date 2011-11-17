//*****************************************************************************
#ifndef COVERAGE_REGION_PIXELS_HH
#define COVERAGE_REGION_PIXELS_HH
//*****************************************************************************
#include "topological-map.hh"
#include "point2d.hh"
#include "region.hh"
#include <stack>
using namespace std;
//*****************************************************************************
namespace Map2d
{
  /**
   * Itérateur des pixels d'une région.
   * @author Alexandre Dupas
   */
  class CCoverageRegionPixels
  {
  public:
    //@name Constructeur et destructeur.

    //@{
    /**
     * Constructeur.
     * @param ATopologicalMap Référence aux données topologiques de la carte.
     * @param ARegion Référence à la région que l'on veut parcourir.
     */
    CCoverageRegionPixels(CTopologicalMap* ATopologicalMap, CRegion* ARegion);

    /**
     * Destructeur.
     */
    virtual ~CCoverageRegionPixels();

    //@}

    /**
     * Réinitialisation du parcours.
     */
    void reinit();

    /**
     * Itérateur préfixe. Passe au pixel suivant.
     */
    void operator++();

    /**
     * Itérateur postfixe.
     * @return Le pixel suivant.
     */
    const CPoint2D & operator++(int);

    /**
     * Drapeau d'arrêt du parcours.
     * @return <code>true</code> si il reste des pixels qui n'ont pas été
     * parcourus, <code>false</code> sinon.
     */
    bool cont();

    /**
     * Déréférencement de l'iterateur en pixel.
     * @return Le pixel actuel.
     */
    const CPoint2D & operator*();

  protected:
    /// Pointeur sur les données topologique.
    CTopologicalMap *      FTopologicalMap;
    /// Pointeur sur la région parcourue.
    CRegion *              FRegion;
    /// Pile des pixels que l'on doit traiter et parcourir.
    stack<CPoint2D>     FPixelsStack;
    /// Pixel en cours.
    CPoint2D            FCurrentPixel;  
    /// Flag de fin de parcours.
    bool                   FStop;
  };

} // namespace Map2d
//*****************************************************************************
#endif // COVERAGE_REGION_PIXELS_HH
//*****************************************************************************
