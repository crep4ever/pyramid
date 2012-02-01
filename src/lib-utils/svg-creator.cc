#include "svg-creator.hh"
#include <iostream>
#include <ostream>

//------------------------------------------------------------------------------
CSvgCreator::CSvgCreator( std::ostream & os ) :
  FOs( os )
{}

//------------------------------------------------------------------------------
CSvgCreator::~CSvgCreator()
{}


//******************************************************************************
// SVG Element (document creation)
//******************************************************************************


void CSvgCreator::svgBegin( double h, double w, const std::string & opt )
{
  FOs << "<?xml version=\"1.0\"?>" << endl
      << "<svg "
      << "height=\"" << h << "\" "
      << "width=\"" << w << "\" "
      << opt
      << ">"
      << endl;

  // Palette tango: black:#2e3436 ; blue:#204a87; red:#ef2929 ; green:#4e9a06 ; white:#eeeeec ;
  //  styleCSS(" .dart { fill:none; stroke:\"#2e3436\"; stroke-width:0.1; } .representativeDart { fill:none; stroke:\"#ef2929\"; stroke-width:0.1; } .pointel { fill:\"#4e9a06\"; stroke:\"#4e9a06\"; stroke-width:0.1; } .linel { fill:\"#4e9a06\"; stroke:\"#4e9a06\"; stroke-width:0.1; } .firstPixel { fill=\"#eeeeec\"; stroke:\"#204a87\"; stroke-width:0.1; } " );

  styleCSS(" .dart { fill:none; stroke:#2e3436; stroke-width:0.1; } .representativeDart { fill:none; stroke:#ef2929; stroke-width:0.1; } .pointel { fill:none; stroke:#204a87; stroke-width:0.1; } .linel { fill:none; stroke:#4e9a06; stroke-width:0.1; } .fpixel { fill:#eeeeec; stroke:#204a87; stroke-width:0.1; } .fictivePointel { fill:none; stroke:#ef2929; stroke-width:0.1; } .fictiveLinel { fill:none; stroke:#ef2929; stroke-width:0.1; }" );

  defsBegin();

  // Définition du marker inkscape Arrow2Send
  typedef std::pair<double,double> TCoord;
  std::vector<TCoord> tab;  // Le vecteur des points de la courbe
  tab.push_back(TCoord(6.9730900,-1.6296469));
  tab.push_back(TCoord(6.9831476,1.6157441));
  tab.push_back(TCoord(8.7185878,4.0337352));

  markerBegin( "arrow", 0, 0, "auto" );

  pathBegin(8.7185878, 4.0337352, "style=\"font-size:12.0;fill-rule:evenodd;stroke-width:0.62500000;stroke-linejoin:round;\" transform=\"scale(0.3) rotate(180) translate(-2.3,0)\" ");

  pathLineTo(-2.2072895,0.016013256);
  pathLineTo(8.7185884,-4.0017078);
  pathCurveTo(tab);

  pathClose();
  pathEnd();

  markerEnd();
  defsEnd();

}

//------------------------------------------------------------------------------
void CSvgCreator::svgEnd()
{
  FOs << "</svg>" << endl;
}

//******************************************************************************
// Definitions
//******************************************************************************

