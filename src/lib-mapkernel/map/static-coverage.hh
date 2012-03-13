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
#ifndef STATIC_COVERAGE_HH
#define STATIC_COVERAGE_HH
//******************************************************************************
#include <list>

#include "coverage.hh"
#include "dynamic-coverage.hh"
#include "inline-macro.hh"
//******************************************************************************
/** @author Guillaume Damiand, Frédéric Vidil
 */

namespace Map2d
{
  class CDart;
  class CMapBasic;

  //****************************************************************************
  /** Classe CStaticCoverage2 : classe de base pour les parcours
   *  correspondant à une orbite spécifiée par une seule involution beta2
   *  (mais pas pour beta0 ni beta1 qui sont des cas particulier).
   *
   *  Cette classe contient une structure de données permettant de parcourir
   *  tous les brins de l'orbite (à partir d'un brin de départ) associée à une
   *  seule involution. Cela implique donc seulement 2 brins.
   */
  class CStaticCoverage2 : public CCoverage
  {
  private:
    /** Brin de départ
     *  Indique le premier brin du parcours
     */
    CDart* FDart1;

    /** Brin 2
     *  Indique le brin 2 du parcours
     */
    CDart* FDart2;

    /** Booléen indiquant si le parcours est fini ou non.
     */
    bool FDoContinue;

    /** Booléen indiquant si on est sur le premier brin.
     *  Si oui le prochain ++ va faire beta2, sinon le prochain ++
     *  va "sortir" du parcours, il sera alors fini.
     */
    bool FDart1Active;

  public:
    /** @name Constructeurs et destructeur
     */

    //@{

    /** Constructeur
     *  Pour construire un objet de type CStaticCoverage2.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     */
    CStaticCoverage2(CMapBasic* AMap, const CDart* ADart);

    /** Destructeur de la classe CStaticCoverage2
     */
    virtual ~CStaticCoverage2();

    //@}

    /** @name Opérations de parcours
     */

    //@{

    /** Réinitialisation du parcours à son état original.
     */
    virtual void reinit();

    /** Passage au brin suivant du parcours.
     */
    virtual void operator++();

    /** Passage au brin suivant postfix.
     *  Réalise le déplacement du curseur et retourne le brin avant l'incrémentation
     */
    virtual CDart* operator++(int);

    /** Indication de fin de parcours.
     *  @return vrai si et seulement si il est encore possible de continuer le
     *          parcours.
     */
    virtual bool cont();

    //@}

    /** @name Accesseurs
     */

    //@{

    /** Récupération du brin courant
     *  @return le brin courant.
     */
    virtual CDart* operator*();

    /** Type de parcours.
     *  @return un identifiant de parcours correspondant à la classe.
     */
    virtual int type() const;

    //@}
  };
  //****************************************************************************
  class CGenericStaticCoverage : public CCoverage
  {
  protected:
    std::list<CDart*> FAllDarts;
    std::list<CDart*>::iterator FIt;

  public:
    /// @name Constructeurs et destructeur
    //@{

    /// Constructeur
    CGenericStaticCoverage(CMapBasic* AMap);

    /// Destructeur
    virtual ~CGenericStaticCoverage();
    //@}

    /// @name Opérateurs de parcours
    //@{

    /// La fonction reinit (re)positionne le pointeur de parcours
    /// sur le premier brin (FDart1)
    void reinit();

    /// Cet opérateur permet de déplacer le curseur de parcours sur le brin
    /// suivant (et met à jour si besoin le booléen de fin de parcours)
    void operator++();

    /// Opérateur postfix
    /// Réalise le déplacement du curseur après avoir retourné le brin courant
    CDart* operator++(int);

    /** Indication de fin de parcours.
     *  Cette méthode indique si le parcours est achevé ou non. Il n'y a aucun
     *  paramètre d'entrée.
     *
     *  @return vrai si et seulement si il est encore possible de continuer le
     *  parcours.
     */
    bool cont();

    /// Cet opérateur retourne un pointeur sur le brin courant.
    CDart* operator*();

    //@}
    };
  //****************************************************************************
  class CStaticCoverage0 : public CGenericStaticCoverage
  {
  public:
    /// @name Constructeurs et destructeur
    //@{

