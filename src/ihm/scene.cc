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
#include <QtGui>
#include "scene.hh"
#include "controler.hh"
#include "view.hh"
#include "imageTiff.hh"
#include "pyramid.hh"
#include "level.hh"
#include "pyramidal-dart.hh"
#include "tile.hh"
#include "dart-path.hh"
#include "region.hh"
#include "traversal-region-pixels.hh"
using namespace Map2d;
using namespace fogrimmi;
//******************************************************************************
CScene::CScene( CControler * AControler, uint ALevel )
  : QGraphicsScene( )
  , m_controler(AControler)
  , m_pixmap(NULL)
  , m_imageItem(NULL)
  , m_interpixelItem(NULL)
  , m_mapItem(NULL)
  , m_bordersItem(NULL)
  , m_labelsItem(NULL)
  , m_fictiveBorders(NULL)
  , m_isImageToDisplay(false)
  , m_isInterpixelToDisplay(false)
  , m_isMapToDisplay(false)
  , m_isBordersToDisplay(true)
  , m_isLabelsToDisplay(false)
  , m_level(ALevel)
  , m_autoHide(false)
  , m_autoHideThreshold(10.0)
  , m_filter(100)
  , dartPen(QColor(0,0,0))
  , dartBrush(QColor(0,0,0))
  , representativeDartPen(QColor(0xce,0x5c,0x00))
  , representativeDartBrush(QColor(0xce,0x5c,0x00))
  , fictiveDartPen(Qt::red)
  , fictiveDartBrush(Qt::red)
  , edgePen(QColor(0,0,0))
  , pointelPen(QColor(0xa4,0x00,0x00))
  , pointelBrush(QColor(0xa4,0x00,0x00))
  , linelPen(QColor(0x72,0x97,0xcf))
  , linelBrush(QColor(0x72,0x97,0xcf))
  , linelFictivePen(QColor(0xce,0x5c,0x00))
  , linelFictiveBrush(QColor(0xce,0x5c,0x00))
  , m_actionImageToDisplay(NULL)
  , m_actionInterpixelToDisplay(NULL)
  , m_actionMapToDisplay(NULL)
  , m_actionBordersToDisplay(NULL)
  , m_actionAutoHide(NULL)
  , m_levelSelector(NULL)
  , m_count(0)
  , m_drawToolBar(new QToolBar)
{
  empty();
  //widgets
  levelSelector();
  //settings
  readSettings();
  //actions
  setActions();
  autoHideOnZoom(1.0);
  update();
}

//------------------------------------------------------------------------------
CScene::~CScene()
{}
//------------------------------------------------------------------------------
CControler * CScene::controler() const
{
  return m_controler;
}
//------------------------------------------------------------------------------
void CScene::readSettings()
{
  QSettings settings;

  settings.beginGroup("CScene");
  setImageToDisplay(settings.value("isImageToDisplay",m_isImageToDisplay).toBool());
  setInterpixelToDisplay(settings.value("isInterpixelToDisplay",m_isInterpixelToDisplay).toBool());
  setMapToDisplay(settings.value("isMapToDisplay",m_isMapToDisplay).toBool());
  setBordersToDisplay(settings.value("isBordersToDisplay",m_isBordersToDisplay).toBool());
  setLabelsToDisplay(settings.value("isLabelsToDisplay",m_isLabelsToDisplay).toBool());
  setAutoHide(settings.value("autoHide",m_autoHide).toBool());
  setAutoHideThreshold(settings.value("autoHideThreshold",m_autoHideThreshold).toDouble());
  setFilter(settings.value("opacity",m_filter).toUInt());
  settings.endGroup();
}
//------------------------------------------------------------------------------
void CScene::writeSettings()
{
  QSettings settings;

  settings.beginGroup("CScene");
  settings.setValue("isImageToDisplay",QVariant::fromValue(isImageToDisplay()));
  settings.setValue("isInterpixelToDisplay",isInterpixelToDisplay());
  settings.setValue("isMapToDisplay",isMapToDisplay());
  settings.setValue("isBordersToDisplay",isBordersToDisplay());
  settings.setValue("isLabelsToDisplay",isLabelsToDisplay());
  settings.setValue("autoHide",autoHide());
  settings.setValue("autoHideThreshold",autoHideThreshold());
  settings.setValue("opacity",filter());
  settings.endGroup();
}

