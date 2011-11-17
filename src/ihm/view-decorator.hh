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
#ifndef __VIEW_DECORATOR_HH__
#define __VIEW_DECORATOR_HH__
//*****************************************************************************
#include <QWidget>
#include <QDockWidget>
//*****************************************************************************

class QBoxLayout;

/**
 * \file view-decorator.hh
 * Class for generic viewDecorators in the Qt version of the topological map.
 *
 * \brief CViewDecorator is a decorator for CView classes
 * \class CViewDecorator "view-decorator.hh"
 * \author Alexandre Dupas
 * \version 0.1
 * \date 04 June 2009
 */

class CViewDecorator : public QWidget
{
  Q_OBJECT
  
public:
  /// Constructor 
  CViewDecorator( QWidget * parent = 0 );
  
  /// Destructor
  virtual ~CViewDecorator();

  /// getter on the central widget
  QWidget * widget() const;
  
  /// setter on the central widget
  void setWidget( QWidget * widget );

signals:

public slots:
  /// add wigdet to the view according to the position parameter
  void addWidget( QWidget * widget, Qt::DockWidgetArea area );

  /// remove the widget from the view
  void removeWidget( QWidget * widget );

protected:
  QWidget * m_centralWidget;
  QBoxLayout * m_layout1;
  QBoxLayout * m_layout2;

private:

private slots:

};

#endif  // __VIEW_DECORATOR_HH__