void CSvgCreator::defsBegin( const std::string & opt )
{
  FOs << "<defs>" << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::defsEnd()
{
  FOs << "</defs>" << endl;
}

//******************************************************************************
// Markers
//******************************************************************************

void CSvgCreator::markerBegin( const std::string & id,
			       double refX,
			       double refY,
			       const std::string & orient,
			       const std::string & opt )
{
  FOs << "<marker "
      << "id=\""<< id << "\" "
      << "orient=\""<< orient <<"\" "
      << "refX=\"" << refX << "\" "
      << "refY=\"" << refY << "\" "
      << opt << ">"
      << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::markerEnd()
{
  FOs << "</marker>" << endl;
}


//******************************************************************************
// Use directive
//******************************************************************************

void CSvgCreator::use( const std::string & href,
		       double x, double y,
		       const std::string & opt )
{}

//******************************************************************************
// CSS Styling
//******************************************************************************

void CSvgCreator::styleCSS( const std::string & AStyle )
{
  FOs << "<style type=\"text/css\"><![CDATA["
      << endl
      << AStyle
      << endl
      << "]]></style>"
      << endl;
}

//******************************************************************************
// Grouping
//******************************************************************************

void CSvgCreator::groupBegin( const std::string & opt )
{
  FOs << "<g " << opt << ">" << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::groupEnd()
{
  FOs << "</g>" << endl;
}

//******************************************************************************
// Basic shape (missing ellipse)
//******************************************************************************

void CSvgCreator::line( double x1, double y1, double x2, double y2,
			const std::string & opt )
{
  FOs << "<line "
      << "x1=\"" << x1 << "\" y1=\"" << y1 << "\" "
      << "x2=\"" << x2 << "\" y2=\"" << y2 << "\" "
      << opt
      << "/>"
      << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::circle( double cx, double cy, double r,
			  const std::string & opt )
{
  FOs << "<circle "
      << "cx=\"" << cx << "\" cy=\"" << cy << "\" "
      << "r=\"" << r << "\" "
      << opt
      << "/>"
      << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::rect( double x, double y,
			double width, double height,
			const std::string & opt,
			double rx, double ry )
{
  FOs << "<rect "
      << "x=\"" << x << "\" y=\"" << y << "\" "
      << "width=\"" << width << "\" height=\"" << height << "\" ";

  if( rx != 0 )
    FOs << "rx=\"" << rx << "\" ";
  if( ry != 0 )
    FOs << "ry=\"" << ry << "\" ";
  FOs << opt
      << "/>"
      << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::polyPoint( double x, double y )
{
  point( x, y );
}

//------------------------------------------------------------------------------
void CSvgCreator::polylineBegin( const std::string & opt )
{
  FOs << "<polyline "
      << opt
      << " points=\"";
}

//------------------------------------------------------------------------------
void CSvgCreator::polylineEnd()
{
  FOs << "\" />" << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::polygonBegin( const std::string & opt )
{
  FOs << "<polygon "
      << opt
      << " points=\"";
}

//------------------------------------------------------------------------------
void CSvgCreator::polygonEnd()
{
  FOs << "\" />" << endl;
}

//******************************************************************************
// Path drawing
//******************************************************************************

void CSvgCreator::pathBegin( double x, double y, const std::string & opt )
{
  FOs << "<path "
      << opt
      << " d=\"";
  typedPoint( MOVE_TO, x, y );
}

//------------------------------------------------------------------------------
void CSvgCreator::pathEnd()
{
  FOs << "\" />" << endl;
}

//------------------------------------------------------------------------------
void CSvgCreator::pathLineTo( std::vector< TCoord > points )
{
  FOs << static_cast<char>(LINE_TO) << " ";
  for( std::vector< TCoord >::iterator it = points.begin() ;
       it != points.end() ; ++it )
    {
      point( *it );
    }
}

//------------------------------------------------------------------------------
void CSvgCreator::pathLineTo( double x, double y )
{
  typedPoint( LINE_TO, x, y );
}

//------------------------------------------------------------------------------
void CSvgCreator::pathHLineTo( double x )
{
  FOs << static_cast<char>(HLINE_TO) << " " << x << " ";
}

//------------------------------------------------------------------------------
void CSvgCreator::pathVLineTo( double y )
{
  FOs << static_cast<char>(VLINE_TO) << " " << y << " ";
}

//------------------------------------------------------------------------------
void CSvgCreator::pathClose()
{
  FOs << static_cast<char>(CLOSE);
}

//------------------------------------------------------------------------------
void CSvgCreator::pathCurveTo( std::vector< TCoord > points )
{
  if( points.size() % 3 != 0 )
    {
      std::cerr << "Error in the path" << std::endl;
    }
  FOs << static_cast<char>(CURVE_TO) << " ";
  for( std::vector< TCoord >::iterator it = points.begin() ;
       it != points.end() ; ++it )
    {
      point( *it );
    }
}

//------------------------------------------------------------------------------
void CSvgCreator::pathCurveTo( double x1, double y1,
			       double x2, double y2,
			       double x, double y )
{
  typedPoint( CURVE_TO, x1, y1 );
  point( x2, y2 );
  point( x, y );
}

//------------------------------------------------------------------------------
void CSvgCreator::pathSCurveTo( std::vector< TCoord > points )
{
  if( points.size() % 2 != 0 )
    {
      std::cerr << "Error in the path" << std::endl;
    }
  FOs << static_cast<char>(SCURVE_TO) << " ";
  for( std::vector< TCoord >::iterator it = points.begin() ;
       it != points.end() ; ++it )
    {
      point( *it );
    }
}

//------------------------------------------------------------------------------
void CSvgCreator::pathSCurveTo( double x2, double y2,
				double x, double y )
{
  typedPoint( SCURVE_TO, x2, y2 );
  point( x, y );
}

//------------------------------------------------------------------------------
void CSvgCreator::pathQCurveTo( std::vector< TCoord > points )
{
  if( points.size() % 2 != 0 )
    {
      std::cerr << "Error in the path" << std::endl;
    }
  FOs << static_cast<char>(QCURVE_TO) << " ";
  for( std::vector< TCoord >::iterator it = points.begin() ;
       it != points.end() ; ++it )
    {
      point( *it );
    }
}
void CSvgCreator::pathQCurveTo( double x1, double y1, double x, double y )
{
  typedPoint( QCURVE_TO, x1, y1 );
  point( x, y );
}

//------------------------------------------------------------------------------
void CSvgCreator::pathSQCurveTo( std::vector< TCoord > points )
{
  FOs << static_cast<char>(SQCURVE_TO) << " ";
  for( std::vector< TCoord >::iterator it = points.begin() ;
       it != points.end() ; ++it )
    {
      point( *it );
    }
}

//------------------------------------------------------------------------------
void CSvgCreator::pathSQCurveTo( double x, double y )
{
  typedPoint( SQCURVE_TO, x, y );
}

//******************************************************************************
// Import a raw string
//******************************************************************************

void CSvgCreator::import( const std::string & str )
{
  FOs << str;
}

//******************************************************************************
// Utilities
//******************************************************************************

void CSvgCreator::point( double x, double y )
{
  FOs << x << "," << y << " ";
}

//------------------------------------------------------------------------------
void CSvgCreator::point( const TCoord & ACoord )
{
  point( ACoord.first, ACoord.second );
}

//------------------------------------------------------------------------------
void CSvgCreator::typedPoint( TPathType t, double x, double y )
{
  FOs << static_cast<char>(t) << " ";
  point( x, y );
}