//------------------------------------------------------------------------------
void CScene::initialize( CView * view )
{
  connect( view, SIGNAL(zoomChanged(qreal)), SLOT(autoHideOnZoom(qreal)) );
  setBackgroundBrush(Qt::white);
}

//------------------------------------------------------------------------------
void CScene::setActions()
{
  QAction * action = NULL;

  action = new QAction(tr("Image"), this);
  action->setStatusTip(tr("Show or hide the image"));
  action->setCheckable(true);
  action->setChecked(isImageToDisplay());
  connect(action, SIGNAL(toggled(bool)), SLOT(setImageToDisplay(bool)));
  addAction( action, true );
  m_actionImageToDisplay = action;

  action = new QAction(tr("Interpixel"), this);
  action->setStatusTip(tr("Show or hide the image interpixel"));
  action->setCheckable(true);
  action->setChecked(isInterpixelToDisplay());
  connect(action, SIGNAL(toggled(bool)), SLOT(setInterpixelToDisplay(bool)));
  addAction( action, true );
  m_actionInterpixelToDisplay = action;

  action = new QAction(tr("Map"), this);
  action->setStatusTip(tr("Show or hide the image map"));
  action->setCheckable(true);
  action->setChecked(isMapToDisplay());
  connect(action, SIGNAL(toggled(bool)), SLOT(setMapToDisplay(bool)));
  addAction( action, true );
  m_actionMapToDisplay = action;

  action = new QAction(tr("Borders"), this);
  action->setStatusTip(tr("Show or hide the image borders"));
  action->setCheckable(true);
  action->setChecked(isBordersToDisplay());
  connect(action, SIGNAL(toggled(bool)), SLOT(setBordersToDisplay(bool)));
  addAction( action, true );
  m_actionBordersToDisplay = action;

  action = new QAction(tr("Labels"), this);
  action->setStatusTip(tr("Show or hide the image region labels"));
  action->setCheckable(true);
  action->setChecked(isLabelsToDisplay());
  connect(action, SIGNAL(toggled(bool)), SLOT(setLabelsToDisplay(bool)));
  addAction( action, true );
  m_actionLabelsToDisplay = action;

  action = new QAction(tr("Show fictive borders"), this);
  action->setStatusTip(tr("Show or hide fictive darts on tile's borders"));
  action->setCheckable(true);
  action->setChecked(isLabelsToDisplay());
  connect(action, SIGNAL(toggled(bool)), SLOT(setFictiveBordersToDisplay(bool)));
  addAction( action );
  m_actionFictiveBordersToDisplay = action;

  action = new QAction(tr("Auto &hide"), this);
  action->setStatusTip(tr("Auto hide interpixel and map depending on zoom"));
  action->setCheckable(true);
  action->setChecked(autoHide());
  connect(action, SIGNAL(toggled(bool)), SLOT(setAutoHide(bool)));
  addAction( action );
  m_actionAutoHide = action;

  action = new QAction(tr("Update"), this);
  action->setIcon(QIcon::fromTheme("view-refresh"));
  action->setStatusTip(tr("Destroy and rebuilds the scene"));
  connect(action, SIGNAL(triggered()), SLOT(update()));
  addAction( action, true );
}
//------------------------------------------------------------------------------
void CScene::empty()
{
  clear();

  m_imageItem = new QGraphicsItemGroup();
  m_interpixelItem = new QGraphicsItemGroup();
  m_mapItem = new QGraphicsItemGroup();
  m_bordersItem = new QGraphicsItemGroup();
  m_labelsItem = new QGraphicsItemGroup();
  m_fictiveBorders = new QGraphicsItemGroup();

  addItem(m_imageItem);
  addItem(m_interpixelItem);
  addItem(m_mapItem);
  addItem(m_bordersItem);
  addItem(m_labelsItem);
  addItem(m_fictiveBorders);

  m_imageItem->setZValue(0);
  m_interpixelItem->setZValue(2);
  m_mapItem->setZValue(2);
  m_bordersItem->setZValue(1);
  m_labelsItem->setZValue(0);

  m_imageItem->setVisible(isImageToDisplay());
  m_interpixelItem->setVisible(isInterpixelToDisplay());
  m_mapItem->setVisible(isMapToDisplay());
  m_bordersItem->setVisible(isBordersToDisplay());
  m_labelsItem->setVisible(isLabelsToDisplay());
}
//------------------------------------------------------------------------------
void CScene::update()
{
  empty();
  controler()->pyramid()->unloadAllTiles();
  if(controler()->pyramid()->image()->isOpen)
    {
      controler()->pyramid()->image()->close();
      controler()->pyramid()->image()->unload();
    }
  drawCurrentLevel();
}
//------------------------------------------------------------------------------
bool CScene::isImageToDisplay() const
{
  return m_isImageToDisplay;
}
//------------------------------------------------------------------------------
bool CScene::isInterpixelToDisplay() const
{
  return m_isInterpixelToDisplay;
}
//------------------------------------------------------------------------------
bool CScene::isMapToDisplay() const
{
  return m_isMapToDisplay;
}
//------------------------------------------------------------------------------
bool CScene::isBordersToDisplay() const
{
  return m_isBordersToDisplay;
}
//------------------------------------------------------------------------------
bool CScene::isLabelsToDisplay() const
{
  return m_isLabelsToDisplay;
}
//------------------------------------------------------------------------------
uint CScene::level() const
{
  return m_level;
}
//------------------------------------------------------------------------------
bool CScene::autoHide() const
{
  return m_autoHide;
}
//------------------------------------------------------------------------------
qreal CScene::autoHideThreshold() const
{
  return m_autoHideThreshold;
}
//------------------------------------------------------------------------------
uint CScene::filter() const
{
  return m_filter;
}
//------------------------------------------------------------------------------
void CScene::setImageToDisplay( bool value )
{
  if( isImageToDisplay() != value )
    {
      m_isImageToDisplay = value;
      m_imageItem->setVisible(value);
    }
}
//------------------------------------------------------------------------------
void CScene::setInterpixelToDisplay( bool value )
{
  if( isInterpixelToDisplay() != value )
    {
      m_isInterpixelToDisplay = value;
      m_interpixelItem->setVisible(value);
    }
}
//------------------------------------------------------------------------------
void CScene::setMapToDisplay( bool value )
{
  if( isMapToDisplay() != value )
    {
      m_isMapToDisplay = value;
      m_mapItem->setVisible(value);
    }
}
//------------------------------------------------------------------------------
void CScene::setBordersToDisplay( bool value )
{
  if( m_isBordersToDisplay != value )
    {
      m_isBordersToDisplay = value;
      m_bordersItem->setVisible(value);
    }
}
//------------------------------------------------------------------------------
void CScene::setLabelsToDisplay( bool value )
{
  if( m_isLabelsToDisplay != value )
    {
      m_isLabelsToDisplay = value;
      m_labelsItem->setVisible(value);
    }
}
//------------------------------------------------------------------------------
void CScene::setFictiveBordersToDisplay( bool value )
{
  if( m_isFictiveBordersToDisplay != value )
    {
      m_isFictiveBordersToDisplay = value;
      m_fictiveBorders->setVisible(value);
    }
}
//------------------------------------------------------------------------------
void CScene::setLevel( uint ALevel )
{
  if( level() != ALevel )
    {
      m_level = ALevel;
      m_levelSelector->setValue(level());
      update();
    }
}
//------------------------------------------------------------------------------
void CScene::setAutoHide( bool value )
{
  m_autoHide = value;
}
//------------------------------------------------------------------------------
void CScene::setAutoHideThreshold( qreal threshold )
{
  m_autoHideThreshold = threshold;
}
//------------------------------------------------------------------------------
void CScene::setFilter( uint value )
{
  m_filter = value;
}
//------------------------------------------------------------------------------
void CScene::setOpacity()
{
  if(m_imageItem==NULL)
    return;

  m_imageItem->setOpacity( filter()/255.0 );
}
//------------------------------------------------------------------------------
void CScene::autoHideOnZoom( qreal zoom )
{
  if( autoHide() )
    {
      if( zoom > autoHideThreshold() )
	{
	  m_actionMapToDisplay->setChecked( true );
	  m_actionInterpixelToDisplay->setChecked( true );
	  m_actionBordersToDisplay->setChecked( false );
	  m_actionLabelsToDisplay->setChecked( false );
	  m_isMapToDisplay = true;
	  m_isInterpixelToDisplay = true;
	  m_isBordersToDisplay = false;
	  m_isLabelsToDisplay = false;
	}
      else
	{
	  assert(m_actionMapToDisplay!=NULL);
	  m_actionMapToDisplay->setChecked( false );
	  m_actionInterpixelToDisplay->setChecked( false );
	  m_actionBordersToDisplay->setChecked( true );
	  m_actionLabelsToDisplay->setChecked( true );
	  m_isMapToDisplay = false;
	  m_isInterpixelToDisplay = false;
	  m_isBordersToDisplay = true;
	  m_isLabelsToDisplay = true;
	}
      update();
    }
}
//------------------------------------------------------------------------------
void CScene::drawImage()
{
  CPyramid * a_pyramid = controler()->pyramid();
  if(a_pyramid==NULL) return;

  CImageTiff * image = a_pyramid->image();
  if( image == NULL ) return;
       
  assert(image->isOpen);
  QImage qImage;
  IM_Box box;

  if(image->getNbPages()>1 || m_pixmap==NULL)
    {
      uint width  = image->getXSize(level());
      uint height = image->getYSize(level());
      
      uint depth = level();
      if(depth > (unsigned)image->getNbPages()-1)
	depth = image->getNbPages()-1;
      
      box.XTop = 0; box.YTop = 0;
      box.XBtm = width; box.YBtm = height;
      image->read(box, depth);
      
      uint8* data = image->getDataQT(box, depth);
      qImage = QImage(data, width, height, QImage::Format_RGB32).rgbSwapped();
      assert(!qImage.isNull());
      m_pixmap = new QPixmap(QPixmap::fromImage(qImage));
      delete [] data;
    }
  
  QGraphicsPixmapItem * pixItem = addPixmap(*m_pixmap);
  if(pixItem != NULL)
    m_imageItem->addToGroup(pixItem);
  
  setOpacity();
}