    /// Constructeur
    CStaticCoverage0(CMapBasic* AMap, const CDart* ADart1);

    /// Destructeur
    virtual ~CStaticCoverage0();
    //@}

    /// cette fonction retourne le type du parcours.
    int type() const;
  };
  //****************************************************************************
  class CStaticCoverage1 : public CGenericStaticCoverage
  {
  public:
    /// @name Constructeurs et destructeur
    //@{

    /// Constructeur
    CStaticCoverage1(CMapBasic* AMap, const CDart* ADart1);

    /// Destructeur
    virtual ~CStaticCoverage1();
    //@}

    /// cette fonction retourne le type du parcours.
    int type() const;
  };
  //****************************************************************************
  class CStaticCoverage12 : public CGenericStaticCoverage
  {
  public:
    /// @name Constructeurs et destructeur
    //@{

    /// Constructeur :
    CStaticCoverage12(CMapBasic* AMap, const CDart* ADart1);

    /// Destructeur :
    virtual ~CStaticCoverage12();
    //@}

    /// cette fonction retourne le type du parcours.
    int type() const;
  };
  //****************************************************************************
   class CStaticCoverageVertex : public CGenericStaticCoverage
  {
  public:
    /// @name Constructeurs et destructeur
    //@{

    /// Constructeur :
    CStaticCoverageVertex(CMapBasic* AMap, const CDart* ADart1);

    /// Destructeur :
    virtual ~CStaticCoverageVertex();
    //@}

    /// cette fonction retourne le type du parcours.
    int type() const;
  };
   //****************************************************************************
  class CStaticCoverageAll : public CGenericStaticCoverage
  {
  public:
    /// @name Constructeurs et destructeur
    //@{

    /// Constructeur :
    CStaticCoverageAll(CMapBasic* AMap);

    /// Destructeur :
    virtual ~CStaticCoverageAll();
    //@}

    /// cette fonction retourne le type du parcours.
    int type() const;
  };
  //****************************************************************************
  /** Tous les parcours possibles dans la carte: instanciation des templates
   *     et différents noms pour les mêmes parcours
   *
   */
  typedef CDynamicCoverageSelf        CStaticCoverageSelf;

   /// Nouveaux noms pour les cellules
  typedef CStaticCoverage1           CStaticCoverageEdge;
  typedef CStaticCoverage2           CStaticCoverageFace;
  typedef CStaticCoverage12          CStaticCoverageCC;

} // namespace Map2d

//******************************************************************************
//******************************************************************************
//******************************************************************************
//- INLINE CODE
//******************************************************************************
//******************************************************************************
//******************************************************************************

