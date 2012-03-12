/***************************************************************************
 *   Copyright (C) 2007 by Guillaume Damiand and David Coeurjolly          *
 *   damiand@sic.univ-poitiers.fr david.coeurjolly@liris.cnrs.fr           *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//******************************************************************************
#include "dss.hh"
using namespace std;
//******************************************************************************
CDss::CDss() :
  FStartingPoint(0,0),
  FEndPoint     (0,0),
  FLowerLeft    (0,0),
  FLowerRight   (0,0),
  FUpperLeft    (0,0),
  FUpperRight   (0,0),
  FDir1         (0),
  FDir2         (0),
  Fa            (0),
  Fb            (0),
  Fc            (0),
  FLastDir      (0)
{}
//******************************************************************************
CDss::CDss(const Point<int> &APoint1,const  Point<int> &APoint2) :
  FStartingPoint(APoint1),
  FEndPoint     (APoint2),
  FLowerLeft    (APoint1),
  FLowerRight   (APoint2),
  FUpperLeft    (APoint1),
  FUpperRight   (APoint2),
  FDir1         (APoint1.getDirection(APoint2)),
  FDir2         (0),
  Fa            (APoint2.x-APoint1.x),
  Fb            (APoint2.y-APoint1.y),
  Fc            (Fa*FLowerRight.y-Fb*FLowerRight.x),
  FLastDir      (FDir1)
{
  assert( APoint1.is4Connected(APoint2) );
}
//******************************************************************************
CDss::CDss( const CDss &ADss ) :
  FStartingPoint(ADss.FStartingPoint),
  FEndPoint     (ADss.FEndPoint),
  FLowerLeft    (ADss.FLowerLeft),
  FLowerRight   (ADss.FLowerRight),
  FUpperLeft    (ADss.FUpperLeft),
  FUpperRight   (ADss.FUpperRight),
  FDir1         (ADss.FDir1),
  FDir2         (ADss.FDir2),
  Fa            (ADss.Fa),
  Fb            (ADss.Fb),
  Fc            (ADss.Fc),
  FLastDir      (ADss.FLastDir)

{}
//******************************************************************************
CDss &CDss::operator=(const CDss &ADss)
{
  FStartingPoint = ADss.FStartingPoint;
  FEndPoint	 = ADss.FEndPoint;
  FLowerLeft     = ADss.FLowerLeft;
  FLowerRight    = ADss.FLowerRight;
  FUpperLeft     = ADss.FUpperLeft;
  FUpperRight    = ADss.FUpperRight;
  FDir1      	 = ADss.FDir1;
  FDir2      	 = ADss.FDir2;
  Fa		 = ADss.Fa;
  Fb		 = ADss.Fb;
  Fc		 = ADss.Fc;
  FLastDir       = ADss.FLastDir;


  return *this;
}
//******************************************************************************
bool CDss::init(const Point<int> &APoint1, const  Point<int> &APoint2)
{
  if ( !APoint1.is4Connected(APoint2) ) return false;

  FStartingPoint = APoint1;
  FEndPoint      = APoint2;
  FLowerLeft     = APoint1;
  FLowerRight    = APoint2;
  FUpperLeft     = APoint1;
  FUpperRight    = APoint2;
  FDir1          = APoint1.getDirection(APoint2);
  FDir2          = 0;
  Fa             = APoint2.x-APoint1.x;
  Fb             = APoint2.y-APoint1.y;
  Fc             = Fa*FLowerRight.y-Fb*FLowerRight.x;
  FLastDir       = FDir1;

  return true;
}
//------------------------------------------------------------------------------
bool CDss::init(int Ax1, int Ay1, int Ax2, int Ay2)
{
  return init(Point<int>(Ax1,Ay1),Point<int>(Ax2,Ay2));
}
//******************************************************************************
void CDss::reverse()
{
  { // On échange le point de départ et le point d'arrivée.
    Point<int> tmp(FStartingPoint);
    FStartingPoint = FEndPoint;
    FEndPoint      = tmp;
  }

  {
    Point<int> tmp1(FLowerLeft);
    Point<int> tmp2(FLowerRight);

    FLowerLeft  = FUpperRight;
    FLowerRight = FUpperLeft;

    FUpperLeft  = tmp2;
    FUpperRight = tmp1;
  }

  Fa = -Fa;
  Fb = -Fb;
  Fc = Fa*FLowerRight.y-Fb*FLowerRight.x;

  if ( FDir2==0 )
    { // On n'a qu'une direction, il suffit de l'inverser.
      FDir1    = -FDir1;
      FLastDir = -FLastDir;
    }
  else
    { // On a deux directions, il faut les inverser, et tester
      // s'il faut les échanger en utilisant FLastDir;
      if ( FLastDir==FDir1 ) // on inverse sans echanger
	{
	  FDir1	   = -FDir1;
	  FDir2	   = -FDir2;
	  FLastDir = -FLastDir;
	}
      else // on inverse et on échange
	{
	  assert ( FLastDir==FDir2 );
	  int tmp  = FDir2;
	  FDir2	   = -FDir1;
	  FDir1	   = -tmp;
	  FLastDir = FDir2;
	}
    }
}
//******************************************************************************
bool CDss::addPointRight(const Point<int> &APoint)
{
  // On ne peux ajouter qu'un point 4 connexe avec le dernier point du dss.
  if ( !FEndPoint.is4Connected(APoint) )
    return false;

  int dir = FEndPoint.getDirection(APoint);

  // 1) On n'est pas encore dans un quadrant précis,
  //    i.e. on est horizontal ou vertical.
  //    Si la direction reste la même, on prolonge le DSS
  if ( FDir2==0 && abs(dir)==abs(FDir1) )
    {
      if ( dir==-FDir1 ) return false; // Retour en arrière impossible

      FLowerRight.shiftInDirection(FDir1);
      FUpperRight.shiftInDirection(FDir1);
      FEndPoint = APoint;
      return true;
    }

  // 2) Ici soit on est déjà dans un quadrant, i.e. on est en
  // diagonale, soit on vient juste de changer de direction.
  if ( FDir2==0 )
    { // On vient de changer de direction : ce point sera forcément accepté
      FDir2 = dir;
    }
  else
    { // Vérification de la compatibilité avec les 2 directions courantes
      if ( dir!=FDir1 && dir!=FDir2 ) return false;
    }

  // Test du cas courant et mis à jour des paramètres.
  int h = Fb*APoint.x - Fa*APoint.y + Fc;

  // Si on est dans la droite, il n'y a rien à mettre à jour
  if ( 0<h && h<abs(Fa)+abs(Fb)-1 )
    {}
  else if ( h==0 )
    {
      FLowerRight = APoint;
    }
  else if ( h==abs(Fa)+abs(Fb)-1 )
    {
      FUpperRight = APoint;
    }
  else if ( h==-1 )
    {
      FLowerRight = APoint;
      FUpperLeft = FUpperRight;

      Fa = APoint.x - FLowerLeft.x;
      Fb = APoint.y - FLowerLeft.y;
      Fc = Fa*FLowerRight.y - Fb*FLowerRight.x;
    }
  else if ( h==abs(Fa)+abs(Fb) )
    {
      assert( h==abs(Fa)+abs(Fb) );

      FUpperRight = APoint;
      FLowerLeft = FLowerRight;

      Fa = APoint.x - FUpperLeft.x;
      Fb = APoint.y - FUpperLeft.y;
      Fc = Fa*FLowerRight.y - Fb*FLowerRight.x;
    }
  else
    return false; // Le point est rejeté.

  FEndPoint = APoint; // On décale le dernier point
  FLastDir  = dir; // On met à jour la dernière direction utilisée

  return true;
}
//******************************************************************************
// @TODO  Debuguer : a priori ne marche pas dans tout les cas...
bool CDss::addPointLeft ( const Point<int> &APoint )
{
  // On ne peux ajouter qu'un point 4 connexe avec le dernier point du dss.
  if ( !FStartingPoint.is4Connected(APoint) )
    return false;

  int dir = APoint.getDirection(FStartingPoint);

  // 1) On n'est pas encore dans un quadrant précis,
  //    i.e. on est horizontal ou vertical.
  //    Si la direction reste la même, on prolonge le DSS
  if ( FDir2==0 && abs(dir)==abs(FDir1) )
    {
      if ( dir==-FDir1 ) return false; // Retour en arrière impossible

      FLowerLeft.shiftInReverseDirection(FDir1);
      FUpperLeft.shiftInReverseDirection(FDir1);
      FStartingPoint = APoint;
      return true;
    }

  // 2) Ici soit on est déjà dans un quadrant, i.e. on est en
  // diagonale, soit on vient juste de changer de direction.
  if ( FDir2==0 )
    { // On vient de changer de direction : ce point sera forcément accepté
      FDir2 = dir;
    }
  else
    { // Vérification de la compatibilité avec les 2 directions courantes
      if ( dir!=FDir1 && dir!=FDir2 ) return false;
    }

  // Test du cas courant et mis à jour des paramètres.
  int h = Fb*APoint.x - Fa*APoint.y + Fc;

  // Si on est dans la droite, il n'y a rien à mettre à jour
  if ( 0<h && h<abs(Fa)+abs(Fb)-1 )
    {}
  else if ( h==0 )
    {
      FLowerLeft = APoint;
    }
  else if ( h==abs(Fa)+abs(Fb)-1 )
    {
      FUpperLeft = APoint;
    }
  else if ( h==-1 )
    {
      FLowerLeft  = APoint;
      FUpperRight = FUpperLeft;

      Fa = FLowerRight.x - APoint.x;
      Fb = FLowerRight.y - APoint.y;
      Fc = Fa*FLowerRight.y - Fb*FLowerRight.x;
    }
  else if ( h==abs(Fa)+abs(Fb) )
    {
      assert( h==abs(Fa)+abs(Fb) );

      FUpperLeft  = APoint;
      FLowerRight = FLowerLeft;

      Fa = FUpperRight.x - APoint.x;
      Fb = FUpperRight.y - APoint.y;
      Fc = Fa*FLowerRight.y - Fb*FLowerRight.x;
    }
  else
    return false; // Le point est rejeté.

  FStartingPoint = APoint; // On décale le premier point
  FLastDir	 = dir; // On met à jour la dernière direction utilisée

  return true;
}
//******************************************************************************
bool CDss::unionDSS(const CDss &ADss)
{
  if( FEndPoint!=ADss.FStartingPoint ) return false;

  CDss localdss;

  ///Test Unimodularité
  if ( ( fabs ( Fa * ADss.Fb - Fb*ADss.Fa ) != 1 ) and
       ( fabs ( Fa * ADss.Fb - Fb*ADss.Fa ) != 0 ) )
    {
      return false;
    }

  // @TODO à remettre quand on sait ajouter des points des 2 cotés
  // We add the shorter dss to the longer.
  //  if ( ADss.getNbPixels()<=getNbPixels() )
  {
    // We add ADss to this
    localdss = ( *this );
    Point<int> p ( ADss.FStartingPoint );

    // We add successively each point of dss to localdss, while it is possible.
    while ( p!=ADss.FEndPoint )
      {
        p = ADss.nextPoint ( p );

        if ( !localdss.addPointRight ( p ) ) return false;
      }
  }
  /*  else
  {
    // We add this into ADss
    localdss = ADss;
    Point<int> p(ADss.FEndPoint);

    // We add successively each point of dss to localdss, while it is possible.
    while ( p!=FStartingPoint )
  {
  p = ADss.prevPoint(p);

  if ( !localdss.addPointLeft(p) ) return false;
  }
  }
  */
  // Here the union was possible, we update this.
  (*this)=localdss;
  return true;
}
//******************************************************************************
/**  Define the preimage based on CDss parameters
 *
 *  A = Dual (FUpperRight, FLowerLeft)
 *  D = (Fb/Fa,Fc/Fa)
 *  C = Dual (FUpperLeft, FLowerRight)
 *  B = (Fb/Fa,(Fc-|Fb|-|Fa|+1)/Fa);
 */
