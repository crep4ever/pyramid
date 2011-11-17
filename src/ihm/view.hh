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

//*****************************************************************************
#ifndef __VIEW_HH__
#define __VIEW_HH__
//*****************************************************************************
#include <QGraphicsView>
#include <QString>
//*****************************************************************************

class CScene;

/**
 * \file view.hh
 * 
 * Class for generic views in the Qt version of the topological map.
 *
 * \class CView "view.hh"
 * \brief CView is a generic class for viewing in the application.
 * \author Alexandre Dupas
 * \version 0.1
 * \date 04 June 2009
 */

class CView : public QGraphicsView
{
  Q_OBJECT
  
public:
  /// Constructor 
  CView( CScene * scene );
  
  /// Destructor
  virtual ~CView();
  
  /// getter on the controled scene
  CScene * controledScene() const;
  
  /// getter on the current zoom factor
  qreal zoom() const;
  
signals:
  /// zoom change
  void zoomChanged(qreal zoom);

  /// position change
  void positionChanged(QPoint position);

  /// position change
  void changed(CView*);

public slots:
  /// setter on the zoom factor
  void setZoom(qreal zoom);

  /// restore the default zoom
  void defaultZoom();

  /// maximize the scene into the window
  void maximizeZoom();

  /// center view on the origin
  void viewOrigin();

  /// center view on the center of the displayed figure
  void viewCenter();

  //void itemSelected(QGraphicsItem *item);

  // save the view as a png image
  void exportPng(const QString & AFilename );


protected:
  /// on mouse wheel
  virtual void wheelEvent(QWheelEvent * event);

  /// on mouse move
  virtual void mouseMoveEvent(QMouseEvent * event);

  /// on close
  virtual void closeEvent(QCloseEvent * event);

private:
  qreal m_zoom;

  /// set all the actions provided by the scene
  void setActions();

private slots:

};

#endif  // __VIEW_HH__
