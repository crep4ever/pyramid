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
#include <QDebug>
#include "main-window.hh"
#include "controler.hh"
#include "pyramid.hh"
#include "level.hh"
#include "svg-export.hh"
#include "chrono.hh"
#include "imageTiff.hh"
#include "file-chooser.hh"
using namespace Map2d;
//******************************************************************************

CControler::CControler( CMainWindow* AParent )
  : QObject()
  , m_parent(AParent)
  , m_pyramid (NULL )
  , m_nbLevels  (5)
  , m_tileWidth (512)
  , m_tileHeight(512)
  , m_extractMode( ConstantTileSize )
  , m_segmentationMode( Histology )
  , m_projectionMode( DirectProjection )
  , m_focusAttentionMode( FocusHisto )
  , m_detectFictiveBordersMode( DetectionOff )
{
  m_merge.push_back(0);
  m_merge.push_back(80);
  m_merge.push_back(40);
  m_merge.push_back(20);
  m_merge.push_back(10);
  m_merge.push_back(5);

  readSettings();
}
//------------------------------------------------------------------------------
CControler::~CControler()
{
  if( m_pyramid )
    {
      writeSettings();
      m_pyramid->image()->close();
      m_pyramid->image()->unload();
      delete m_pyramid;
      m_pyramid = NULL;
    }
}
//------------------------------------------------------------------------------
void CControler::readSettings()
{
  QSettings settings;
  settings.beginGroup("pyramid");
  m_imageFilename = settings.value("imageFilename",m_imageFilename).toString();
  m_nbLevels = settings.value("nbLevels",m_nbLevels).toUInt();
  m_tileWidth = settings.value("tileWidth",m_tileWidth).toUInt();
  m_tileHeight = settings.value("tileHeight",m_tileHeight).toUInt();
  m_extractMode = settings.value("extractMode",m_extractMode).toUInt();
  m_segmentationMode = settings.value("segmentationMode",m_segmentationMode).toUInt();
  m_projectionMode = settings.value("projectionMode",m_projectionMode).toUInt();
  m_focusAttentionMode = settings.value("focusAttentionMode",m_focusAttentionMode).toUInt();
  m_detectFictiveBordersMode = settings.value("detectFictiveBordersMode",m_detectFictiveBordersMode).toUInt();

  for(int i=1; i<m_nbLevels;++i)
    setMergeThreshold(i, settings.value(QString("mergeThreshold %1").arg(i),m_merge.at(i)).toUInt());
  
  settings.endGroup();
}
//------------------------------------------------------------------------------
void CControler::writeSettings()
{
  QSettings settings;
  settings.beginGroup("pyramid");
  settings.setValue("imageFilename",imageFilename());
  settings.setValue("nbLevels",nbLevels());
  settings.setValue("tileWidth",tileWidth());
  settings.setValue("tileHeight",tileHeight());
  settings.setValue("extractMode",extractMode());
  settings.setValue("segmentationMode",segmentationMode());
  settings.setValue("projectionMode",projectionMode());
  settings.setValue("focusAttentionMode",focusAttentionMode());
  settings.setValue("detectFictiveBordersMode",detectFictiveBordersMode());
  for(int i=1; i<m_nbLevels;++i)
    {
      if(i<m_merge.size())
	settings.setValue(QString("mergeThreshold %1").arg(i), mergeThreshold(i));
      else
	settings.setValue(QString("mergeThreshold %1").arg(i), 0);
    }
  settings.endGroup();
}
//------------------------------------------------------------------------------
CPyramid * CControler::pyramid()
{
  return m_pyramid;
}
//------------------------------------------------------------------------------
bool CControler::extract( const QString & filename )
{
  writeSettings();
  CPyramid * old = m_pyramid;
  m_pyramid = new CPyramid(filename.toStdString());
  m_imageFilename = filename.section('/', -1);
  m_pyramid->setNbLevels(m_nbLevels);

  std::vector<unsigned int> v;
  
  foreach(QVariant var, m_merge)
    {
      v.push_back(var.toUInt());
    }

  m_pyramid->setMergeThresholds(v);
  m_pyramid->setExtractMode((ExtractMode) extractMode());
  m_pyramid->setSegmentationMode((SegmentationMode) segmentationMode());
  m_pyramid->setProjectionMode((ProjectionMode) projectionMode());
  m_pyramid->setFocusAttentionMode((FocusAttentionMode) focusAttentionMode());
  m_pyramid->setDetectFictiveBordersMode((DetectFictiveBordersMode) detectFictiveBordersMode());

  m_pyramid->setTileWidth(tileWidth());
  m_pyramid->setTileHeight(tileHeight());

  if(parent())
    extractProgressBar();
  else
    m_pyramid->build();

  if( m_pyramid )
    {
      if( old )
	delete old;
      return true;
    }
  else
    {
      delete m_pyramid;
      m_pyramid = old;
      return false;
    }
}
//------------------------------------------------------------------------------
bool CControler::extract()
{
  if(m_imageFilename.isEmpty())
    {
      m_imageFilename = QFileDialog::getOpenFileName(NULL,"Open tiff file",
						 QDir::currentPath(),
						 tr("Images TIFF (*.tif *.tiff)"));
    }

  if (!m_imageFilename.isEmpty())
    {
      setImageFilename( m_imageFilename );
      extract( m_imageFilename );
      return true;
    }
  else
    {
      sendStatus(tr("No file selected for pyramid construction."));
      return false;
    }
}
//------------------------------------------------------------------------------
void CControler::open( const QString & path )
{
  // todo: normalement, pas besoin de charger
  // les paramètres comme nbLevels ou seuils de merge
  m_pyramid = new CPyramid();
  m_pyramid->setNbLevels(m_nbLevels);

  std::vector<unsigned int> v;
  
  foreach(QVariant var, m_merge)
    {
      v.push_back(var.toUInt());
    }

  m_pyramid->setMergeThresholds(v);
  m_pyramid->setTileWidth(tileWidth());
  m_pyramid->setTileHeight(tileHeight());

  m_pyramid->open(path.toStdString());
  sendStatus(tr("Pyramid successfully opened."));
}
//------------------------------------------------------------------------------
void CControler::save( const QString & path )
{
  QDir dir(imageFilename());
  m_pyramid->copyImageInWorkingDirectory(dir.absolutePath().toStdString(), 
					 imageFilename().toStdString());
  m_pyramid->save(path.toStdString());
  sendStatus(tr("Pyramid succesfully saved."));
}
//------------------------------------------------------------------------------
QString CControler::imageFilename() const
{
  return m_imageFilename;
}
//------------------------------------------------------------------------------
void CControler::setImageFilename( const QString & str)
{
  if(str != m_imageFilename)
    {
      m_imageFilename = str;
      //emit wasModified();
    }
  if (CFileChooser *fc = qobject_cast<CFileChooser *>(sender()))
    {
      if (!QFile(str).exists())
	fc->lineEdit()->setStyleSheet("border: 1px solid red;border-radius: 3px;");
      else
	fc->lineEdit()->setStyleSheet("border: 1px solid green;border-radius: 3px;");
    }
}
//------------------------------------------------------------------------------
int CControler::nbLevels() const
{
  return m_nbLevels;
}
//------------------------------------------------------------------------------
int CControler::tileWidth() const
{
  return m_tileWidth;
}
//------------------------------------------------------------------------------
int CControler::tileHeight() const
{
  return m_tileHeight;
}
//------------------------------------------------------------------------------
int CControler::extractMode() const
{
  return m_extractMode;
}
//------------------------------------------------------------------------------
int CControler::segmentationMode() const
{
  return m_segmentationMode;
}
//------------------------------------------------------------------------------
int CControler::projectionMode() const
{
  return m_projectionMode;
}
//------------------------------------------------------------------------------
int CControler::focusAttentionMode() const
{
  return m_focusAttentionMode;
}
//------------------------------------------------------------------------------
int CControler::detectFictiveBordersMode() const
{
  return m_detectFictiveBordersMode;
}
//------------------------------------------------------------------------------
int CControler::mergeThreshold(int pos)
{
  assert(pos<m_merge.size());
  return m_merge.at(pos).toUInt();
}
//------------------------------------------------------------------------------
QList<QVariant> CControler::merge() const
{
  return m_merge;
}
//------------------------------------------------------------------------------
void CControler::setNbLevels( int nbLevels )
{
  if(m_nbLevels != nbLevels)
    {
      m_nbLevels = nbLevels;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setTileWidth( int width )
{
  if(m_tileWidth != width)
    {
      m_tileWidth = width;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setTileHeight( int height )
{
  if(m_tileHeight != height)
    {
      m_tileHeight = height;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setExtractMode( int value )
{
  if(m_extractMode != value)
    {
      m_extractMode = value;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setSegmentationMode( int value )
{
  if (m_segmentationMode != value)
    {
      m_segmentationMode = value;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setProjectionMode( int value )
{
  if(m_projectionMode != value)
    {
      m_projectionMode = value;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setFocusAttentionMode( int value )
{
  if (m_focusAttentionMode != value)
    {
      m_focusAttentionMode = value;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setDetectFictiveBordersMode( int value )
{
  if(value>1) value=1;
  if (m_detectFictiveBordersMode != value)
    {
      m_detectFictiveBordersMode = value;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setMergeThreshold(int pos, int value)
{
  if (m_merge[pos] != value)
    {
      m_merge[pos] = value;
      //emit wasModified();
    }
}
//------------------------------------------------------------------------------
void CControler::setMerge(QList<QVariant>& values)
{
  m_merge.clear();
  m_merge = values;
  //emit wasModified();
}
//------------------------------------------------------------------------------
void CControler::extractProgressBar()
{
  CChrono chrono;
  parent()->progressBar()->setRange(0, m_pyramid->nbTiles());
  parent()->progressBar()->setValue(0);
  parent()->progressBar()->show();

  m_pyramid->image()->open();
  for(int k=0; k<nbLevels(); ++k)
    m_pyramid->addLevel();
  
  sendStatus(tr("Building top level..."));
  m_pyramid->level(0)->createTopLevel();
  
  uint tileCounter = 0;
  for(int k=1; k<nbLevels(); ++k)
    {
      sendStatus(QString(tr("Building level %1...")).arg(k));
      CLevel* level  = m_pyramid->level(k);

      // Parcours des tuiles du niveau précédent
      uint nbWidth  = level->levelUp()->nbTilesWidth();
      uint nbHeight = level->levelUp()->nbTilesHeight();

      level->preprocessing( (SegmentationMode) segmentationMode());
      for(uint j=0; j<nbHeight; ++j)
	for(uint i=0; i<nbWidth; ++i)
	  {
	    CPoint2D pos(i,j);
	    level->levelUp()->loadTile(pos); // Load tile up
	    switch((ExtractMode) extractMode())
	      {
	      case ConstantTileNumber:
		level->burstAndMergeTile(pos, 
					 (SegmentationMode) segmentationMode(), 
					 (ProjectionMode) projectionMode(), 
					 (FocusAttentionMode) focusAttentionMode(), 
					 (DetectFictiveBordersMode) detectFictiveBordersMode());
		break;
	      case ConstantTileSize:
		level->extractTile(pos, 
				   (SegmentationMode) segmentationMode(), 
				   (ProjectionMode) projectionMode(), 
				   (FocusAttentionMode) focusAttentionMode(), 
				   (DetectFictiveBordersMode) detectFictiveBordersMode(),
				   chrono);
		break;
	      default:
		std::cout<<"CLevel::extract error extractMode not supported"<<std::endl;
		assert(false);
	      }
	    level->levelUp()->unloadTile(pos); // Swap tile up
	    parent()->progressBar()->setValue(++tileCounter);
	    QCoreApplication::processEvents();
	  }
      level->unload();
    }
  parent()->progressBar()->hide();
  sendStatus(tr("Success"));
}
//------------------------------------------------------------------------------
uint CControler::getNbRegions(int ALevel) const
{
  if(m_pyramid==NULL)
    return 0;
  
  return m_pyramid->level(ALevel)->nbRegions();
}
//------------------------------------------------------------------------------
uint CControler::getNbDarts(int ALevel) const
{
  if(m_pyramid==NULL)
    return 0;
  
  return m_pyramid->level(ALevel)->nbDarts();
}
//------------------------------------------------------------------------------
void CControler::exportSvg()
{
  if(!m_pyramid)
    return;

  m_dialogSvg = new QDialog;
  QHBoxLayout* layout = new QHBoxLayout;

 
  QGroupBox *groupBox = new QGroupBox(tr("Pyramid Options"));
  
  m_perspective = new QCheckBox(tr("Perspective"));
  m_topology = new QCheckBox(tr("Topology"));
  m_geometry = new QCheckBox(tr("Geometry"));

  m_perspective->setChecked(Qt::Unchecked);
  m_topology->setChecked(Qt::Checked);
  m_geometry->setChecked(Qt::Checked);
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(m_perspective);
  vbox->addWidget(m_topology);
  vbox->addWidget(m_geometry);
  vbox->addStretch(1);
  groupBox->setLayout(vbox);


  QGroupBox *groupBox2 = new QGroupBox(tr("Levels Options"));
  
  m_topology2 = new QCheckBox(tr("Topology"));
  m_geometry2 = new QCheckBox(tr("Geometry"));

  m_topology2->setChecked(Qt::Checked);
  m_geometry2->setChecked(Qt::Checked);
  
  QVBoxLayout *vbox2 = new QVBoxLayout;
  vbox2->addWidget(m_topology2);
  vbox2->addWidget(m_geometry2);
  vbox2->addStretch(1);
  groupBox2->setLayout(vbox2);
  
  QDialogButtonBox * buttonBox = 
    new QDialogButtonBox(QDialogButtonBox::Cancel);

  buttonBox->setOrientation(Qt::Vertical);
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(closeSVG()));
   
  QPushButton * resetButton = 
    new QPushButton(QIcon(":/icons/edit-clear.png"), tr("Restore Defaults"));
  buttonBox->addButton(resetButton, QDialogButtonBox::ResetRole);
  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSVG()));

  QPushButton * pyramidButton = 
    new QPushButton(tr("Export Pyramid"));
  buttonBox->addButton(pyramidButton, QDialogButtonBox::AcceptRole);
  connect(pyramidButton, SIGNAL(clicked()), this, SLOT(exportPyramidSVG()));

  QPushButton * levelsButton = 
    new QPushButton(tr("Export Levels"));
  buttonBox->addButton(levelsButton, QDialogButtonBox::AcceptRole);
  connect(levelsButton, SIGNAL(clicked()), this, SLOT(exportLevelsSVG()));

  layout->addWidget(groupBox);
  layout->addWidget(groupBox2);
  layout->addWidget(buttonBox);
  m_dialogSvg->setLayout(layout);
  m_dialogSvg->show();
}
//------------------------------------------------------------------------------
void CControler::exportPyramidSVG()
{
  assert(m_pyramid!=NULL);

  CSvgExporter exporter;
  exporter.drawPyramid(m_pyramid, m_perspective->checkState(), m_topology->checkState(), m_geometry->checkState());
  sendStatus(tr("Pyramid successfully exported to svg in ./output/."));
}
//------------------------------------------------------------------------------
void CControler::exportLevelsSVG()
{
  assert(m_pyramid!=NULL);
  CSvgExporter exporter;
  exporter.drawPyramidLevels(m_pyramid, m_topology2->checkState(), m_geometry2->checkState());
  sendStatus(tr("Levels successfully exported to svg in ./output/."));
}

//------------------------------------------------------------------------------
void CControler::resetSVG()
{
  m_perspective->setChecked(Qt::Unchecked);
  m_topology->setChecked(Qt::Checked);
  m_geometry->setChecked(Qt::Checked);

  m_topology2->setChecked(Qt::Checked);
  m_geometry2->setChecked(Qt::Checked);
}
//------------------------------------------------------------------------------
void CControler::closeSVG()
{
  resetSVG();
  m_dialogSvg->close();
}
//------------------------------------------------------------------------------
CMainWindow * CControler::parent() const
{
  return m_parent;
}
//------------------------------------------------------------------------------
void CControler::setParent(CMainWindow* AParent)
{
  m_parent = AParent;
}
//------------------------------------------------------------------------------
void CControler::sendStatus(const QString & msg)
{
  parent()->statusBar()->showMessage(msg);
}
//------------------------------------------------------------------------------
void CControler::infos() const
{
  qDebug() << "Pyramid info: "
    	   << "\n image name : " << m_imageFilename
	   << "\n levels : " << m_nbLevels
	   << "\n tile width : " << m_tileWidth
	   << "\n tile height : " << m_tileHeight
    	   << "\n extract mode : " << m_extractMode
    	   << "\n segmentation mode : " << m_segmentationMode
    	   << "\n projection mode : " << m_projectionMode
    	   << "\n focus mode : " << m_focusAttentionMode
    	   << "\n fictive borders mode : " << m_detectFictiveBordersMode;

  if(!m_pyramid)
    qDebug() << tr("The controler is not associated with a given pyramid");
}
