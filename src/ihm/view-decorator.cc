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

#include "view-decorator.hh"
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

CViewDecorator::CViewDecorator( QWidget * AParent )
  : QWidget( AParent )
  , m_centralWidget(NULL)
  , m_layout1(new QVBoxLayout)
  , m_layout2(new QHBoxLayout)
{
  m_layout2->addLayout( m_layout1 );

  m_layout1->setContentsMargins(0,0,0,0);
  m_layout2->setContentsMargins(0,0,0,0);

  setLayout( m_layout2 );
}

CViewDecorator::~CViewDecorator()
{}

QWidget * CViewDecorator::widget() const
{
  return m_centralWidget;
}

void CViewDecorator::setWidget( QWidget * AWidget )
{
  m_centralWidget = AWidget;
  m_layout1->addWidget( AWidget );
}

void CViewDecorator::addWidget( QWidget * AWidget, Qt::DockWidgetArea area )
{
  switch( area )
    {
    case Qt::LeftDockWidgetArea:
      m_layout2->insertWidget( 0, AWidget );
      break;
    case Qt::RightDockWidgetArea:
      m_layout2->insertWidget( -1, AWidget );
      break;
    case Qt::TopDockWidgetArea:
      m_layout1->insertWidget( 0, AWidget );
      break;
    case Qt::BottomDockWidgetArea:
      m_layout1->insertWidget( -1, AWidget );
      break;
    default:
      break;
    }
}

void CViewDecorator::removeWidget( QWidget * AWidget )
{
  m_layout1->removeWidget( AWidget );
  m_layout2->removeWidget( AWidget );
}
