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
#include "inline-macro.hh"
#include "topological-map.hh"
#include "region.hh"
#include INCLUDE_NON_INLINE("tm-deformation.icc")
//******************************************************************************
using namespace Map2d;
using namespace std;
//******************************************************************************
void CTopologicalMap::deformationApply( CDart * ADart, const CDoublet & ALinel )
{
  assert( ADart != NULL );
  assert( deformationIsSimple( ALinel ) );

  CDart * dEdge = ADart;
  CDart * dEdge2 = beta2( ADart );

  CRegion * r1 = getRegion( dEdge );
  CRegion * r2 = getRegion( dEdge2 );

  // Mise à jour des informations associées aux régions
  r1->removePixel( getPixel( getPixelIn( ALinel ) ) );
  r2->addPixel( getPixel( getPixelIn( ALinel ) ) );

  // Mise à jour du plongement
  CDoublet tCur( ALinel );

  int jump = 0;

  CDoublet t1( tCur );
  t1.setPrevLinel();
  while( !isPCell( t1 ) && isLCell( t1 ) )
    {
      ++jump;
      t1.setNextPointel();
      t1.setPrevLinel();
    }
  t1.setNextLinel();
  // t1 est le premier lignel de clf( ALinel )

  CDoublet t2( tCur );
  do
    {
      ++jump;
      t2.setNextPointel();
      t2.setNextLinel();
    }
  while( !isPCell( t2 ) && isLCell( t2 ) );
  t2.setPrevLinel();
  // t2 est le dernier lignel de clf( ALinel ) et désigne le pointel de bord

  tCur = t1;
  // Suppression du chemin de lignel
  for( int i = 0 ; i < jump ; ++i )
    {
      FKhalimsky->setLCell( tCur, false );
      // Lignel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }

  // si le brin appartient à une boucle, il faut faire attention à
  // déplacer le brin convenablement.
  // autre solution, déplacer le brin sauvagement
  if ( ADart == beta1(ADart) )
    {
      t1.setPrevLinel();
      setDoublet( ADart, t1 );
      t1.setNextLinel();
      do t1.setNextLinel();
      while( !isLCell( t1 ) );
      setDoublet( beta2(ADart), t1 );
    }
  else
    {
      // si t1 est un pointel alors getDoublet( dEdge ) == t1
      if( isPCell( t1 ) )
	{
	  assert( getDoublet( dEdge ) == t1 );
	  t1.setPrevLinel();
	  // Tant que t1 est un lignel alors on décale le pointel
	  while( isLCell( t1 ) )
	    {
	      FKhalimsky->setPCell( t1, false );
	      t1.setNextPointel();
	      t1.setPrevLinel();
	      FKhalimsky->setPCell( t1, true );
	    }
	  // Nouveau plongement
	  setDoublet( dEdge, t1 );
	  // Mise à jour du plongement des brins de l'orbite sommet
	  CDart * dFirst = dEdge;
	  dEdge = beta02( dEdge );
	  while( dFirst != dEdge )
	    {
	      do t1.setPrevLinel();
	      while( !isLCell( t1 ) );
	      setDoublet( dEdge, t1 );
	      dEdge = beta02( dEdge );
	    }
	}
      // si t2 est un pointel alors getDoublet( dEdge2 ) == t2
      if( isPCell( t2 ) )
	{
	  assert( getDoublet( dEdge2 ) == t2 );
	  t2.setNextLinel();
	  // Tant que t2 est un lignel alors on décale le pointel
	  while( isLCell( t2 ) )
	    {
	      FKhalimsky->setPCell( t2, false );
	      t2.setNextPointel();
	      t2.setNextLinel();
	      FKhalimsky->setPCell( t2, true );
	    }
	  // Nouveau plongement
	  setDoublet( dEdge2, t2 );
	  // Mise à jour du plongement des brins de l'orbite sommet
	  CDart * dFirst = dEdge2;
	  dEdge2 = beta02( dEdge2 );
	  while( dFirst != dEdge2 )
	    {
	      do t2.setPrevLinel();
	      while( !isLCell( t2 ) );
	      setDoublet( dEdge2, t2 );
	      dEdge2 = beta02( dEdge2 );
	    }
	}
    }
  // On peut à présent remplir les lignels vides
  // Remplissage des lignels vides
  for( int i = jump ; i < 4 ; ++i )
    {
      if( !isLCell( tCur ) )
	FKhalimsky->setLCell( tCur, true );
      // Lignel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }
}
//******************************************************************************
bool CTopologicalMap::deformationIsSimple( const CDoublet & ALinel ) const
{
  uint32_t deg = 0;
  return deformationCheckPointelDegree( ALinel, &deg )
    && deformationCheckLinelBody( ALinel )
    && deformationCheckPointelDegreeExt( ALinel )
    && ( deg < 3 || deformationCheckPointelDegreeOther( ALinel ) );
}
//******************************************************************************
bool CTopologicalMap::deformationCheckPointelDegree( const CDoublet & ALinel,
						     uint32_t * AMaxDegree )
  const
{
  uint32_t deg = 0;
  CDoublet tCur( ALinel );
  for( int i = 0 ; i < 4 ; ++i )
    {
      deg = getPointelDegree( tCur );
      if( deg == 1 || deg == 4 )
	return false;
      if( deg > *AMaxDegree )
	*AMaxDegree = deg;
      // Pointel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }
  return true;
}
//******************************************************************************
bool CTopologicalMap::deformationCheckLinelBody( const CDoublet & ALinel )
  const
{
  int linels = 0;

  CDoublet tCur( ALinel );
  for( int i = 0 ; i < 4 ; ++i )
    {
      if( isLCell( tCur ) )
	{ ++linels; }
      // Lignel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }

  // Cas 0 et 4 : toujours faux
  if( linels == 0 || linels == 4 )
    return false;

  // Cas 2 : vrai si le lignel et son lignel mirroir (par rapport au pixel) sont
  // dans des états différents.
  if( linels == 2 )
    {
      tCur.setNextPointel();
      tCur.setNextLinel();
      tCur.setNextPointel();
      tCur.setNextLinel();
      return ( isLCell( ALinel ) != isLCell( tCur ) );
    }

  // Cas 1 et 3 : toujours vrai
  return true;
}
//******************************************************************************
bool CTopologicalMap::deformationCheckPointelDegreeExt( const CDoublet &
							ALinel ) const
{
  CDoublet tCur( ALinel );
  for( int i = 0 ; i < 4 ; ++i )
    {
      // On ne teste qu'un des lignels extérieur
      if(    !isLCell( tCur )
	  && !isLCell( tCur.getPrevLinel() )
	  && isLCell( tCur.getNextLinel() ) )
	return false;
      // Pointel suivant autour du pixel
      tCur.setNextPointel();
      tCur.setNextLinel();
    }
  return true;
}
//******************************************************************************
bool CTopologicalMap::deformationCheckPointelDegreeOther( const CDoublet &
							  ALinel ) const
{
  CDoublet tCur( ALinel );

  CDoublet t1( tCur );
  t1.setPrevLinel();
  while( !isPCell( t1 ) && isLCell( t1 ) )
    {
      t1.setNextPointel();
      t1.setPrevLinel();
    }
  t1.setNextLinel();
  // t1 est le premier lignel de clf( ALinel )

  CDoublet t2( tCur );
  t2.setNextPointel();
  while( !isPCell( t2 ) && isLCell( t2 ) )
    {
      t2.setNextLinel();
      t2.setNextPointel();
    }
  t2.setNextLinel();
  // t2 est le lignel suivant le dernier de clf( ALinel )

  for( int i = 0 ; i < 2 ; ++i )
    {
      t2.setNextPointel();
      t2.setNextLinel();
      if( t2 == t1 )
	return true;
      else if( getPointelDegree( t2 ) > 2 )
	return false;
    }

  return true;
}
//******************************************************************************