//------------------------------------------------------------------------------
void CScene::drawInterpixel(CTile* a_tile, QGraphicsItemGroup * group)
{
  if( a_tile == NULL ) return;

  CDoublet temp;
  uint sizeX = a_tile->width()+1;
  uint sizeY = a_tile->height()+1;
  uint x,y;
  
  //#pragma omp parallel shared(a_tile, group) private(x,y,temp)
  for(x=0; x<sizeX; ++x)
    for(y=0; y<sizeY; ++y)
      {
	temp.setX(x);
	temp.setY(y);
	    
	// Affichage des pointels
	if( a_tile->isPCell(temp) )
	  drawPointel(temp, group);
	
	// Affichage des lignels
	temp.setLinel(XPOS);
	if( a_tile->isLCell(temp) )
	  drawLinel(x, y, 1, false, group);
	
	temp.setLinel(YPOS);
	if ( a_tile->isLCell(temp) )
	  drawLinel(x, y, 2, false, group);
      }
}
//------------------------------------------------------------------------------
void CScene::drawMap(CTopologicalMap* a_map, QGraphicsItemGroup * group)
{
  assert(false); //use drawOpenMap instead
  if( a_map == NULL ) return;
  for( CDynamicCoverageAll it(a_map); it.cont(); ++it)
    drawDart(a_map, *it, group);
}
//------------------------------------------------------------------------------
void CScene::drawOpenMap(CTile* a_tile, QGraphicsItemGroup* group, 
				QGraphicsItemGroup* fictiveGroup)
{
  if( a_tile == NULL ) return;
  for( CDynamicCoverageAll it(a_tile); it.cont(); ++it)
    if(!a_tile->getRegion(*it)->isInfiniteRegion())
      drawDart(a_tile, *it, group, fictiveGroup);
}
//------------------------------------------------------------------------------
void CScene::drawBorders(CTopologicalMap* a_map, QGraphicsItemGroup * group,
				QGraphicsItemGroup * fictiveGroup)
{
  if( a_map == NULL ) return;
  for( CDynamicCoverageAll it(a_map); it.cont(); ++it)
    if( (*it) < a_map->beta2(*it) ) //sioux
      drawEdge( a_map, *it, group, fictiveGroup );
  
  group->setHandlesChildEvents(false);
  fictiveGroup->setHandlesChildEvents(false);
}
//------------------------------------------------------------------------------
void CScene::drawLabels(CTile* a_map, QGraphicsItemGroup * group)
{
  if( a_map == NULL ) return;

  QPixmap* pixmap = new QPixmap(QSize(a_map->width(), a_map->height()));
  QPainter painter;
  QPen* pen = new QPen;
  pen->setColor(Qt::red);
  QBrush* brush = new QBrush;
  brush->setColor(Qt::green);
  painter.begin(pixmap);

  for( CDynamicCoverageAllRegions it(a_map); it.cont(); ++it)
    {
      switch(static_cast<CPyramidalRegion*>(*it)->label())
	{
	case CPyramidalRegion::Background:
	  pen->setColor(QColor(164,0,0)); //red 3
	  brush->setColor(QColor(164,0,0));
	  break;
	case CPyramidalRegion::Tissue:
	  pen->setColor(QColor(138,226,52));//green 1
	  brush->setColor(QColor(138,226,52));
	  break;
	case CPyramidalRegion::DarkTissue:
	  pen->setColor(QColor(114,159,207)); //blue 1
	  brush->setColor(QColor(114,159,207));
	  break;
	case CPyramidalRegion::BrightTissue:
	  pen->setColor(QColor(252,233,79));//yellow 1
	  brush->setColor(QColor(252,233,79));
	  break;
	case CPyramidalRegion::Lesion:
	  pen->setColor(QColor(32,74,135));//blue 3
	  brush->setColor(QColor(32,74,135));
	  break;
	case CPyramidalRegion::Cancer:
	  pen->setColor(QColor(92,53,102));//purple 3
	  brush->setColor(QColor(92,53,102));
	  break;
	case CPyramidalRegion::Stroma:
	  pen->setColor(QColor(186,189,182));//grey 3
	  brush->setColor(QColor(186,189,182));
	  break;
	case CPyramidalRegion::Mitosis:
	  pen->setColor(QColor(238,238,236));//white
	  brush->setColor(QColor(238,238,236));
	  break;
	case CPyramidalRegion::InSitu:
	  pen->setColor(QColor(92,53,102));//purple 3
	  brush->setColor(QColor(92,53,102));
	  break;
	case CPyramidalRegion::Invalid:
	  pen->setColor(QColor(46,52,54));//grey 6
	  brush->setColor(QColor(46,52,54));
	  break;
	default:
	  //qDebug() << "Warning: region label" <<  region->label() << "unsupported";
	  pen->setColor(QColor(0,0,0));
	  brush->setColor(QColor(0,0,0));
	  break;
	}
      painter.setPen(*pen);
      painter.setBrush(*brush);
      for( CTraversalRegionPixels it2(a_map, static_cast<CPyramidalRegion*>(*it)); it2.cont(); ++it2)
	{
	  painter.drawPoint(QPoint((*it2).x(), (*it2).y()));
	}
    }
  painter.end();
  QGraphicsPixmapItem * pixItem = addPixmap(*pixmap);
  if( group ) 
    group->addToGroup(pixItem); 
  group->setHandlesChildEvents(false);
  
}
//------------------------------------------------------------------------------
void CScene::drawDart( CTopologicalMap* a_map, CDart* a_dart, 
			      QGraphicsItemGroup* group,
			      QGraphicsItemGroup* fictiveGroup)
{
  CDartPath path;
  path.setDart(a_dart);
  
  CDoublet init = a_map->getDoublet(a_dart);
  CDoublet current(init);
  CDoublet next(init);
  CDoublet prev(init);

  // Pour calculer le décalage du départ,
  // on commence par récupérer le doublet précédent
  do{
    prev.setPrevLinel();
  }
  while(!a_map->isLCell(prev));
  prev.setNextPointel();

  // Puis on démarre le chemin
  QPointF startPoint = calculateGap( QPointF(current.getX(),current.getY()), 
				     prev, current);
  path.moveTo( startPoint );

  // On parcours l'arête
  do{
    // Calcul du doublet suivant
    next.setNextPointel();
    do{
      next = next.getNextLinel();
    }
    while(!a_map->isLCell(next));
    
    // On dessine si besoin en rajoutant le point au path
    if(a_map->isPCell(next) || next == init)
      {
	QPointF endPoint = calculateGap( QPointF(next.getX(),next.getY()), 
					 current, next );
	path.lineTo( endPoint );
	break;
    }
    else if( current.getLinel() != next.getLinel())
      {
	QPointF endPoint = calculateGap( QPointF(next.getX(),next.getY()), 
					 current, next );
	path.lineTo( endPoint );
	current = next;
      }
  }
  while ( 1 );

  path.addArrow(current);

  QPainterPathStroker stroker;
  stroker.setWidth(0.02);

  QPainterPath stroke = stroker.createStroke(path);

  // On ajoute le chemin dans la scene.
  QGraphicsPathItem* tmp;
  
  //tmp->setFlag(QGraphicsItem::ItemIsSelectable, true);
  //tmp->setFlag(QGraphicsItem::ItemIsMovable, true);

  if(static_cast<CPyramidalDart*>(a_dart)->isFictive())
    {
      tmp = addPath(stroke, fictiveDartPen, fictiveDartBrush);
      if(fictiveGroup)
	{
	  fictiveGroup->addToGroup(tmp);
	  fictiveGroup->setHandlesChildEvents(false);
	}
    }
  else
    {
      if( a_map->isRepresentativeDart(a_dart) )
	tmp = addPath(stroke, representativeDartPen, representativeDartBrush);
      else
	tmp = addPath(stroke, dartPen, dartBrush);
      if( group )
	{
	  group->addToGroup(tmp);
	  group->setHandlesChildEvents(false);
	}
    }
  tmp->setZValue(3);

}
//------------------------------------------------------------------------------
void CScene::drawEdge( CTopologicalMap* a_map,
			      CDart* a_dart, 
			      QGraphicsItemGroup* group, 
			      QGraphicsItemGroup* fictiveGroup )
{
  QPainterPath path;
  
  CDoublet cur = a_map->getDoublet(a_dart);
  CDoublet stop = a_map->getDoublet(a_map->beta1(a_dart));
  CDoublet next = cur;
  
  path.moveTo(cur.getX(), cur.getY());

  do{
    // Doublet suivant
    next.setNextPointel();
    do 
      {
	next.setNextLinel();
      }
    while( !a_map->isLCell( next ) );

    // Dessine le bord
    if( a_map->isPCell(next) || next == stop)
      {
	path.lineTo(next.getX(), next.getY());
	break;
      }
    else if(cur.getLinel() != next.getLinel())
      {
	path.lineTo(next.getX(), next.getY());
	cur = next;
      }
  }
  while ( 1 );
  // On ajoute le chemin dans la scene.
  QGraphicsPathItem* tmp;
  tmp = addPath(path);
  tmp->setZValue(3);
  
  if(static_cast<CPyramidalDart*>(a_dart)->isFictive() && fictiveGroup)
    fictiveGroup->addToGroup(tmp);
  else  if( group != NULL)
    group->addToGroup(tmp);
}
//------------------------------------------------------------------------------
void CScene::drawPointel( const CDoublet& a_doublet, 
					QGraphicsItemGroup* group )
{
  uint x = a_doublet.getX();
  uint y = a_doublet.getY();

  QGraphicsEllipseItem* item = addEllipse(x-0.1, y-0.1, 
					  0.2, 0.2, 
					  pointelPen, pointelBrush);
  item->setZValue(6);
  
  if( group != NULL)
    group->addToGroup(item);

  group->setHandlesChildEvents(false);
}
//------------------------------------------------------------------------------
void CScene::drawLinel( uint x, uint y, uint orientation, bool fictive,
				      QGraphicsItemGroup* group )
{
  QGraphicsRectItem* item = NULL;
  switch(orientation)
    {
    case 1: //XPOS
      if(fictive)
	item = addRect(x+0.1, y-0.05, 0.8, 0.1, linelFictivePen, linelFictiveBrush);
      else
	item = addRect(x+0.1, y-0.05, 0.8, 0.1, linelPen, linelBrush);
      break;
    case 2: //YPOS
      if(fictive)
	item = addRect(x-0.05, y+0.1, 0.1, 0.8, linelFictivePen, linelFictiveBrush);
      else
	item = addRect(x-0.05, y+0.1, 0.1, 0.8, linelPen, linelBrush);
      break;
    default:
      break;
    }

  if( group != NULL)
    group->addToGroup(item);

  group->setHandlesChildEvents(false);
}
//------------------------------------------------------------------------------
QPointF CScene::calculateGap( const QPointF & point, 
					    const CDoublet & current, 
					    const CDoublet & next )
{
  qreal gap = 0.2;
  qreal gapx = 0;
  qreal gapy = 0;
  
  if( current.getPrevLinel().getLinel() == XPOS ||
      next.getPrevLinel().getLinel() == XPOS )
    gapx = gap;
  
  else if( current.getPrevLinel().getLinel() == XNEG ||
	   next.getPrevLinel().getLinel() == XNEG )
    gapx = -gap;
  
  if( current.getPrevLinel().getLinel() == YPOS ||
      next.getPrevLinel().getLinel() == YPOS )
    gapy = gap;
  
  else if( current.getPrevLinel().getLinel() == YNEG ||
	   next.getPrevLinel().getLinel() == YNEG )
    gapy = -gap;
    
  return point + QPointF(gapx,gapy);
}
//------------------------------------------------------------------------------
void CScene::drawCurrentLevel()
{
  CPyramid * a_pyramid = controler()->pyramid();
  if(a_pyramid==NULL) return;
  
  std::deque<CTile*> levelTiles;
  std::deque<CTile*>::iterator it;
  CLevel* lvl = a_pyramid->level(level());
  lvl->image()->open();
  lvl->loadAllTiles();
  levelTiles = lvl->tiles();
  
  for(it = levelTiles.begin(); it != levelTiles.end(); ++it)
    {
      QGraphicsItemGroup* group = createItemGroup(QList<QGraphicsItem*>());
	  
      QGraphicsItemGroup* groupBorders = createItemGroup(QList<QGraphicsItem*>());
      QGraphicsItemGroup* fictiveBorders = createItemGroup(QList<QGraphicsItem*>());
      if(isBordersToDisplay())
	{
	  drawBorders(*it, groupBorders, fictiveBorders);
	}
	
      QGraphicsItemGroup* groupMap = createItemGroup(QList<QGraphicsItem*>());
      QGraphicsItemGroup* fictiveDarts = createItemGroup(QList<QGraphicsItem*>());
      if(isMapToDisplay())
	{
	  drawOpenMap(*it, groupMap, fictiveDarts);
	}
      
      QGraphicsItemGroup* groupInterpixel = createItemGroup(QList<QGraphicsItem*>());
      if(isInterpixelToDisplay())
	{
	  drawInterpixel(*it, groupInterpixel);
	}

      QGraphicsItemGroup* groupLabels = createItemGroup(QList<QGraphicsItem*>());
      if(isLabelsToDisplay())// && a_pyramid->segmentationMode()==Histology)
	{
	  drawLabels(*it, groupLabels);
	}
      
      // Regroupement par tuile
      group->addToGroup(groupBorders);
      group->addToGroup(groupMap);
      group->addToGroup(groupInterpixel);
      group->addToGroup(groupLabels);
      group->addToGroup(fictiveDarts);
      group->addToGroup(fictiveBorders);
      
      //Positionnement de la tuile
      group->moveBy((*it)->xmin(),(*it)->ymin());
      
      // Regroupement par type d'objets
      m_bordersItem->addToGroup(groupBorders);
      m_mapItem->addToGroup(groupMap);
      m_interpixelItem->addToGroup(groupInterpixel);
      m_labelsItem->addToGroup(groupLabels);
      m_fictiveBorders->addToGroup(fictiveDarts);
      m_fictiveBorders->addToGroup(fictiveBorders);
    }

  if(isImageToDisplay())
    {
      drawImage();   
    }
}
//------------------------------------------------------------------------------
void CScene::levelSelector()
{
  CPyramid * pyramid = controler()->pyramid();
  if(pyramid==NULL) return;
  
  m_levelSelector = new QSlider(Qt::Vertical);
  m_levelSelector->setPageStep(1);
  m_levelSelector->setSingleStep(1);
  m_levelSelector->setTickPosition(QSlider::TicksRight);
  m_levelSelector->setRange(0, pyramid->nbLevels()-1);
  m_levelSelector->setValue(level());
  
  connect(m_levelSelector, SIGNAL(valueChanged(int)),
	  this, SLOT(sliderChanged(int)));
}
//------------------------------------------------------------------------------
void CScene::sliderChanged(int ALevel)
{
  setLevel(ALevel);
  m_levelSelector->setValue(level());
}
//------------------------------------------------------------------------------
void CScene::addAction(QAction* action, bool value)
{
  if(value)
    m_drawToolBar->addAction(action);

  m_actions.append( action );
}
//------------------------------------------------------------------------------
QWidget* CScene::drawToolBar()
{
  return m_drawToolBar;
}
//------------------------------------------------------------------------------
QList< QAction* > CScene::actions() const
{
  return m_actions;
}

