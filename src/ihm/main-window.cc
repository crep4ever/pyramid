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
#include "main-window.hh"
#include "controler.hh"
#include "scene.hh"
#include "view.hh"
#include "view-decorator.hh"
#include "script-engine.hh"
#include "file-chooser.hh"
#include "settings-tree.hh"
#include "tab-controler.hh"
#include "panel.hh"
//******************************************************************************

CMainWindow::CMainWindow()
  : QMainWindow()
  , m_toolBar(new QToolBar)
  , m_progressBar(new QProgressBar)
  , m_zoomDisplay(new QLabel)
  , m_positionDisplay(new QLabel)
{
  setWindowTitle(tr("Tiled Top-Down Pyramid"));
  setWindowIcon(QIcon(":/icons/pyramid"));
  
  createActions();
  createMenus();
  createToolBar();
  readSettings();

  // status bar with an embedded progress bar on the right
  progressBar()->setTextVisible(false);
  progressBar()->setRange(0, 0);
  progressBar()->hide();
  statusBar()->addPermanentWidget(progressBar());
  statusBar()->addPermanentWidget( m_zoomDisplay );
  statusBar()->addPermanentWidget( m_positionDisplay );
  
  m_centralWidget = new QTabWidget;
  m_centralWidget->setTabsClosable(true);
  m_centralWidget->setMovable(true);
  
  connect( m_centralWidget, SIGNAL(currentChanged(int)),
	   SLOT(changeTab(int)) );
  
  setCentralWidget(m_centralWidget);

  //panel
  m_controler = new CControler(this);
  m_panel = new CPanel(m_controler);

  QDockWidget* dock = new QDockWidget;
  dock->setTitleBarWidget(new QWidget);
  QBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(m_panel);
  m_buildButton = new QPushButton(tr("Construction"));
  layout->addWidget(m_buildButton);
  QWidget* widget = new QWidget;
  widget->setLayout(layout);
  dock->setWidget(widget);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  connect(m_buildButton, SIGNAL(clicked()), this, SLOT(newTab()));
}
//------------------------------------------------------------------------------
CMainWindow::~CMainWindow()
{}
//------------------------------------------------------------------------------
void CMainWindow::readSettings()
{
  QSettings settings;
  settings.beginGroup("general");
  resize(settings.value("size", QSize(800,600)).toSize());
  move(settings.value("pos", QPoint(200, 200)).toPoint());
  setStatusBarDisplayed(settings.value("statusBar", true).toBool());
  setToolBarDisplayed(settings.value("toolBar", true).toBool());
  settings.endGroup();
}
//------------------------------------------------------------------------------
void CMainWindow::writeSettings()
{
  QSettings settings;
  settings.beginGroup("general");
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.setValue("statusBar", isStatusBarDisplayed());
  settings.setValue("toolBar", isToolBarDisplayed());
  settings.endGroup();
}
//------------------------------------------------------------------------------
void CMainWindow::createActions()
{
  m_newAct = new QAction(tr("New"), this);
  m_newAct->setIcon(QIcon::fromTheme("folder-new", QIcon(":/icons/folder-new")));
  m_newAct->setShortcut(QKeySequence::New);
  m_newAct->setStatusTip(tr("Create a new pyramid"));
  connect(m_newAct, SIGNAL(triggered()), this, SLOT(newTab()));

  m_openAct = new QAction(tr("Open..."), this);
  m_openAct->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/document-open")));
  m_openAct->setShortcut(QKeySequence::Open);
  m_openAct->setStatusTip(tr("Open a pyramid"));
  connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

  m_saveAct = new QAction(tr("Save"), this);
  m_saveAct->setShortcut(QKeySequence::Save);
  m_saveAct->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/document-save")));
  m_saveAct->setStatusTip(tr("Save the current pyramid"));
  connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

  m_saveAsAct = new QAction(tr("Save As..."), this);
  m_saveAsAct->setShortcut(QKeySequence::SaveAs);
  m_saveAsAct->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/document-save-as")));
  m_saveAsAct->setStatusTip(tr("Save the current pyramid with a different name"));
  connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  m_documentationAct = new QAction(tr("Documentation"), this);
  m_documentationAct->setShortcut(QKeySequence::HelpContents);
  m_documentationAct->setIcon(QIcon::fromTheme("help-contents", QIcon(":/icons/help-contents")));
  m_documentationAct->setStatusTip(tr("Documentation"));
  connect(m_documentationAct, SIGNAL(triggered()), this, SLOT(documentation()));

  m_aboutAct = new QAction(tr("&About"), this);
  m_aboutAct->setIcon(QIcon::fromTheme("help-about", QIcon(":/icons/help-browser")));
  m_aboutAct->setStatusTip(tr("About this application"));
  m_aboutAct->setMenuRole(QAction::AboutRole);
  connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  m_exitAct = new QAction(tr("Quit"), this);
  m_exitAct->setShortcut(QKeySequence::Quit);
  m_exitAct->setStatusTip(tr("Quit the program"));
  m_exitAct->setMenuRole(QAction::QuitRole);
  connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  m_preferencesAct = new QAction(tr("&Preferences"), this);
  m_preferencesAct->setStatusTip(tr("Configure the application"));
  m_preferencesAct->setMenuRole(QAction::PreferencesRole);
  connect(m_preferencesAct, SIGNAL(triggered()), SLOT(preferences()));

  QSettings settings;
  settings.beginGroup("general");
  m_toolBarViewAct = new QAction(tr("ToolBar"),this);
  m_toolBarViewAct->setStatusTip(tr("Show or hide the toolbar in the current window"));
  m_toolBarViewAct->setCheckable(true);
  m_toolBarViewAct->setChecked(settings.value("toolBar", true).toBool());
  connect(m_toolBarViewAct, SIGNAL(toggled(bool)), this, SLOT(setToolBarDisplayed(bool)));

  m_statusBarViewAct = new QAction(tr("StatusBar"),this);
  m_statusBarViewAct->setStatusTip(tr("Show or hide the statusbar in the current window"));
  m_statusBarViewAct->setCheckable(true);
  m_statusBarViewAct->setChecked(settings.value("statusBar", true).toBool());
  connect(m_statusBarViewAct, SIGNAL(toggled(bool)), this, SLOT(setStatusBarDisplayed(bool)));
  settings.endGroup();
  
  m_scriptAct = new QAction(QIcon(":/icons/text-x-generic"), tr("Load script"), this);
  m_scriptAct->setIcon(QIcon::fromTheme("text-x-generic", QIcon(":/icons/text-x-generic")));
  m_scriptAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_X) );
  m_scriptAct->setStatusTip(tr("Apply instructions from an external script."));
  connect(m_scriptAct, SIGNAL(triggered()), SLOT(loadScript()));

  m_exportPngAct = new QAction(tr("Export png"), this);
  m_exportPngAct->setIcon(QIcon::fromTheme("document-export"));
  m_exportPngAct->setStatusTip(tr("Export current scene to png."));
  connect(m_exportPngAct, SIGNAL(triggered()), SLOT(exportPng()));

  m_exportSvgAct = new QAction(tr("Export svg"), this);
  m_exportSvgAct->setIcon(QIcon::fromTheme("document-export"));
  m_exportSvgAct->setStatusTip(tr("Export current scene to svg."));
  connect(m_exportSvgAct, SIGNAL(triggered()), SLOT(exportSvg()));

  m_addViewAct = new QAction(tr("New scene"), this);
  m_addViewAct->setStatusTip(tr("Add a new representation of the same pyramid"));
  connect(m_addViewAct, SIGNAL(triggered()), this, SLOT(addScene()));

  m_tileViewAct = new QAction(tr("&Tile"), this);
  m_tileViewAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_T) );
  m_tileViewAct->setStatusTip(tr("Tile all the windows"));

  m_cascadeViewAct = new QAction(tr("&Cascade"), this);
  m_cascadeViewAct->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_C) );
  m_cascadeViewAct->setStatusTip(tr("Cascade all the windows"));
}
//------------------------------------------------------------------------------
void CMainWindow::createMenus()
{
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);

  m_fileMenu = menuBar()->addMenu(tr("&Pyramid"));
  m_fileMenu->addAction(m_newAct);
  m_fileMenu->addAction(m_openAct);
  m_fileMenu->addAction(m_saveAct);
  m_fileMenu->addAction(m_saveAsAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_exportPngAct);
  m_fileMenu->addAction(m_exportSvgAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_scriptAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_exitAct);

  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_preferencesAct);

  m_viewMenu = menuBar()->addMenu(tr("&View"));
  m_viewMenu->addAction(m_toolBarViewAct);
  m_viewMenu->addAction(m_statusBarViewAct);
  m_viewMenu->addSeparator();
  m_viewMenu->addAction(m_addViewAct);
  m_viewMenu->addAction(m_tileViewAct);
  m_viewMenu->addAction(m_cascadeViewAct);
  

  m_helpMenu = menuBar()->addMenu(tr("&Help"));
  m_helpMenu->addAction(m_documentationAct);
  m_helpMenu->addAction(m_aboutAct);
}
//------------------------------------------------------------------------------
void CMainWindow::createToolBar()
{
  m_toolBar->setMovable(false);
  m_toolBar->setContextMenuPolicy(Qt::PreventContextMenu);

  m_toolBar->addAction(m_newAct);
  m_toolBar->addAction(m_openAct);
  m_toolBar->addAction(m_saveAct);
  m_toolBar->addAction(m_saveAsAct);

  addToolBar(m_toolBar);
  setUnifiedTitleAndToolBarOnMac(true);
}
//------------------------------------------------------------------------------
void CMainWindow::closeEvent(QCloseEvent * AEvent)
{
  writeSettings();
  AEvent->accept();
}
//------------------------------------------------------------------------------
void CMainWindow::open()
{
  QString path = QFileDialog::getExistingDirectory(NULL, tr("Select New Directory"),
						   QDir::currentPath(),
						   QFileDialog::ShowDirsOnly
						   | QFileDialog::DontResolveSymlinks);
  if(path.isEmpty())
    {
      statusBar()->showMessage(tr("Specify a valid directory to open an existing pyramid"));
      return;
    }
  currentTab()->controler()->open(path);
}
//------------------------------------------------------------------------------
void CMainWindow::save()
{
  //todo
  saveAs();
}
//------------------------------------------------------------------------------
void CMainWindow::saveAs()
{

  QString path = QFileDialog::getExistingDirectory(NULL, tr("Select New Directory"),
						   QDir::currentPath(),
						   QFileDialog::ShowDirsOnly
						   | QFileDialog::DontResolveSymlinks);
  if(path.isEmpty())
    {
      statusBar()->showMessage(tr("Specify a valid directory to save the pyramid"));
      return;
    }
  currentTab()->controler()->save(path);
}
//------------------------------------------------------------------------------
void CMainWindow::documentation()
{
  QDesktopServices::openUrl(QUrl(QString("./doc/html/index.html")));
}
//------------------------------------------------------------------------------
void CMainWindow::about()
{
  QString title = QString(tr("About Tiled top-down pyramids"));
  QString version = QCoreApplication::applicationVersion();

  QString description = QString(tr("This program allows to build tiled top-down "
				   "pyramids from multi-resolution images"));

  QStringList authorsList = QStringList() << "R. Goffe";

  QString authors = authorsList.join(", ");

  QMessageBox::about(this, title, QString(tr("<p>%1</p>"
					     "<p><b>Version:</b> %2</p>"
					     "<p><b>Authors:</b> %3</p>"))
		     .arg(description).arg(version).arg(authors));
}
//------------------------------------------------------------------------------
void CMainWindow::setToolBarDisplayed(bool value)
{
  if (m_isToolBarDisplayed != value && m_toolBar)
    {
      m_isToolBarDisplayed = value;
      m_toolBar->setVisible(value);
    }
}
//------------------------------------------------------------------------------
bool CMainWindow::isToolBarDisplayed( )
{
  return m_isToolBarDisplayed;
}
//------------------------------------------------------------------------------
void CMainWindow::setStatusBarDisplayed(bool value)
{
  m_isStatusBarDisplayed = value;
  statusBar()->setVisible(value);
}
//------------------------------------------------------------------------------
bool CMainWindow::isStatusBarDisplayed( )
{
  return m_isStatusBarDisplayed;
}