void CDss::updateToPreimage()
{
}

////Is C between [AB] ? 1D
bool simpleBetween(int A ,int B, int C)
{
	return (((A <= C) && (C <= B)) ||  ((A >= C) && (C >= B)));
}


void switchPoint(Point<int> &A, Point<int> &B)
{
	Point<int> tmp;
	tmp = A;
	A = B;
	B = tmp;
}


//Rotate a DSS to the first Quadrant
void CDss::rotateFirstQuadrant()
{

	if (Fa<0)
	{
		Fa = abs(Fa);
		FUpperLeft.x = -FUpperLeft.x;
		FUpperRight.x = -FUpperRight.x;
		FLowerLeft.x = -FLowerLeft.x;
		FLowerRight.x = - FLowerRight.x;
		switchPoint(FUpperLeft,FLowerLeft);
		switchPoint(FUpperRight,FLowerRight);

		FStartingPoint.x = -FStartingPoint.x;
		FEndPoint.x = -FEndPoint.x;
		if (abs(FDir1) == 1)
			FDir1 = -FDir1;
		if (abs(FDir2) == 1)
			FDir2 = -FDir2;
		if (abs(FLastDir) == 1)
			FLastDir = -FLastDir;
	}

	if (Fb<0)
	{
		Fb = abs(Fb);
		FUpperLeft.y = -FUpperLeft.y;
		FUpperRight.y = -FUpperRight.y;
		FLowerLeft.y = -FLowerLeft.y;
		FLowerRight.y = - FLowerRight.y;

		switchPoint(FUpperLeft,FLowerLeft);
		switchPoint(FUpperRight,FLowerRight);


		FStartingPoint.y = -FStartingPoint.y;
		FEndPoint.y = -FEndPoint.y;
		if (abs(FDir1) == 2)
			FDir1 = -FDir1;
		if (abs(FDir2) == 2)
			FDir2 = -FDir2;
		if (abs(FLastDir) == 2)
			FLastDir = -FLastDir;
	}

	//We update Fc
	Fc = -Fb*FLowerLeft.x + Fa*FLowerLeft.y;
}


