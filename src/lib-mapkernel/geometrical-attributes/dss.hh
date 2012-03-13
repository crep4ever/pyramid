/***************************************************************************
 *   Copyright (C) 2007 by Guillaume Damiand and David Coeurjolly          *
 *   damiand@sic.univ-poitiers.fr david.coeurjolly@liris.cnrs.fr           *
 *                                                                         *
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
#ifndef DSS_HH
#define DSS_HH
//******************************************************************************
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <vector>

#include "point.hh"

//******************************************************************************
class CDss
{
  public:

    //Create an empty DSS based.
    CDss();

    //Create a DSS based on specific edge.
    //PRECOND : the two points APoint1 and APoint2 must be 4-connected
    CDss(const Point<int> &APoint1, const Point<int> &APoint2);

    //Create a DSS by copy constructor.
    CDss(const CDss &ADss);

    // Affectation operator.
    CDss &operator=(const CDss &ADss);

    // Init a DSS given two 4-connected points.
    bool init(const Point<int> &APoint1, const Point<int> &APoint2);
    bool init(int Ax1, int Ay1, int Ax2, int Ay2);

    //Reverse a DSS
    void reverse();

    //Add a pixel to the right of the DSS. Return true iif the pixel was added.
    bool addPointRight(const Point<int> &APoint);

    //Add a pixel to the left of the DSS. Return true iif the pixel was added.
    bool addPointLeft(const Point<int> &APoint);

    //High level merging operation
    /**
     *  Self becomes the merge between a DSS and self
     *
     *  PRECOND:   --self-->o--dss-->    (no  --->o<----, <---o--->,...)
     *
     * @param ADss
     * @return True or false if the union has been done
     */
    bool unionDSS(const CDss &ADss);

    // Operator <<
    friend std::ostream& operator<<(std::ostream&, const CDss &);

    // Affichage de tout les pixels du DSS
    void drawPixels(std::ostream& AOs) const;

    // Return the previous following APoint in the DSS
    // @precond APoint in the DSS and not equal to FStartingPoint
    Point<int> prevPoint(Point<int> &APoint) const;

    // Return the point following APoint in the DSS
    // @precond APoint in the DSS and not equal to FEndPoint
    Point<int> nextPoint(Point<int> &APoint) const;

    // Return the number of pixels that belong to the DSS
    int getNbPixels() const;

	////
	//Preimage Related Stuffs
	////

	//Rotate the DSS to the first Quadrant
	void rotateFirstQuadrant();

	//We update the Preimage
	void updateToPreimage();

	//We update the CDss parameters from Preimage
	void updateFromPreimage();


	// private:
    // Datas

    // Extremities of the segment
    Point <int> FStartingPoint, FEndPoint;

    // Leaning points
    Point<int> FLowerLeft, FLowerRight; // lower (or negative) line
    Point<int> FUpperLeft, FUpperRight; // upper (or positive) line

    // Les deux directions du DSS :
    //   0=pas utilisé; 1=droite, 2=haut, -1=gauche, -2=bas
    int FDir1, FDir2; // FDir1 est forcément != 0 mais FDir2 peut être = 0.
    int Fa,Fb,Fc;     // Parameters of the line

    // La dernière direction (entre l'avant dernier et le dernier point)
    // Nécessaire pour la fonction reverse.
    int FLastDir;
};
//******************************************************************************
#endif // DSS_HH
//******************************************************************************