//------------------------------------------------------------------------------
/*
void CScene::drawTile(CTile* a_tile)
{
  QGraphicsItemGroup* group = createItemGroup(QList<QGraphicsItem*>());
	  
  QGraphicsItemGroup* groupBorders = createItemGroup(QList<QGraphicsItem*>());
  QGraphicsItemGroup* fictiveBorders = createItemGroup(QList<QGraphicsItem*>());
  if(isBordersToDisplay())
    {
      drawBorders(a_tile, groupBorders, fictiveBorders);
    }
	
  QGraphicsItemGroup* groupMap = createItemGroup(QList<QGraphicsItem*>());
  QGraphicsItemGroup* fictiveDarts = createItemGroup(QList<QGraphicsItem*>());
  if(isMapToDisplay())
    {
      drawOpenMap(a_tile, groupMap, fictiveDarts);
    }
      
  QGraphicsItemGroup* groupInterpixel = createItemGroup(QList<QGraphicsItem*>());
  if(isInterpixelToDisplay())
    {
      drawInterpixel(a_tile, groupInterpixel);
    }

  QGraphicsItemGroup* groupLabels = createItemGroup(QList<QGraphicsItem*>());
  if(isLabelsToDisplay())// && a_pyramid->segmentationMode()==Histology)
    {
      drawLabels(a_tile, groupLabels);
    }
      
  // Regroupement par tuile
  group->addToGroup(groupBorders);
  group->addToGroup(groupMap);
  group->addToGroup(groupInterpixel);
  group->addToGroup(groupLabels);
  group->addToGroup(fictiveDarts);
  group->addToGroup(fictiveBorders);
      
  //Positionnement de la tuile
  group->moveBy(a_tile->xmin(),a_tile->ymin());
      
  // Regroupement par type d'objets
  m_bordersItem->addToGroup(groupBorders);
  m_mapItem->addToGroup(groupMap);
  m_interpixelItem->addToGroup(groupInterpixel);
  m_labelsItem->addToGroup(groupLabels);
  m_fictiveBorders->addToGroup(fictiveDarts);
  m_fictiveBorders->addToGroup(fictiveBorders);
}
*/
