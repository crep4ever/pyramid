/*
 * Copyright (C) 2008-2011, Romain Goffe <romain.goffe@gmail.com>
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
#ifndef __SCENE_HH__
#define __SCENE_HH__
//******************************************************************************
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsItemGroup>
#include <QList>
#include <QSlider>
//******************************************************************************

namespace Map2d
{
  class CDart;
  class CPyramid;
  class CTile;
  class CTopologicalMap;
};

class QAction;
class QPixmap;
class QToolBar;
class CView;
class CControler;
class CDoublet;

/** 
 * \file scene.hh
 * \class CScene: "scenePyramid.hh" 
 * \author Romain Goffe
 * \brief CScene is a scene used to represent a hierachy of 2d partitions
 */

class CScene : public QGraphicsScene
{
  Q_OBJECT

 
  public:
  /// constructor
  CScene( CControler * controler, uint ALevel=1 );

  /// destructor
  virtual ~CScene();

  /// return the actual controler with correct type
  CControler * controler() const;

  /// read settings of the scene
  virtual void readSettings();

  /// write settings of the scene
  virtual void writeSettings();

  /// getter on the image drawn property
  bool isImageToDisplay() const;

  /// getter on the frame drawn property
  //bool isFrameToDisplay() const;

  /// getter on the interpixel drawn property
  bool isInterpixelToDisplay() const;

  /// getter on the map drawn property
  bool isMapToDisplay() const;

  /// getter on the borders drawn property
  bool isBordersToDisplay() const;
  bool isLabelsToDisplay() const;
  bool isFictiveBordersToDisplay() const;

  /// getter on the current level of the pyramid
  uint level() const;

  /// getter on the auto hide property
  bool autoHide() const;

  /// getter on the auto hide threshold property
  qreal autoHideThreshold() const;

  /// getter on the filter property
  uint filter() const;

  // a slider for level selection
  void levelSelector();

public slots:
  /// empty scene
  virtual void empty();

  /// update scene
  virtual void update();

  /// setter on the image drawn property
  void setImageToDisplay( bool value );
  
  /// setter on the frame drawn property
  //void setFrameToDisplay( bool value );

  /// setter on the interpixel drawn property
  void setInterpixelToDisplay( bool value );

  /// setter on the map drawn property
  void setMapToDisplay( bool value );

  /// setter on the frontiers drawn property
  void setBordersToDisplay( bool value );
  void setLabelsToDisplay( bool value );
  void setFictiveBordersToDisplay( bool value );

  /// setter on the current level of the pyramid
  void setLevel( uint level );

  /// setter on the auto hide property
  void setAutoHide( bool value );

  /// setter on the auto hide property
  void setAutoHideThreshold( qreal threshold );

  /// setter on the filter property
  void setFilter( uint filter  );

  /// apply filter() as an opacity sheet
  void setOpacity();

  /// automatic hidding of details depending on zoom factor
  void autoHideOnZoom( qreal zoom );

  virtual void initialize( CView* view );
  
  void sliderChanged(int ALevel);

protected:
  /// draw the pixmap representing the image
  void drawImage();

  /// draw the elements representing the interpixel elements
  void drawInterpixel(Map2d::CTile* ATile, QGraphicsItemGroup * group = NULL);

  /// draw the elements representing the map
  void drawMap(Map2d::CTopologicalMap* map, QGraphicsItemGroup * group = NULL);

  /// draw the elements representing the borders of regions
  void drawBorders(Map2d::CTopologicalMap* map, QGraphicsItemGroup * group = NULL,
		   QGraphicsItemGroup * fictiveGroup = NULL);

  /// draw the image regions labels
  void drawLabels(Map2d::CTile* map, QGraphicsItemGroup * group = NULL);

  /// draw the elements representing the borders of tiles
  //  void drawFictiveBorders(Map2d::CTopologicalMap* map, QGraphicsItemGroup * group = NULL,
  //			  QGraphicsItemGroup * fictiveGroup = NULL);

