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
#ifndef DYNAMIC_COVERAGE_HH
#define DYNAMIC_COVERAGE_HH
//******************************************************************************
#include <list>
#include <iostream>
#include <queue>
#include <stack>

#include "coverage.hh"
#include "inline-macro.hh"
//******************************************************************************
/** @author Guillaume Damiand, Frédéric Vidil
 */

namespace Map2d
{
  class CDart;
  class CMapBasic;

  /** Classe CDynamicCoverageSelf : classe pour le parcours
   *  correspondant à l'orbite "self" c'est à dire le brin lui-même.
   */
  class CDynamicCoverageSelf : public CCoverage
  {
  private:
    /// Brin de départ (et le seul parcouru).
    CDart* FSelf;

    /// Booléen indiquant si le parcours est fini ou non.
    bool FDoContinue;

  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CDynamicCoverageSelf.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     */
    CDynamicCoverageSelf(CMapBasic* AMap, const CDart* ADart);

    /// Destructeur de la classe CDynamicCoverage
    virtual ~CDynamicCoverageSelf();

    //@}

    /// @name Opérations de parcours
    //@{

    /// Réinitialisation du parcours à son état original.
    virtual void reinit();

    /// Passage au brin suivant du parcours.
    virtual void operator++();

    /** Passage au brin suivant postfix. Réalise le déplacement du curseur 
     *  et retourne le brin avant l'incrémentation
     */
    virtual CDart* operator++(int);

    /** Indication de fin de parcours.
     *  @return vrai si et seulement si il est encore possible de 
     *  continuer le parcours.
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

  //****************************************************************************
  /** Classe CDynamicCoverage2 : classe de base pour les parcours
   *  correspondant à une orbite spécifiée par une seule involution beta2
   *  (mais pas pour beta1 qui est un cas particulier).
   *  
   *  Cette classe contient une structure de données permettant de parcourir
   *  tous les brins de l'orbite (à partir d'un brin de départ) associée à une
   *  seule involution. Cela implique donc seulement 2 brins.
   */
  class CDynamicCoverage2 : public CCoverage
  {
  private:
    /// Brin de départ du parcours.
    CDart* FDart1;
    
    /// Booléen indiquant si le parcours est fini ou non.
    bool FDoContinue;

    /** Booléen indiquant si on est sur le premier brin.
     *  Si oui le prochain ++ va faire beta2, sinon le prochain ++
     *  va "sortir" du parcours, il sera alors fini.
     */
    bool FDart1Active;

  public:	
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CDynamicCoverage2.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     */
    CDynamicCoverage2(CMapBasic* AMap, const CDart* ADart);

    /// Destructeur de la classe CDynamicCoverage2
    virtual ~CDynamicCoverage2();

    //@}

    /// @name Opérations de parcours
    //@{

    /// Réinitialisation du parcours à son état original.
    virtual void reinit();

    /// Passage au brin suivant du parcours.
    virtual void operator++();

    /** Passage au brin suivant postfix. Réalise le déplacement du curseur 
     *  et retourne le brin avant l'incrémentation
     */
    virtual CDart* operator++(int);

    /** Indication de fin de parcours.
     *  @return vrai si et seulement si il est encore possible de 
     *  continuer le parcours.
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
  //****************************************************************************
  /// Classe CDynamicCoverage0 : classe pour le parcours beta0.
  class CDynamicCoverage0 : public CCoverage
  {
  private:
    /// Brin de départ du parcours et brin courant (curseur)
    CDart* FDart1, * FDartCurrent;

    /// Booléen indiquant si le parcours est fini ou non.
    bool FDoContinue;

    /// Booléen indiquant si on est dans la première direction
    bool FFirstDirection;

    /// Mémoire du type d'opération dernièrement réalisée
    TOperationState FPrevOp;

  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CDynamicCoverage0.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     */
    CDynamicCoverage0(CMapBasic* AMap, const CDart* ADart);

    /// Destructeur de la classe CDynamicCoverage0
    virtual ~CDynamicCoverage0();

    //@}

    /// @name Opérations de parcours
    //@{

    /// Réinitialisation du parcours à son état original.
    virtual void reinit();

    /// Passage au brin suivant du parcours.
    virtual void operator++();

    /** Passage au brin suivant postfix. Réalise le déplacement du curseur 
     *  et retourne le brin avant l'incrémentation
     */
    virtual CDart* operator++(int);

    /** Indication de fin de parcours.
     *  @return vrai si et seulement si il est encore possible de 
     *          continuer le parcours.
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
  //****************************************************************************
  /// Classe CDynamicCoverage1 : classe pour le parcours beta1.
  class CDynamicCoverage1 : public CCoverage
  {
  private:
    /// Brin de départ du parcours et brin courant (curseur)
    CDart* FDart1, * FDartCurrent;

    /// Booléen indiquant si le parcours est fini ou non.
    bool FDoContinue;

    /// Booléen indiquant si on est dans la première direction
    bool FFirstDirection;

    /// Mémoire du type d'opération dernièrement réalisée
    TOperationState FPrevOp;

  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CDynamicCoverage1.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     */
    CDynamicCoverage1(CMapBasic* AMap, const CDart* ADart);