//------------------------------------------------------------------------------
bool CMainWindow::loadScript()
{
  QString filename = QFileDialog::getOpenFileName(NULL,tr("Import script"), 
						  QDir::currentPath(),
						  tr("QT Scripts (*.js *.qs)"));
  
  if (filename.isEmpty())
    return false;

  CScriptEngine::engine()->execute( filename );
  return true;
}
//------------------------------------------------------------------------------
QProgressBar * CMainWindow::progressBar() const
{
  return m_progressBar;
}
//------------------------------------------------------------------------------
QTabWidget * CMainWindow::centralWidget() const
{
  return m_centralWidget;
}
//------------------------------------------------------------------------------
void CMainWindow::preferences()
{
  readSettings();
  CSettingsTree * settingsTree = new CSettingsTree;
  settingsTree->setSettingsObject(new QSettings);
   
  m_preferences = new QDialog(this);
  m_preferences->setSizeGripEnabled(true);
  
   QDialogButtonBox * buttonBox = 
     new QDialogButtonBox(QDialogButtonBox::Ok | 
			  QDialogButtonBox::Cancel);
   
   connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptSettings()));
   connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelSettings()));
   
   QPushButton * resetButton = 
     new QPushButton(QIcon(":/icons/edit-clear.png"), tr("&Clear Settings"));
   buttonBox->addButton(resetButton, QDialogButtonBox::ResetRole);
   connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSettings()));
   
   QVBoxLayout * layout = new QVBoxLayout;
   layout->addWidget(settingsTree);
   layout->addWidget(buttonBox);

   m_preferences->setLayout(layout);
   m_preferences->show();
}
//------------------------------------------------------------------------------
void CMainWindow::acceptSettings()
{
  readSettings();
  writeSettings();
  m_preferences->close();
}
//------------------------------------------------------------------------------
void CMainWindow::cancelSettings()
{
  writeSettings();
  m_preferences->close();
}
//------------------------------------------------------------------------------
void CMainWindow::resetSettings()
{
  QSettings settings;
  settings.clear();
}
//------------------------------------------------------------------------------
void CMainWindow::exportPng()
{
  if(!currentTab() || !currentTab()->view())
    {
      statusBar()->showMessage("No view to be exported");
      return;
    }
  
  QString filename = QFileDialog::getSaveFileName(NULL,"Png export", 
						  QDir::currentPath(),
						  tr("Images PNG (*.png)"));
  
  if(!filename.isEmpty())
    currentTab()->view()->exportPng(filename);
}
//------------------------------------------------------------------------------
void CMainWindow::exportSvg()
{
  if(!currentTab() || !currentTab()->controler())
    {
      statusBar()->showMessage("No scene to be exported");
      return;
    }
  currentTab()->controler()->exportSvg();
}
//------------------------------------------------------------------------------
void CMainWindow::newTab()
{
  if (centralWidget()->count() > 0)
      m_controler = new CControler(this);
      m_panel->changeControler(m_controler);

  CTabControler* tab = new CTabControler(m_controler);
  if(tab->scene())
      centralWidget()->addTab(tab, m_controler->imageFilename());
}
//------------------------------------------------------------------------------
void CMainWindow::addScene()
{
  currentTab()->addScene();

  connect( currentTab()->view(), SIGNAL(zoomChanged(qreal)),
	   this, SLOT(updateZoomDisplay(qreal)) );

  connect( currentTab()->view(), SIGNAL(positionChanged(QPoint)), 
	   this, SLOT(updatePositionDisplay(QPoint)) );
}
//------------------------------------------------------------------------------
CTabControler* CMainWindow::currentTab() const
{
  return (CTabControler*) centralWidget()->currentWidget();
}
//------------------------------------------------------------------------------
void CMainWindow::changeTab(int index)
{
  CTabControler* tab = (CTabControler*) centralWidget()->widget(index);

  disconnect(m_tileViewAct, SIGNAL(triggered()), 0, 0);
  disconnect(m_cascadeViewAct, SIGNAL(triggered()), 0, 0);

  connect(m_tileViewAct, SIGNAL(triggered()), 
	  tab, SLOT(tileSubWindows()));

  connect(m_cascadeViewAct, SIGNAL(triggered()), 
	  tab, SLOT(cascadeSubWindows()));

  disconnect(tab->view(), SIGNAL(zoomChanged(qreal)), 0, 0);
  disconnect(tab->view(), SIGNAL(positionChanged(QPoint)), 0, 0);

  connect( tab->view(), SIGNAL(zoomChanged(qreal)),
	   this, SLOT(updateZoomDisplay(qreal)) );

  connect( tab->view(), SIGNAL(positionChanged(QPoint)), 
	   this, SLOT(updatePositionDisplay(QPoint)) );

  m_panel->changeControler(tab->controler());
}
//------------------------------------------------------------------------------
void CMainWindow::updateZoomDisplay( qreal zoom )
{
  m_zoomDisplay->setText( QString("%1").arg(zoom) );
}
//------------------------------------------------------------------------------
void CMainWindow::updatePositionDisplay( QPoint position )
{
  m_positionDisplay->setText( QString("%1x%2").arg(position.x()).arg(position.y()) );
}
