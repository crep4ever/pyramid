/*
 * Copyright (C) 2008-2011, Alexandre Dupas <alexandre.dupas@gmail.com>
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
#include "view.hh"
#include "scene.hh"
#include <QMatrix>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QAction>
#include <QDebug>
//******************************************************************************
CView::CView( CScene * AScene )
  : QGraphicsView( AScene )
{
  setDragMode(QGraphicsView::ScrollHandDrag);

  setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
  setActions();

  AScene->initialize( this );
}
//------------------------------------------------------------------------------
CView::~CView()
{}
//------------------------------------------------------------------------------
CScene * CView::controledScene() const
{
  return static_cast< CScene* >(scene());
}
//------------------------------------------------------------------------------
qreal CView::zoom() const
{
  return matrix().m11();
}
//------------------------------------------------------------------------------
void CView::setZoom(qreal AZoom)
{
  if (AZoom != 1.0)
    {
      scale(AZoom,AZoom);
    }
  emit(zoomChanged(zoom()));
  emit( changed(this) );
}
//------------------------------------------------------------------------------
void CView::defaultZoom()
{
  resetTransform();
  emit(zoomChanged(zoom()));
  emit( changed(this) );
}
//------------------------------------------------------------------------------
void CView::maximizeZoom()
{
  fitInView(sceneRect(), Qt::KeepAspectRatio);
  emit(zoomChanged(zoom()));
  emit( changed(this) );
}
//------------------------------------------------------------------------------
void CView::viewOrigin()
{
  centerOn(sceneRect().topLeft());
}
//------------------------------------------------------------------------------
void CView::viewCenter()
{
  centerOn(sceneRect().center());
}
//------------------------------------------------------------------------------
void CView::wheelEvent(QWheelEvent * AEvent)
{
  qreal factor = AEvent->delta() / (0.8*120.0);

  if ( AEvent->modifiers() & Qt::ControlModifier )
    {
      factor *= 5.0;
    }
  if ( factor < 0.0 )
    {
      factor = -1.0 / factor;
    }

  setZoom(factor);
  centerOn(mapToScene(AEvent->pos()));
}
//------------------------------------------------------------------------------
void CView::mouseMoveEvent(QMouseEvent * AEvent)
{
  QGraphicsView::mouseMoveEvent(AEvent);
  emit( positionChanged(mapToScene(AEvent->pos()).toPoint()) );
  emit( changed(this) );
}
//------------------------------------------------------------------------------
void CView::closeEvent(QCloseEvent * AEvent)
{
  controledScene()->writeSettings();
  AEvent->accept();
}
//------------------------------------------------------------------------------
void CView::exportPng(const QString & AFilename)
{
  int width  = (scene()->width()+1)*zoom();
  int height = (scene()->height()+1)*zoom();
  resize(width,height);
  QPixmap pixmap(width, height);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  render(&painter);
  pixmap.save(AFilename);
  painter.end();
}
//------------------------------------------------------------------------------
void CView::setActions()
{
  QAction * action = NULL;

  action = new QAction(tr("View origin"), this);
  action->setStatusTip(tr("Center the view on the origin"));
  connect(action, SIGNAL(triggered()), SLOT(viewOrigin()));
  addAction( action );

  action = new QAction(tr("View center"), this);
  action->setStatusTip(tr("Center the view on the middle"));
  connect(action, SIGNAL(triggered()), SLOT(viewCenter()));
  addAction( action );

  action = new QAction( this );
  action->setSeparator(true);
  addAction( action );

  action = new QAction(tr("Default zoom"), this);
  action->setStatusTip(tr("Restore the default zoom"));
  connect(action, SIGNAL(triggered()), SLOT(defaultZoom()));
  addAction( action );

  action = new QAction(tr("Maximize zoom"), this);
  action->setStatusTip(tr("Set the zoom such as all the scene fit in the window"));
  connect(action, SIGNAL(triggered()), SLOT(maximizeZoom()));
  addAction( action );
}