    /// Destructeur de la classe CDynamicCoverage1
    virtual ~CDynamicCoverage1();

    //@}

    /// @name Opérations de parcours
    //@{

    /// Réinitialisation du parcours à son état original.
    virtual void reinit();

    /// Passage au brin suivant du parcours.
    virtual void operator++();

    /** Passage au brin suivant postfix. Réalise le déplacement du curseur 
     *  et retourne le brin avant l'incrémentation
     */
    virtual CDart* operator++(int);

    /** Indication de fin de parcours.
     *  @return vrai si et seulement si il est encore possible de 
     *          continuer le parcours.
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

  //****************************************************************************
  /** Classe CBasicDynamicCoverage12 : classe pour le parcours <beta1, beta2>,
   *   qui ne gère pas les marques (pas de démarquage des brins marqués)
   */
  class CBasicDynamicCoverage12 : public CCoverage
  {
  private:
    /// Brin de départ du parcours
    CDart* FDart1;

    /// Brin courant
    CDart* FDartCurrent;

    /// File d'attente des brins à traiter
    std::queue<CDart*>* FToTreat;

    /// Mémoire du type d'opération dernièrement réalisée
    TOperationState FPrevOp;

    /// On utilise le parcours <Beta1>
    CDynamicCoverage1* FIt;

    /// Vrai ssi c'est le premier parcours de cet itérateur. Devient faux après un reinit.
    bool FFirstCoverage;

  protected:
    /// Indices de la marque booléenne utilisée.
    int FMarkNumber;
    
    /// Numéro du direct info utilisé. Si -1, n'utilise pas de direct info
    /// et utilise alors la file FToTreat.
    int FDirectIndex;

  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CBasicDynamicCoverage12.
     *
     *  @param AMap         la carte à parcourir.
     *  @param ADart        le brin à partir duquel commence le parcours.
     *  @param AMarkNumber  le numéro de marque à utiliser pour ce parcours.
     *  @param ADirectIndex le numéro du direct info à utiliser pour ce parcours.
     */
    CBasicDynamicCoverage12(CMapBasic* AMap, const CDart* ADart,
			    int AMarkNumber,
			    int ADirectIndex = -1);

    /// Destructeur de la classe CBasicDynamicCoverage12
    virtual ~CBasicDynamicCoverage12();

    //@}

    /// @name Opérations de parcours
    //@{

    /** Réinitialisation du parcours à son état original.
     *  Attention, il y aura certaimenent des problèmes de marque.
     *  Il vaut peut-être mieux utiliser CDynamicCoverage12.
     */
    virtual void reinit();

    /// Passage au brin suivant du parcours.
    virtual void operator++();

    /** Passage au brin suivant postfix. Réalise le déplacement du curseur 
     *  et retourne le brin avant l'incrémentation
     */
    virtual CDart* operator++(int);

    /** Indication de fin de parcours.
     *  @return vrai si et seulement si il est encore possible de continuer le
     *          parcours.
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
  //****************************************************************************
  /// Classe CDynamicCoverage12 : classe pour le parcours <beta1, beta2> .
  class CDynamicCoverage12 : public CBasicDynamicCoverage12
  {
  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CDynamicCoverage12.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     */
    CDynamicCoverage12(CMapBasic* AMap, const CDart* ADart);

    /// Destructeur de la classe CDynamicCoverage12
    virtual ~CDynamicCoverage12();

    //@}

    /// @name Opérations de parcours
    //@{

    /// Réinitialisation du parcours à son état initial en effacant les marques.
    virtual void reinit();

    /** Type de parcours.
     *  @return un identifiant de parcours correspondant à la classe.
     */
    virtual int type() const;

    //@}
  };

  
  //****************************************************************************
  /** Classe CBasicDynamicCoverageVertex : classe pour le parcours sommet
   *   qui ne gère pas les marques (pas de démarquage des brins marqués)
   */
  class CBasicDynamicCoverageVertex : public CCoverage
  {
  private:
    /// Brin de départ du parcours
    CDart* FDart1;

    /// Brin courant
    CDart* FDartCurrent;

    /// File d'attente des brins à traiter
    std::queue<CDart*>* FToTreat;

    /// Mémoire du type d'opération dernièrement réalisée
    TOperationState FPrevOp;

    /// On utilise le parcours <Beta1>
    CDynamicCoverage1* FIt;

    /// Vrai ssi c'est le premier parcours de cet itérateur. Devient faux après un reinit.
    bool FFirstCoverage;