  /// draw a dart from the current topological map
  void drawDart( Map2d::CTopologicalMap* map, Map2d::CDart* dart, QGraphicsItemGroup* group = NULL,
		 QGraphicsItemGroup* AFictiveGroup=NULL );

  /// draw an edge from the current topological map
  void drawEdge( Map2d::CTopologicalMap* map, Map2d::CDart* dart, QGraphicsItemGroup* group = NULL,
		 QGraphicsItemGroup* fictiveGroup = NULL );

  /// draw a pointel
  void drawPointel( const CDoublet& doublet, QGraphicsItemGroup* group=NULL );

  /// draw a linel
  //void drawLinel( const CDoublet& doublet, uint AOrientation, QGraphicsItemGroup* group=NULL );
  void drawLinel( uint x, uint y, uint orientation, bool fictive, QGraphicsItemGroup* group=NULL );


  /// draw the level of the pyramid that corresponds to m_level
  void drawCurrentLevel();

  //only draw visible tiles
  void drawCurrentLevel2();

  /// draw a map without darts that belong to the infinite region
  void drawOpenMap(Map2d::CTile* ATile, QGraphicsItemGroup* AGroup=NULL,
		   QGraphicsItemGroup* AFictiveGroup=NULL);

  CControler* m_controler;

  /// pixmap containing the data
  QPixmap* m_pixmap;
  /// item for the image
  QGraphicsItemGroup * m_imageItem;
  /// item for the image frame
  //QGraphicsRectItem * m_frameItem;
  /// item for the interpixel
  QGraphicsItemGroup * m_interpixelItem;
  /// item for the map 
  QGraphicsItemGroup * m_mapItem;
  /// item for the borders
  QGraphicsItemGroup * m_bordersItem;
  QGraphicsItemGroup * m_labelsItem;
  /// item for the fictive borders
  QGraphicsItemGroup * m_fictiveBorders;

  bool m_isImageToDisplay;
  //bool m_isFrameToDisplay;
  bool m_isInterpixelToDisplay;
  bool m_isMapToDisplay;
  bool m_isBordersToDisplay;
  bool m_isLabelsToDisplay;
  bool m_isFictiveBordersToDisplay;
  uint m_level;
  bool m_autoHide;
  qreal m_autoHideThreshold;
  uint m_filter;

  QPen dartPen;
  QBrush dartBrush;
  QPen representativeDartPen;
  QBrush representativeDartBrush;
  QPen fictiveDartPen;
  QBrush fictiveDartBrush;
  QPen edgePen;
  QPen pointelPen;
  QPen selectedPen;
  QBrush pointelBrush;
  QPen linelPen;
  QBrush linelBrush;
  QPen linelFictivePen;
  QBrush linelFictiveBrush;

  QAction * m_actionImageToDisplay;
  //QAction * m_actionFrameToDisplay;
  QAction * m_actionInterpixelToDisplay;
  QAction * m_actionMapToDisplay;
  QAction * m_actionBordersToDisplay;
  QAction * m_actionLabelsToDisplay;
  QAction * m_actionFictiveBordersToDisplay;
  QAction * m_actionAutoHide;

  CView* m_view;

public:
  QSlider * m_levelSelector;
  uint m_count;

  QToolBar* m_drawToolBar;

  QList< QAction* > m_actions;

  /// set all the actions provided by the scene
  void setActions();

  void addAction(QAction* action, bool value=false);

  /// view actions provided by the scene
  QList< QAction* > actions() const;

  QWidget* drawToolBar();
  
private:
  /// add an arrow at the end of the path according to the direction
  void addArrow(QPainterPath* APath, const CDoublet& ADirection );
  
  /// compute the gap to have a cool representation of the map
  QPointF calculateGap( const QPointF & point, 
			const CDoublet & current, const CDoublet & next );

};
//******************************************************************************
#endif // __SCENE_HH__
//******************************************************************************
