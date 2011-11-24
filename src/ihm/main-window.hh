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
#ifndef __MAIN_WINDOW_HH__
#define __MAIN_WINDOW_HH__
//******************************************************************************
#include <QtGui>
//******************************************************************************

/** 
 * \class CMainWindow "mainWindow.hh"
 * \brief CMainWindow is the base class of the application
 */

class CTabControler;
class CControler;
class CView;
class CPanel;

class CMainWindow : public QMainWindow
{
  Q_OBJECT
  
  public:
  /// Constructor 
  CMainWindow();
  
  /// Destructor
  ~CMainWindow();

  /// import settings
  void readSettings();

  /// export settings
  void writeSettings();

  QProgressBar* progressBar() const;
  QTabWidget* centralWidget() const;
  CTabControler* currentTab() const;

public slots:

  void open();
  void save();
  void saveAs();
  void documentation();
  void about();
  void preferences();

  bool isToolBarDisplayed();
  void setToolBarDisplayed(bool value);

  bool isStatusBarDisplayed();
  void setStatusBarDisplayed(bool value);

  void exportPng();
  void exportSvg();

  void addScene();

  void acceptSettings();
  void cancelSettings();
  void resetSettings();

protected:
  /// on close
  virtual void closeEvent(QCloseEvent * event);

protected slots:
  bool loadScript();

private slots:
  /// add new tab corresponding to a new controler
  void newTab();

  /// update conections when changing tab
  void changeTab(int index);

  /// update zoom factor display
  void updateZoomDisplay( qreal zoom );

  /// update cursor position display
  void updatePositionDisplay( QPoint position );


private:
  /// defines actions of the main window
  void createActions();
  void createMenus();
  void createToolBar();

  QTabWidget* m_centralWidget;  
  QToolBar* m_toolBar;
  QProgressBar* m_progressBar;
  QLabel * m_zoomDisplay;
  QLabel * m_positionDisplay;

  bool m_isToolBarDisplayed;
  bool m_isStatusBarDisplayed;

  // Application actions
  QAction *m_preferencesAct;
  QAction *m_toolBarViewAct;
  QAction *m_statusBarViewAct;
  QAction *m_documentationAct;
  QAction *m_scriptAct;
  QAction *m_aboutAct;
  QAction *m_exitAct;
  QAction *m_addViewAct;
  QAction *m_tileViewAct; 
  QAction *m_cascadeViewAct; 

  // Pyramid actions
  QAction *m_newAct;
  QAction *m_openAct;
  QAction *m_saveAct;
  QAction *m_saveAsAct;
  QAction *m_exportPngAct;
  QAction *m_exportSvgAct;

  // Menus
  QMenu* m_fileMenu;
  QMenu* m_editMenu;
  QMenu* m_viewMenu;
  QMenu* m_helpMenu;

  QPushButton* m_buildButton;
  QDialog* m_preferences;
  CPanel* m_panel;
  CControler* m_controler; // pyramid controler pf the current tab
};

#endif  // __MAIN_WINDOW_HH__