  protected:
    /// Indices de la marque booléenne utilisée.
    int FMarkNumber;
    
    /// Numéro du direct info utilisé. Si -1, n'utilise pas de direct info
    /// et utilise alors la file FToTreat.
    int FDirectIndex;
    
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
    CBasicDynamicCoverageVertex(CMapBasic* AMap, const CDart* ADart,
				int AMarkNumber, int ADirectIndex = -1);

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
  //****************************************************************************
  /// Classe CDynamicCoverageVertex : classe pour le sommet
  class CDynamicCoverageVertex : public CBasicDynamicCoverageVertex
  {
  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CDynamicCoverageVertex.
     *
     *  @param AMap  la carte à parcourir.
     *  @param ADart le brin à partir duquel commence le parcours.
     */
    CDynamicCoverageVertex(CMapBasic* AMap, const CDart* ADart);

    /// Destructeur de la classe CDynamicCoverageVertex
    virtual ~CDynamicCoverageVertex();

    //@}

    /// @name Opérations de parcours
    //@{

    /// Réinitialisation du parcours à son état initial en effacant les marques.
    virtual void reinit();

    /** Type de parcours.
     *  @return un identifiant de parcours correspondant à la classe.
     */
    virtual int type() const;

    //@}
  };

 
  //****************************************************************************
  /// Classe pour parcourir tous les brins de la carte
  class CDynamicCoverageAll : public CCoverage
  {
  private:
    /// Brin courant du parcours
    unsigned int Fi,Fj;
    CDart* FCurrent;

    /// Déplace Fi et Fj pour aller sur le prochain brin
    /// non marqué FMarkToDelete.
    /// FCurrent devient NULL si un tel brin n'existe pas.
    void goToNextDart();

  public:
    /// @name Constructeurs et destructeur
    //@{

    /** Constructeur
     *  Pour construire un objet de type CDynamicCoverageAll.
     *
     *  @param AMap  la carte à parcourir.
     */
    CDynamicCoverageAll(CMapBasic* AMap);

    /// Destructeur de la classe CDynamicCoverage1
    virtual ~CDynamicCoverageAll();

    //@}

    /// @name Opérations de parcours
    //@{
    
    /// Réinitialisation du parcours à son état original.
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

  //****************************************************************************
 
