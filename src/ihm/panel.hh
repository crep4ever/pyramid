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
#ifndef __PANEL_HH__
#define __PANEL_HH__
//******************************************************************************
#include <QWidget>

/** 
 * \file panel.hh
 * \class CPanel "panel.hh"  
 * \author Romain Goffe
 * \brief CPanel is a widget panel for tiled top-down pyramids preferences
 */

class CFileChooser;
class CControler;
class QListWidget;

class CPanel : public QWidget
{
  Q_OBJECT
  
  public:
  /// Constructor 
  CPanel(CControler* controler);
  
  /// Destructor
  virtual ~CPanel();
  
  void update();
  CControler* controler() const;
  void setControler(CControler *a_controler);

private slots:
  void tiffInfo(const QString & filename);

private:
  QWidget* createPyramidWidget();
  QWidget* createMapWidget();
  QWidget* createTileWidget();
  QWidget* createImageWidget();

  QStringList imageProperties();

  CControler* m_controler;  
  QListWidget* m_imageProperties;
  CFileChooser* m_imageFileChooser;
};
#endif  // __PANEL_HH__