//******************************************************************************
#include "map-basic.hh"
#include "dart.hh"
#include "dynamic-coverage.hh"
#include <cassert>
//******************************************************************************
namespace Map2d
{
//******************************************************************************
// CStaticCoverage2
//******************************************************************************
inline
void CStaticCoverage2::reinit()
{
  FDoContinue  = true;
  FDart1Active = true;
}
//******************************************************************************
inline
CStaticCoverage2::CStaticCoverage2(CMapBasic* AMap, const CDart* ADart1) :
  CCoverage(AMap),
  FDart1   ((CDart*) ADart1)
{
  if (AMap->isFree2(FDart1))
    FDart2 = NULL;
  else
    FDart2 = AMap->beta2(FDart1);

  reinit();
}
//******************************************************************************
inline
CStaticCoverage2::~CStaticCoverage2()
{}
//******************************************************************************
inline
bool CStaticCoverage2::cont()
{
  return FDoContinue;
}
//******************************************************************************
inline
void CStaticCoverage2::operator++()
{
  assert(cont());

  if (! FDart1Active || FMap->isFree2(FDart1))
    FDoContinue = false;
  else
    FDart1Active = false;
}
//******************************************************************************
inline
CDart* CStaticCoverage2::operator*()
{
  assert(cont());

  if (FDart1Active)
    return FDart1;
  else
    return FDart2;
}
//******************************************************************************
inline
CDart* CStaticCoverage2::operator++(int)
{
  CDart* tmp = operator*();

  operator++();
  return tmp;
}
//******************************************************************************
inline
int CStaticCoverage2::type() const
{
  return COVERAGE1;
}
//******************************************************************************
// CGenericStaticCoverage
//******************************************************************************
inline
void CGenericStaticCoverage::reinit()
{
  FIt = FAllDarts.begin();
}
//******************************************************************************
inline
CGenericStaticCoverage::CGenericStaticCoverage(CMapBasic* AMap) :
  CCoverage(AMap)
{
}
//******************************************************************************
inline
CGenericStaticCoverage::~CGenericStaticCoverage()
{}
//******************************************************************************
inline
bool CGenericStaticCoverage::cont()
{
  return FIt != FAllDarts.end();
}
//******************************************************************************
inline
void CGenericStaticCoverage::operator++()
{
  assert(cont());

  ++ FIt;
}
//******************************************************************************
inline
CDart* CGenericStaticCoverage::operator*()
{
  assert(cont());

  return * FIt;
}
//******************************************************************************
inline
CDart* CGenericStaticCoverage::operator++(int)
{
  CDart* tmp = operator *();
  operator ++();
  return tmp;
}
//******************************************************************************
// CStaticCoverage0
//******************************************************************************
inline
CStaticCoverage0::CStaticCoverage0(CMapBasic* AMap, const CDart* ADart1) :
  CGenericStaticCoverage(AMap)
{
  CDynamicCoverage0 it(AMap, ADart1);

  while (it.cont())
    FAllDarts.push_back(it++);

  reinit();
}
//******************************************************************************
inline
CStaticCoverage0::~CStaticCoverage0()
{}
//******************************************************************************
inline
int CStaticCoverage0::type() const
{
  return COVERAGE_BETA0;
}
//******************************************************************************
// CStaticCoverage1
//******************************************************************************
inline
CStaticCoverage1::CStaticCoverage1(CMapBasic* AMap, const CDart* ADart1) :
  CGenericStaticCoverage(AMap)
{
  CDynamicCoverage1 it(AMap, ADart1);

  while (it.cont())
    FAllDarts.push_back(it++);

  reinit();
}
//******************************************************************************
inline
CStaticCoverage1::~CStaticCoverage1()
{}
//******************************************************************************
inline
int CStaticCoverage1::type() const
{
  return COVERAGE_BETA1;
}
//******************************************************************************
// CStaticCoverage12
//******************************************************************************
inline
CStaticCoverage12::CStaticCoverage12(CMapBasic* AMap, const CDart* ADart1) :
  CGenericStaticCoverage(AMap)
{
  CDynamicCoverage12 it(AMap, ADart1);

  while (it.cont())
    FAllDarts.push_back(it++);

  reinit();
}
//******************************************************************************
inline
CStaticCoverage12::~CStaticCoverage12()
{}
//******************************************************************************
inline
int CStaticCoverage12::type() const
{
  return COVERAGE_BETA12;
}
//******************************************************************************
// CStaticCoverageVertex
//******************************************************************************
inline
CStaticCoverageVertex::CStaticCoverageVertex(CMapBasic* AMap,
					     const CDart* ADart1) :
  CGenericStaticCoverage(AMap)
{
  CDynamicCoverageVertex it(AMap, ADart1);

  while (it.cont())
    FAllDarts.push_back(it++);

  reinit();
}
//******************************************************************************
inline
CStaticCoverageVertex::~CStaticCoverageVertex()
{}
//******************************************************************************
inline
int CStaticCoverageVertex::type() const
{
  return COVERAGE_VERTEX;
}
//******************************************************************************
// CStaticCoverageAll
//******************************************************************************
inline
CStaticCoverageAll::CStaticCoverageAll(CMapBasic* AMap) :
  CGenericStaticCoverage(AMap)
{
  CDynamicCoverageAll it(AMap);

  while (it.cont())
    FAllDarts.push_back(it++);

  reinit();
}
//******************************************************************************
inline
CStaticCoverageAll::~CStaticCoverageAll()
{
}
//******************************************************************************
inline
int CStaticCoverageAll::type() const
{
  return COVERAGE_ALL;
}
//******************************************************************************
} // namespace Map2d
//******************************************************************************
#endif // STATIC_COVERAGE_HH
//******************************************************************************
