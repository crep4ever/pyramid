/*
 * Copyright (C) 2008-2011, Alexandre Dupas <alexandre.dupas@gmail.com>
 *                          Romain Goffe <romain.goffe@gmail.com>
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

//*****************************************************************************
#ifndef __SVG_CREATOR_HH__
#define __SVG_CREATOR_HH__
//*****************************************************************************
#include <iostream>
#include <ostream>
#include <vector>
//*****************************************************************************

class CSvgCreator
{
  static const char endl = '\n';

public:
  typedef std::pair<double,double> TCoord;

public:
  CSvgCreator( std::ostream & os );
  ~CSvgCreator();

  // SVG Element (document creation)
  void svgBegin( double h, double w,
		 const std::string & opt = "" );
  void svgEnd();

  // Definitions
  void defsBegin( const std::string & opt = "" );
  void defsEnd();

  // Markers
  void markerBegin( const std::string & id,
		    double refX = 0.0,
		    double refY = 0.0,
		    const std::string & orient = "auto",
		    const std::string & opt = "" );
  void markerEnd();

  // Use directive
  void use( const std::string & href,
	    double x=0, double y=0,
	    const std::string & opt = "" );

  // CSS Styling
  void styleCSS( const std::string & AStyle );

  // Grouping
  void groupBegin( const std::string & opt = "" );
  void groupEnd();

  // Basic shape (missing ellipse)
  void line( double x1, double y1, double x2, double y2,
	     const std::string & opt = "" );
  void circle( double cx, double cy, double r,
	       const std::string & opt = "" );
  void rect( double x1, double y1,
	     double width, double height,
	     const std::string & opt = "",
	     double rx=0, double ry=0 );

  void polyPoint( double x, double y );
  void polylineBegin( const std::string & opt = "" );
  void polylineEnd();
  void polygonBegin( const std::string & opt = "" );
  void polygonEnd();

  // Path drawing
  void pathBegin( double x, double y, const std::string & opt = "" );
  void pathEnd();

  void pathClose();

  void pathLineTo( std::vector< TCoord > points );
  void pathLineTo( double x, double y );
  void pathHLineTo( double x );
  void pathVLineTo( double y );

  void pathCurveTo( std::vector< TCoord > points );
  void pathCurveTo( double x1, double y1,
		    double x2, double y2,
		    double x, double y );

  void pathSCurveTo( std::vector< TCoord > points );
  void pathSCurveTo( double x2, double y2,
		     double x, double y );

  void pathQCurveTo( std::vector< TCoord > points );
  void pathQCurveTo( double x1, double y1,
		     double x, double y );

  void pathSQCurveTo( std::vector< TCoord > points );
  void pathSQCurveTo( double x, double y );

  // Contatenation
  void import( const std::string & str );

protected:
  enum TPathType {
    MOVE_TO='M',
    CLOSE='Z',
    LINE_TO='L',
    HLINE_TO='H',
    VLINE_TO='V',
    CURVE_TO='C',
    SCURVE_TO='S',
    QCURVE_TO='Q',
    SQCURVE_TO='T'
  };

  void point( double x, double y );
  void point( const TCoord & ACoord );
  void typedPoint( TPathType t, double x, double y );

  // Ouput stream
  std::ostream & FOs;
};

#endif // __SVG_CREATOR_HH__
