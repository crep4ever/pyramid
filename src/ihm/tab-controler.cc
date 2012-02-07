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

#include "tab-controler.hh"
#include "view.hh"
#include "view-decorator.hh"
#include "scene.hh"
#include "controler.hh"
#include <QtGui>
#include <QDebug>

CTabControler::CTabControler(CControler * AControler)
  : QMdiArea()
  , m_controler(AControler)
  , m_scene(0)
  , m_view(0)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  if(m_controler->extract())
    addScene();
}

CTabControler::~CTabControler()
{}

CControler * CTabControler::controler() const
{
  return m_controler;
}

CScene * CTabControler::scene() const
{
  return m_scene;
}

CView * CTabControler::view() const
{
  return m_view;
}

void CTabControler::addScene()
{
  m_scene = new CScene(m_controler);
  addView(m_scene)->showMaximized();
}

QMdiSubWindow * CTabControler::addView(CScene * scene)
{
  m_view = new CView( m_scene );
  m_view->addActions(m_scene->actions());
  m_view->setContextMenuPolicy(Qt::ActionsContextMenu);

  CViewDecorator * viewDecorator = new CViewDecorator( this );
  viewDecorator->setWidget( m_view );
  viewDecorator->addWidget(m_scene->drawToolBar(), Qt::TopDockWidgetArea);
  viewDecorator->addWidget(m_scene->m_levelSelector, Qt::LeftDockWidgetArea);
  viewDecorator->show();
  return addSubWindow(viewDecorator);
}