  /// Nouveaux noms pour les cellules
  typedef CDynamicCoverage1  CDynamicCoverageEdge;
  typedef CDynamicCoverage2  CDynamicCoverageFace;
  typedef CDynamicCoverage12 CDynamicCoverageCC;

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
#include <cassert>
//******************************************************************************
namespace Map2d
{
//******************************************************************************
//- MACRO pour les itérateurs basic effectuant un chaînage dans le directinfo
#define EXIST_BETAIJ(D,AI, AJ) \
  ( !FMap->isFree(D, AI) && !FMap->isFree(FMap->beta(D,AI), AJ) )

#define GET_BETAIJ(D,AI,AJ) ( FMap->beta(FMap->beta(D,AI),AJ) )
      
#define IS_UNMARKED(D,AI) \
  ( !FMap->isFree(D, AI) && !FMap->isMarked(FMap->beta(D,AI), FMarkNumber) )

#define IS_UNMARKED2(D,AI,AJ) \
  ( !FMap->isFree(D, AI) && !FMap->isFree(FMap->beta(D,AI), AJ) &&\
    !FMap->isMarked(GET_BETAIJ(D,AI,AJ), FMarkNumber) )

#define MARK_BETAI(D,AI) \
  ( FMap->setMark(FMap->beta(D,AI), FMarkNumber) )

#define MARK_BETAIJ(D,AI, AJ) \
  ( FMap->setMark(GET_BETAIJ(D,AI,AJ), FMarkNumber) )

#define GET_BETAI_AND_MARK(D,AI) \
  ( FMap->setMark(FMap->beta(D,AI), FMarkNumber), FMap->beta(D,AI) )

#define GET_BETAIJ_AND_MARK(D,AI,AJ) \
  ( FMap->setMark(GET_BETAIJ(D,AI,AJ), FMarkNumber), \
    GET_BETAIJ(D,AI,AJ) )

#define PUSH_AND_MARK(D,AI) \
  { \
   if ( FDirectIndex==-1 ) \
      FToTreat->push(GET_BETAI_AND_MARK(D,AI)); \
   else \
     { \
       CDart* firstInList = (CDart*) \
                             FMap->getDirectInfo(FDartCurrent, FDirectIndex); \
       FMap->setDirectInfo( GET_BETAI_AND_MARK(D,AI), \
			    FDirectIndex, firstInList); \
       FMap->setDirectInfo( FDartCurrent, FDirectIndex, FMap->beta(D,AI)); \
     } \
  }

#define PUSH_AND_MARK2(D,AI,AJ) \
  { \
    if ( FDirectIndex==-1 ) \
       FToTreat->push(GET_BETAIJ_AND_MARK(D,AI,AJ)); \
    else \
     { \
       CDart* firstInList = (CDart*) \
                             FMap->getDirectInfo(FDartCurrent, FDirectIndex); \
       FMap->setDirectInfo( GET_BETAIJ_AND_MARK(D,AI,AJ), \
                            FDirectIndex, firstInList); \
       FMap->setDirectInfo( FDartCurrent, FDirectIndex, GET_BETAIJ(D,AI,AJ)); \
     } \
  }
//******************************************************************************
// CDynamicCoverageSelf
//******************************************************************************
inline
void CDynamicCoverageSelf::reinit()
{ FDoContinue = true; }
//******************************************************************************
inline
int CDynamicCoverageSelf::type() const
{ return COVERAGE_SELF; }
//******************************************************************************
inline
CDynamicCoverageSelf::CDynamicCoverageSelf(CMapBasic* AMap,
					   const CDart* ADart1) :
  CCoverage(AMap),
  FSelf	   ((CDart*) ADart1)
{ reinit(); }
//******************************************************************************
inline
CDynamicCoverageSelf::~CDynamicCoverageSelf()
{}
//******************************************************************************
inline
bool CDynamicCoverageSelf::cont()
{ return FDoContinue; }
//******************************************************************************
inline
void CDynamicCoverageSelf::operator ++()
{
  assert(cont());
  FDoContinue = false;
}
//******************************************************************************
inline
CDart* CDynamicCoverageSelf::operator*()
{
  assert(cont());
  return FSelf;
}
//******************************************************************************
inline
CDart* CDynamicCoverageSelf::operator++(int)
{
  CDart* tmp = operator*();

  operator++();
  return tmp;
}
//******************************************************************************
inline
TOperationState CDynamicCoverageSelf::prevOperationType()
{ return FDoContinue ? OP_NONE : OP_END; }
//******************************************************************************
// CDynamicCoverage2
//******************************************************************************
inline
void CDynamicCoverage2::reinit()
{
  FDoContinue  = true;
  FDart1Active = true;
}
//******************************************************************************
inline
CDynamicCoverage2::CDynamicCoverage2(CMapBasic* AMap,const CDart* ADart1) :
  CCoverage(AMap),
  FDart1   ((CDart*) ADart1)
{ reinit(); }
//******************************************************************************
inline
CDynamicCoverage2::~CDynamicCoverage2()
{}
//******************************************************************************
inline
bool CDynamicCoverage2::cont()
{ return FDoContinue; }
//******************************************************************************
inline
void CDynamicCoverage2::operator++()
{
  assert(cont());

  if (! FDart1Active || FMap->isFree2(FDart1))
    FDoContinue = false;
  else
    FDart1Active = false;
}
//******************************************************************************
inline
CDart* CDynamicCoverage2::operator*()
{
  assert(cont());

  if (FDart1Active)
    return FDart1;
  else
    return FMap->beta2(FDart1);
}
//******************************************************************************
inline
CDart* CDynamicCoverage2::operator++(int)
{
  CDart* tmp = operator*();

  operator++();
  return tmp;
}
//******************************************************************************
inline
TOperationState CDynamicCoverage2::prevOperationType()
{
  if (FDoContinue)
    return FDart1Active ? OP_NONE : OP_BETAI;
  else
    return OP_END;
}
//******************************************************************************
inline
int CDynamicCoverage2::type() const
{ return COVERAGE1; }
//******************************************************************************
// CDynamicCoverage0
//******************************************************************************
inline
void CDynamicCoverage0::reinit()
{
  FDoContinue	  = true;
  FFirstDirection = true;
  FDartCurrent	  = FDart1;
  FPrevOp	  = OP_NONE;
}
//******************************************************************************
inline
int CDynamicCoverage0::type() const
{ return COVERAGE_BETA0; }
//******************************************************************************
inline
CDynamicCoverage0::CDynamicCoverage0(CMapBasic* AMap,
				     const CDart* ADart1) :
  CCoverage(AMap),
  FDart1   ((CDart*) ADart1)
{ reinit(); }
//******************************************************************************
inline
CDynamicCoverage0::~CDynamicCoverage0()
{}
//******************************************************************************
inline
bool CDynamicCoverage0::cont()
{ return FDoContinue; }
//******************************************************************************
inline
void CDynamicCoverage0::operator ++()
{
  assert(cont());

  if ( FFirstDirection && FMap->isFree0(FDartCurrent) )
    {
      FDartCurrent = FDart1;
      FFirstDirection = false;
      FPrevOp = OP_JUMP;
    }
  else
    {
      FPrevOp = OP_BETAI;
    }
  
  if ( FFirstDirection )
    {
      assert(!FMap->isFree0(FDartCurrent));
      FDartCurrent = FMap->beta0(FDartCurrent);

      if (FDartCurrent == FDart1)
	{
	  FDoContinue = false;
	  FPrevOp = OP_END;
	}
    }
  else
    {
      if ( FMap->isFree1(FDartCurrent) )
	{
	  FDoContinue = false;
	  FPrevOp = OP_END;
	}
      else
	{
	  FDartCurrent = FMap->beta1(FDartCurrent);
	}
    }
}
//******************************************************************************
inline
CDart* CDynamicCoverage0::operator*()
{
  assert(cont());
  return FDartCurrent;
}
//******************************************************************************
inline
CDart* CDynamicCoverage0::operator++(int)
{
  CDart* tmp = operator*();

  operator++();
  return tmp;
}
//******************************************************************************
inline
TOperationState CDynamicCoverage0::prevOperationType()
{ return FPrevOp; }
//******************************************************************************
// CDynamicCoverage1
//******************************************************************************
inline
void CDynamicCoverage1::reinit()
{
  FDoContinue	  = true;
  FFirstDirection = true;
  FDartCurrent	  = FDart1;
  FPrevOp	  = OP_NONE;
}
//******************************************************************************
inline
int CDynamicCoverage1::type() const
{ return COVERAGE_BETA1; }
//******************************************************************************
inline
CDynamicCoverage1::CDynamicCoverage1(CMapBasic* AMap,
				     const CDart* ADart1) :
  CCoverage(AMap),
  FDart1   ((CDart*) ADart1)
{ reinit(); }
//******************************************************************************
inline
CDynamicCoverage1::~CDynamicCoverage1()
{}
//******************************************************************************
inline
bool CDynamicCoverage1::cont()
{ return FDoContinue; }
//******************************************************************************
inline
void CDynamicCoverage1::operator ++()
{
  assert(cont());

  if ( FFirstDirection && FMap->isFree1(FDartCurrent) )
    {
      FDartCurrent = FDart1;
      FFirstDirection = false;
      FPrevOp = OP_JUMP;
    }
  else
    {
      FPrevOp = OP_BETAI;
    }
  
  if ( FFirstDirection )
    {
      assert(!FMap->isFree1(FDartCurrent));
      FDartCurrent = FMap->beta1(FDartCurrent);

      if (FDartCurrent == FDart1)
	{
	  FDoContinue = false;
	  FPrevOp = OP_END;
	}
    }
  else
    {
      if ( FMap->isFree0(FDartCurrent) )
	{
	  FDoContinue = false;
	  FPrevOp = OP_END;
	}
      else
	{
	  FDartCurrent = FMap->beta0(FDartCurrent);
	}
    }
}
//******************************************************************************
inline
CDart* CDynamicCoverage1::operator*()
{
  assert(cont());
  return FDartCurrent;
}
//******************************************************************************
inline
CDart* CDynamicCoverage1::operator++(int)
{
  CDart* tmp = operator*();

  operator++();
  return tmp;
}
//******************************************************************************
inline
TOperationState CDynamicCoverage1::prevOperationType()
{ return FPrevOp; }
//******************************************************************************
// CBasicDynamicCoverage12
//******************************************************************************
inline
CBasicDynamicCoverage12::CBasicDynamicCoverage12(CMapBasic* AMap,
						 const CDart* ADart1,
						 int AMarkNumber,
						 int ADirectIndex) :
  CCoverage     (AMap),
  FDart1        ((CDart*) ADart1),
  FDartCurrent	((CDart*) ADart1),
  FPrevOp       (OP_NONE),
  FFirstCoverage(true),
  FMarkNumber   (AMarkNumber),
  FDirectIndex  (ADirectIndex)
{
  assert( ADart1!=NULL );

  FMap->setMark(FDartCurrent, FMarkNumber);

  if ( FDirectIndex!=-1 )
    FMap->setDirectInfo(FDartCurrent, FDirectIndex, NULL);
  else
    FToTreat = new std::queue<CDart*>;
}
//******************************************************************************
inline
CBasicDynamicCoverage12::~CBasicDynamicCoverage12()
{
  if ( FDirectIndex==-1 )
    delete FToTreat;
}
//******************************************************************************
inline
void CBasicDynamicCoverage12::reinit()
{
  if ( FDirectIndex!=-1 )
    {
      if ( FDartCurrent!=NULL && FFirstCoverage )
	FMap->setDirectInfo(FDartCurrent, FDirectIndex, NULL);
    }
  else
    {
      delete FToTreat; FToTreat = new std::queue<CDart*>;
    }
  
  FDartCurrent	 = FDart1;
  FPrevOp	 = OP_NONE;
  FFirstCoverage = false;
  FMap->setMark(FDartCurrent, FMarkNumber);
}
//******************************************************************************
inline
bool CBasicDynamicCoverage12::cont()
{ return FDartCurrent != NULL; }
//******************************************************************************
inline
void CBasicDynamicCoverage12::operator ++()
{
  assert(cont());
  
  if ( FFirstCoverage || FDirectIndex==-1 )
    {      
      CDart* nd = NULL;

      if (IS_UNMARKED(FDartCurrent, 0))
	{
	  nd = GET_BETAI_AND_MARK(FDartCurrent, 0);
	  FPrevOp = OP_BETA0;

	  if (IS_UNMARKED(FDartCurrent, 1))
	    PUSH_AND_MARK(FDartCurrent, 1);

	  if (IS_UNMARKED(FDartCurrent, 2))
	    PUSH_AND_MARK(FDartCurrent, 2);
	}
      else if (IS_UNMARKED(FDartCurrent, 1))
	{
	  nd = GET_BETAI_AND_MARK(FDartCurrent, 1);
	  FPrevOp = OP_BETA1;

	  if (IS_UNMARKED(FDartCurrent, 2))
	    PUSH_AND_MARK(FDartCurrent, 2);
	}
      else  if (IS_UNMARKED(FDartCurrent, 2))
	{
	  nd = GET_BETAI_AND_MARK(FDartCurrent, 2);
	  FPrevOp = OP_BETA2;
	}
      else
	{
	  if ( FDirectIndex==-1 )
	    {
	      if ( !FToTreat->empty() )
		{
		  nd = FToTreat->front();
		  FToTreat->pop();
		  FPrevOp = OP_JUMP;
		}
	      else
		FPrevOp = OP_END;
	    }
	  else
	    {
	      // On récupère le premier brin de la liste.
	      nd = (CDart*) FMap->getDirectInfo(FDartCurrent,FDirectIndex);
	      // Et on l'enlève.
	      if ( nd!=NULL )
		{
		  FMap->setDirectInfo(FDartCurrent, FDirectIndex,
				      FMap->getDirectInfo(nd,FDirectIndex)); 
		  FPrevOp = OP_JUMP;
		}
	      else
		{
		  FMap->setDirectInfo(FDartCurrent, FDirectIndex, NULL);
		  FPrevOp = OP_END;
		}
	    }	  
	}

      if ( FDirectIndex!=-1 )
	{
	  // On récupère la liste des brins à traiter de FDartCurrent pour la
	  // reporter sur nd.
	  if ( nd!=NULL )
	    FMap->setDirectInfo(nd, FDirectIndex,
				FMap->getDirectInfo(FDartCurrent,FDirectIndex));
	  // Puis on fait le chainage entre les brins parcourus pour le
	  // prochain parcours.
	  FMap->setDirectInfo(FDartCurrent, FDirectIndex, nd);
	}
    
      FDartCurrent = nd;
    }
  else
    {
      // Pour avoir exactement le même effet que le parcours sans direct info,
      // on marque tout le voisinage du brin courant.
      if ( !FMap->isFree0(FDartCurrent) ) MARK_BETAI(FDartCurrent, 0);
      if ( !FMap->isFree1(FDartCurrent) ) MARK_BETAI(FDartCurrent, 1);
      if ( !FMap->isFree2(FDartCurrent) ) MARK_BETAI(FDartCurrent, 2);

      // Ici on utilise le direct info lors du 2ème (et suivant) parcours.
      FDartCurrent = (CDart*) FMap->getDirectInfo(FDartCurrent,FDirectIndex);
    }
}
//******************************************************************************
inline
CDart* CBasicDynamicCoverage12::operator*()
{
  assert(cont());
  return FDartCurrent;
}
//******************************************************************************
inline
CDart* CBasicDynamicCoverage12::operator++(int)
{
  CDart* tmp = operator*();
  operator++();
  return tmp;
}
//******************************************************************************
inline
TOperationState CBasicDynamicCoverage12::prevOperationType()
{ return FPrevOp; }
//******************************************************************************
inline
int CBasicDynamicCoverage12::type() const
{ return BASIC_COVERAGE_BETA12; }
//******************************************************************************
// CDynamicCoverage12
//******************************************************************************
inline
CDynamicCoverage12::CDynamicCoverage12(CMapBasic* AMap,
				       const CDart* ADart1) :
  CBasicDynamicCoverage12(AMap, ADart1,
			  AMap->getNewMark(),
			  AMap->getNewDirectInfo())
{
}
//******************************************************************************
inline
CDynamicCoverage12::~CDynamicCoverage12()
{
  // Si on refais exactement le même parcours, en ayant d'abord
  // inversé les marques, ça enlève les marques déjà posées.
  FMap->negateMaskMark(FMarkNumber);
  
  CBasicDynamicCoverage12::reinit();
  
  while ( cont() )
    operator++();
  
  FMap->negateMaskMark(FMarkNumber);
  
  if ( FDirectIndex!=-1 )
    FMap->freeDirectInfo(FDirectIndex);

  FMap->freeMark(FMarkNumber);
}
//******************************************************************************
inline
void CDynamicCoverage12::reinit()
{
  // Si on refais exactement le même parcours, en ayant d'abord
  // inversé les marques, ça enlève les marques déjà posées.
  FMap->negateMaskMark(FMarkNumber);
  
  CBasicDynamicCoverage12::reinit();
  
  while ( cont() )
    operator++();
  
  FMap->negateMaskMark(FMarkNumber);

  // Puis on se repositionne en début de liste.
  CBasicDynamicCoverage12::reinit();
}
//******************************************************************************
inline
int CDynamicCoverage12::type() const
{ return COVERAGE_BETA12; }

//******************************************************************************
// CBasicDynamicCoverageVertex
//******************************************************************************
inline
CBasicDynamicCoverageVertex::CBasicDynamicCoverageVertex(CMapBasic* AMap,
							 const CDart* ADart1,
							 int AMarkNumber,
							 int ADirectIndex) :
  CCoverage     (AMap),
  FDart1        ((CDart*) ADart1),
  FDartCurrent	((CDart*) ADart1),
  FPrevOp       (OP_NONE),
  FFirstCoverage(true),
  FMarkNumber   (AMarkNumber),
  FDirectIndex  (ADirectIndex)
{
  assert( ADart1!=NULL );

  FMap->setMark(FDartCurrent, FMarkNumber);

  if ( FDirectIndex!=-1 )
    FMap->setDirectInfo(FDartCurrent, FDirectIndex, NULL);
  else
      FToTreat = new std::queue<CDart*>;
}
//******************************************************************************
inline
CBasicDynamicCoverageVertex::~CBasicDynamicCoverageVertex()
{
  if ( FDirectIndex==-1 )
    delete FToTreat;
}
//******************************************************************************
inline
void CBasicDynamicCoverageVertex::reinit()
{
  if ( FDirectIndex!=-1 )
    {
      if ( FDartCurrent!=NULL && FFirstCoverage )
	FMap->setDirectInfo(FDartCurrent, FDirectIndex, NULL);
    }
  else
    {
      delete FToTreat; FToTreat = new std::queue<CDart*>;
    }
  
  FDartCurrent	 = FDart1;
  FPrevOp	 = OP_NONE;
  FFirstCoverage = false;
  FMap->setMark(FDartCurrent, FMarkNumber);
}
//******************************************************************************
inline
bool CBasicDynamicCoverageVertex::cont()
{ return FDartCurrent != NULL; }
//******************************************************************************
inline
void CBasicDynamicCoverageVertex::operator ++()
{
  assert(cont());

  if ( FFirstCoverage || FDirectIndex==-1 )
    {      
      CDart* nd = NULL;

      if ( IS_UNMARKED2(FDartCurrent, 0, 2) )
	{
	  nd = GET_BETAIJ_AND_MARK(FDartCurrent, 0, 2);
	  FPrevOp = OP_BETA02;
	}

      
      if ( IS_UNMARKED2(FDartCurrent, 2, 1) )
	{
	  if ( nd==NULL )
	    {
	      nd = GET_BETAIJ_AND_MARK(FDartCurrent, 2, 1);
	      FPrevOp = OP_BETA21;
	    }
	  else
	    PUSH_AND_MARK2(FDartCurrent, 2, 1);
	}

           
       if ( nd==NULL)
	{
	  if ( FDirectIndex==-1 )
	    {
	      if ( !FToTreat->empty() )
		{
		  nd = FToTreat->front();
		  FToTreat->pop();
		  FPrevOp = OP_JUMP;
		}
	      else
		FPrevOp = OP_END;
	    }
	  else
	    {
	      // On récupère le premier brin de la liste.
	      nd = (CDart*) FMap->getDirectInfo(FDartCurrent,FDirectIndex);
	      // Et on l'enlève.
	      if ( nd!=NULL )
		{
		  FMap->setDirectInfo(FDartCurrent, FDirectIndex,
				      FMap->getDirectInfo(nd,FDirectIndex)); 
		  FPrevOp = OP_JUMP;
		}
	      else
		{
		  FMap->setDirectInfo(FDartCurrent, FDirectIndex, NULL);
		  FPrevOp = OP_END;
		}
	    }
	}
      
      if ( FDirectIndex!=-1 )
	{
	  // On récupère la liste des brins à traiter de FDartCurrent pour la
	  // reporter sur nd.
	  if ( nd!=NULL )
	    FMap->setDirectInfo(nd, FDirectIndex,
				FMap->getDirectInfo(FDartCurrent,FDirectIndex));
	  // Puis on fait le chainage entre les brins parcourus pour le
	  // prochain parcours.
	  FMap->setDirectInfo(FDartCurrent, FDirectIndex, nd);
	}
      
      FDartCurrent = nd;
    }
  else
    {
      // Pour avoir exactement le même effet que le parcours sans direct info,
      // on marque tout le voisinage du brin courant.
      if (EXIST_BETAIJ(FDartCurrent, 0, 2)) MARK_BETAIJ(FDartCurrent, 0, 2);
     
      if (EXIST_BETAIJ(FDartCurrent, 2, 1)) MARK_BETAIJ(FDartCurrent, 2, 1);

      // Ici on utilise le direct info lors du 2ème (et suivant) parcours.
      FDartCurrent = (CDart*) FMap->getDirectInfo(FDartCurrent,FDirectIndex);
    }
}
//******************************************************************************
inline
CDart* CBasicDynamicCoverageVertex::operator*()
{
  assert(cont());
  return FDartCurrent;
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
// CDynamicCoverageVertex
//******************************************************************************
inline
CDynamicCoverageVertex::CDynamicCoverageVertex( CMapBasic* AMap,
						const CDart* ADart1 ) :
  CBasicDynamicCoverageVertex(AMap, ADart1,
			      AMap->getNewMark(),
			      AMap->getNewDirectInfo())
{
}
//******************************************************************************
inline
CDynamicCoverageVertex::~CDynamicCoverageVertex()
{
  // Si on refais exactement le même parcours, en ayant d'abord
  // inversé les marques, ça enlève les marques déjà posées.
  FMap->negateMaskMark(FMarkNumber);
  
  CBasicDynamicCoverageVertex::reinit();
  
  while ( cont() )
    operator++();
  
  FMap->negateMaskMark(FMarkNumber);
    
  if ( FDirectIndex!=-1 )
    FMap->freeDirectInfo(FDirectIndex);

  FMap->freeMark(FMarkNumber);
}
//******************************************************************************
inline
void CDynamicCoverageVertex::reinit()
{
  // Si on refais exactement le même parcours, en ayant d'abord
  // inversé les marques, ça enlève les marques déjà posées.
  FMap->negateMaskMark(FMarkNumber);
  
  CBasicDynamicCoverageVertex::reinit();
  
  while ( cont() )
    operator++();
  
  FMap->negateMaskMark(FMarkNumber);

  // Puis on se repositionne en début de parcours.
  CBasicDynamicCoverageVertex::reinit();
}
//******************************************************************************
inline
int CDynamicCoverageVertex::type() const
{ return COVERAGE_VERTEX; }

//******************************************************************************
// CDynamicCoverageAll
//******************************************************************************
inline
void CDynamicCoverageAll::goToNextDart()
{
  assert( FMap->getNbTabsDarts()>0 );
  
  do
    {
      ++Fj;
      if ( Fi==FMap->getNbTabsDarts()-1 && Fj==FMap->getNbDartsInLastTab() )
	FCurrent = NULL;
      else
	{
	  if ( Fj==FMap->getSizeDartArray() )
	    {
	      ++Fi; Fj=0;
	    }
	  
	  FCurrent = FMap->getDart(Fi,Fj);
	}
    }
  while ( FCurrent!=NULL &&
	  FMap->isMarked(FCurrent, FMap->getMarkToDelete()) );
}
//------------------------------------------------------------------------------
inline
void CDynamicCoverageAll::reinit()
{
  FCurrent = NULL;
  Fi = 0;
  Fj = -1;

  if ( FMap->getNbDarts()>0 )
    goToNextDart();
}
//******************************************************************************
inline
CDynamicCoverageAll::CDynamicCoverageAll(CMapBasic* AMap)
  : CCoverage(AMap)
{ reinit(); }
//******************************************************************************
inline
CDynamicCoverageAll::~CDynamicCoverageAll()
{}
//******************************************************************************
inline
bool CDynamicCoverageAll::cont()
{ return FCurrent!=NULL; }
//******************************************************************************
inline
void CDynamicCoverageAll::operator++()
{
  assert(cont());
  goToNextDart();
}
//******************************************************************************
inline
CDart* CDynamicCoverageAll::operator*()
{
  assert(cont());
  return FCurrent;
}//******************************************************************************
inline
CDart* CDynamicCoverageAll::operator++(int)
{
  CDart* tmp = operator*();
  operator++();
  return tmp;
}
//******************************************************************************
inline
TOperationState CDynamicCoverageAll::prevOperationType()
{ return OP_UNKNOWN; }
//******************************************************************************
inline
int CDynamicCoverageAll::type() const
{ return COVERAGE_ALL; }
//******************************************************************************
#undef EXIST_BETAIJ
#undef GET_BETAIJ
#undef IS_UNMARKED
#undef IS_UNMARKED2
#undef MARK_BETAI
#undef MARK_BETAIJ
#undef GET_BETAI_AND_MARK
#undef GET_BETAIJ_AND_MARK
#undef PUSH_AND_MARK
#undef PUSH_AND_MARK2
//******************************************************************************
} // namespace Map2d
//******************************************************************************
#endif // DYNAMIC_COVERAGE_HH
//******************************************************************************