void CDss::updateFromPreimage()
{
}

//******************************************************************************
Point<int> CDss::prevPoint(Point<int> &APoint) const
{
  assert ( FDir1!=0 );
  assert ( APoint!=FStartingPoint );

  //We test the point in direction FDir1 in first
  Point<int> res(APoint.getPointInDirection(-FDir1));
  int r = res.x*Fb - res.y*Fa + Fc;

  if ( (r>=0) and (r<abs(Fa)+abs(Fb)) )
    return res;

  if ( FDir2!=0 )
    {
      res = APoint.getPointInDirection(-FDir2);
      r = res.x*Fb - res.y*Fa + Fc;
      assert ( (r>=0) and (r<abs(Fa)+abs(Fb)) );

      return res;
    }

  assert(false);
  return APoint;
}
//******************************************************************************
Point<int> CDss::nextPoint(Point<int> &APoint) const
{
  assert ( FDir1!=0 );
  assert ( APoint!=FEndPoint );

  //We test the point in direction FDir1 in first
  Point<int> res(APoint.getPointInDirection(FDir1));
  int r = res.x*Fb - res.y*Fa + Fc;

	if ( (r>=0) and (r<abs(Fa)+abs(Fb)) )
    return res;

  if ( FDir2!=0 )
    {
      res = APoint.getPointInDirection(FDir2);
      r = res.x*Fb - res.y*Fa + Fc;
      assert ( (r>=0) and (r<abs(Fa)+abs(Fb)) );

      return res;
    }

  assert(false);
  return APoint;
}
//******************************************************************************
void CDss::drawPixels(ostream& AOs) const
{
  Point<int> p(FStartingPoint);

  AOs << "Pixels : "<<p;

  while ( p!=FEndPoint )
    {
      p	= nextPoint(p);
      AOs<<", "<<p;
    }



  AOs<<endl;
}
//------------------------------------------------------------------------------
int CDss::getNbPixels() const
{
  Point<int> tmp(FEndPoint-FStartingPoint);
  return abs(tmp.x)+abs(tmp.y)+1;
}
//------------------------------------------------------------------------------
ostream& operator<<(ostream& AOs,const  CDss &ADss)
{
  return AOs <<"a,b,c("<<ADss.Fa<<","<<ADss.Fb<<","<<ADss.Fc<<")"
	     <<" dir("<<ADss.FDir1<<","<<ADss.FDir2<<"), last="<<ADss.FLastDir
	     <<" Lower(left "<<ADss.FLowerLeft
	     <<", right "<<ADss.FLowerRight<<") "
    	     <<" Upper(left"<<ADss.FUpperLeft
	     <<", right "<<ADss.FUpperRight<<") "
	     <<" Left"<<ADss.FStartingPoint <<" Right"<<ADss.FEndPoint
	     <<" Lg="<<ADss.getNbPixels();
}
//******************************************************************************

