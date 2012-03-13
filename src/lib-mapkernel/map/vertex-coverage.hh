/*
 * lib-mapkernel : Un noyau de 2-cartes et des opérations.
 * Copyright (C) Moka Team, moka@sic.univ-poitiers.fr
 *               http://www.sic.univ-poitiers.fr/moka/

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
#ifndef VERTEX_COVERAGE_HH
#define VERTEX_COVERAGE_HH
//******************************************************************************
#include <cassert>
#include "coverage.hh"
//******************************************************************************
/** @author Alexandre Dupas, Romain Goffe
 */

namespace Map2d
{
  class CDart;
  class CMapBasic;


  //****************************************************************************
  /** Classe CBasicDynamicCoverageVertex : classe pour le parcours sommet
   *   qui ne gère pas les marques (pas de démarquage des brins marqués)
   */
  class CBasicDynamicCoverageVertex : public CCoverage
  {
  private:
    /// Brin de départ du parcours
    CDart* FFirstDart;

    /// Brin courant
    CDart* FCurrentDart;

    /// Mémoire du type d'opération dernièrement réalisée
    TOperationState FPrevOp;

    bool FDoContinue;

  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CBasicDynamicCoverageVertex.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     *  @param AMarkNumber le numéro de marque à utiliser pour ce parcours.
     */
    CBasicDynamicCoverageVertex(CMapBasic* AMap, const CDart* ADart, int a=0, int b=0);

    /// Destructeur de la classe CBasicDynamicCoverageVertex
    virtual ~CBasicDynamicCoverageVertex();

    //@}

    /// @name Opérations de parcours
    //@{

    /** Réinitialisation du parcours à son état original.
     *  Attention, il y aura certaimenent des problèmes de marque.
     *  Il vaut peut-être mieux utiliser CDynamicCoverageVertex.
     */
    virtual void reinit();

    /// Passage au brin suivant du parcours.
    virtual void operator++();

    /** Passage au brin suivant postfix. Réalise le déplacement du curseur
     *  et retourne le brin avant l'incrémentation
     */
    virtual CDart* operator++(int);

    /** Indication de fin de parcours.
     *  @return vrai si et seulement si il est encore possible
     *          de continuer le parcours.
     */
    virtual bool cont();

    //@}

    /// @name Accesseurs
    //@{

    /** Récupération du brin courant
     *  @return le brin courant.
     */
    virtual CDart* operator*();

    /** Dernière opération.
     *  @return le type de la dernière opération effectuée par le
     *          dernier appel à l'operator ++
     */
    virtual TOperationState prevOperationType();

    /** Type de parcours.
     *  @return un identifiant de parcours correspondant à la classe.
     */
    virtual int type() const;

    //@}
  };

  typedef CBasicDynamicCoverageVertex CDynamicCoverageVertex;

#define GET_BETAIJ(D,AI,AJ) ( FMap->beta(FMap->beta(D,AI),AJ) )


//******************************************************************************
// CBasicDynamicCoverageVertex
//******************************************************************************
inline
CBasicDynamicCoverageVertex::CBasicDynamicCoverageVertex(CMapBasic* AMap,
							 const CDart* ADart1, int, int) :
  CCoverage     (AMap),
  FFirstDart    ((CDart*) ADart1),
  FCurrentDart	( FFirstDart ),
  FPrevOp       (OP_NONE),
  FDoContinue   (true)
{
  assert( ADart1!=NULL );
}
//******************************************************************************
inline
CBasicDynamicCoverageVertex::~CBasicDynamicCoverageVertex()
{}
//******************************************************************************
inline
void CBasicDynamicCoverageVertex::reinit()
{
  FDoContinue	 = true;
  FCurrentDart	 = FFirstDart;
  FPrevOp	 = OP_NONE;
}
//******************************************************************************
inline
bool CBasicDynamicCoverageVertex::cont()
{ return FDoContinue; }
//******************************************************************************
inline
void CBasicDynamicCoverageVertex::operator ++()
{
  assert(cont());

  FPrevOp = OP_BETA02;
  FCurrentDart = GET_BETAIJ(FCurrentDart,0,2);

  if(FCurrentDart == FFirstDart)
    {
      FDoContinue = false;
      FPrevOp = OP_END;
    }
}
//******************************************************************************
inline
CDart* CBasicDynamicCoverageVertex::operator*()
{
  assert(cont());
  return FCurrentDart;
}
//******************************************************************************
inline
CDart* CBasicDynamicCoverageVertex::operator++(int)
{
  CDart* tmp = operator*();

  operator++();
  return tmp;
}
//******************************************************************************
inline
int CBasicDynamicCoverageVertex::type() const
{ return BASIC_COVERAGE_VERTEX; }
//******************************************************************************
inline
TOperationState CBasicDynamicCoverageVertex::prevOperationType()
{ return FPrevOp; }

//******************************************************************************
} // namespace Map2d
//******************************************************************************
#endif // VERTEX_COVERAGE_HH
//******************************************************************************
