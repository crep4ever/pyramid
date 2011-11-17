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
  , m_imageName(QString())
  , m_imageProperties(new QListWidget)
{
  m_merge.push_back(0);
  m_merge.push_back(80);
  m_merge.push_back(40);
  m_merge.push_back(20);
  m_merge.push_back(10);
  m_merge.push_back(5);

  m_imageProperties->setAlternatingRowColors(true);
  
  readSettings();
}
//------------------------------------------------------------------------------
CControler::~CControler()
{
  if( m_pyramid != NULL )
    {
      writeSettings();
      m_pyramid->image()->close();
      m_pyramid->image()->unload();
      delete m_pyramid;
    }
}
//------------------------------------------------------------------------------
void CControler::readSettings()
{
  QSettings settings;
  
  settings.beginGroup(tr("Tiled top-down pyramid"));
  m_imageName = settings.value("imageName",m_imageName).toString();
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

  settings.beginGroup("CControler");
  settings.setValue("imageName",imageName());
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
  CPyramid * old = m_pyramid;
  m_pyramid = new CPyramid(filename.toStdString());
  m_imagePath = filename;
  m_imageName = filename.section('/', -1);
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
  writeSettings();

  if(m_imageName.isEmpty())
    {
      m_imageName = QFileDialog::getOpenFileName(NULL,"Import file", 
						 QDir::currentPath(),
						 tr("Images TIFF (*.tif *.tiff)"));
    }

  if (!m_imageName.isEmpty())
    {
      extract( m_imageName );
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
  m_pyramid->copyImageInWorkingDirectory(m_imagePath.toStdString(), m_imageName.toStdString());
  m_pyramid->save(path.toStdString());
  sendStatus(tr("Pyramid succesfully saved."));
}
//------------------------------------------------------------------------------
QString CControler::imageName() const
{
  return m_imageName;
}
//------------------------------------------------------------------------------
void CControler::setImageName( const QString & str)
{
  m_imageName = str;
  writeSettings();
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
  m_nbLevels = nbLevels;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setTileWidth( int width )
{
  m_tileWidth = width;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setTileHeight( int height )
{
  m_tileHeight = height;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setExtractMode( int value )
{
  m_extractMode = value;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setSegmentationMode( int value )
{
  qDebug() << "CControler::setSegmentation value " << value;
  m_segmentationMode = value;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setProjectionMode( int value )
{
  m_projectionMode = value;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setFocusAttentionMode( int value )
{
  //qDebug() << "CControler::setFocus value " << value;
  m_focusAttentionMode = value;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setDetectFictiveBordersMode( int value )
{
  //qDebug() << "CControler::setDetectFictiveBordersMode value " << value;
  if(value>1) value=1;
  m_detectFictiveBordersMode = value;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setMergeThreshold(int pos, int value)
{
  m_merge[pos] = value;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::setMerge(QList<QVariant>& values)
{
  m_merge.clear();
  m_merge = values;
  writeSettings();
}
//------------------------------------------------------------------------------
void CControler::extractProgressBar()
{
  CChrono chrono;
  qDebug() <<"nb Tiles =" << m_pyramid->nbTiles() ;
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
  if(m_pyramid==NULL)
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
QWidget* CControler::panel()
{
  QWidget * widget = new QWidget;
  QBoxLayout* layout = new QVBoxLayout;

  //Pyramid
  QGroupBox* pyramidGroup = new QGroupBox(tr("Tiled top-down pyramid"));
  QFormLayout *pyramidLayout = new QFormLayout;

  QSpinBox* levels = new QSpinBox;
  levels->setRange(0,10);
  levels->setValue(nbLevels());
  connect(levels, SIGNAL(valueChanged(int)), 
	  this, SLOT(setNbLevels(int)));
  pyramidLayout->addRow(new QLabel(tr("Levels:")), levels);

  QComboBox* build  = new QComboBox;
  build->addItem(tr("Constant number"));
  build->addItem(tr("Constant size"));
  build->setCurrentIndex(extractMode());
  connect(build, SIGNAL(currentIndexChanged(int)), 
	  this, SLOT(setExtractMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Pyramid of tiles:")), build);

  QComboBox* projection  = new QComboBox;
  projection->addItem(tr("Direct"));
  projection->addItem(tr("Dijkstra"));
  projection->addItem(tr("None"));
  projection->setCurrentIndex(projectionMode());
  connect(projection, SIGNAL(currentIndexChanged(int)), 
	  this, SLOT(setProjectionMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Projection:")), projection);

  QComboBox* focus  = new QComboBox;
  focus->addItem(tr("Size"));
  focus->addItem(tr("Grey"));
  focus->addItem(tr("Homogeneous"));
  focus->addItem(tr("Histology"));
  focus->addItem(tr("None"));
  focus->addItem(tr("All"));
  focus->addItem(tr("Red"));
  focus->addItem(tr("Blue"));
  focus->setCurrentIndex(focusAttentionMode());
  connect(focus, SIGNAL(currentIndexChanged(int)), 
	  this, SLOT(setFocusAttentionMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Focus of attention:")), focus);
  
  //Map
  QGroupBox* mapGroup = new QGroupBox(tr("Tiled map"));
  QFormLayout *mapLayout = new QFormLayout;

  QCheckBox* fictive = new QCheckBox;
  fictive->setChecked((bool)detectFictiveBordersMode());
  connect(fictive,  SIGNAL(stateChanged(int)), 
	  this, SLOT(setDetectFictiveBordersMode(int)));
  mapLayout->addRow(new QLabel(tr("Detect fictive borders:")), fictive);

  QComboBox* segmentation  = new QComboBox;
  segmentation->addItem(tr("Grey value"));
  segmentation->addItem(tr("Thresholds"));
  segmentation->addItem(tr("Classif"));
  segmentation->addItem(tr("Histology"));
  segmentation->addItem(tr("None"));
  segmentation->setCurrentIndex(segmentationMode());
  connect(segmentation, SIGNAL(currentIndexChanged(int)), 
	  this, SLOT(setSegmentationMode(int)));
  mapLayout->addRow(new QLabel(tr("Segmentation oracle:")), segmentation);


  //Tile
  QGroupBox* tileGroup = new QGroupBox(tr("Topological tile"));
  QFormLayout *tileLayout = new QFormLayout;

  QSpinBox* width = new QSpinBox;
  width->setRange(0,40000);
  width->setValue(tileWidth());
  connect(width, SIGNAL(valueChanged(int)), 
	  this, SLOT(setTileWidth(int)));
  tileLayout->addRow(new QLabel(tr("Width:")), width);

  QSpinBox* height = new QSpinBox;
  height->setRange(0,40000);
  height->setValue(tileHeight());
  connect(height, SIGNAL(valueChanged(int)), 
	  this, SLOT(setTileHeight(int)));
  tileLayout->addRow(new QLabel(tr("Height:")), height);


  //Image
  QGroupBox* imageGroup = new QGroupBox(tr("Image"));
  QBoxLayout * imageLayout = new QVBoxLayout;

  CFileChooser* filename = new CFileChooser();
  filename->setType(CFileChooser::OpenFileChooser);
  filename->setPath(imageName());
  if(!imageName().isEmpty())  
    tiffInfo(imageName());
  filename->setCaption(tr("Image"));
  filename->setFilter(tr("Images (*.tif)"));
  connect(filename, SIGNAL(pathChanged(const QString&)),
          this, SLOT(setImageName(const QString&)));
  connect(filename, SIGNAL(pathChanged(const QString&)),
          this, SLOT(tiffInfo(const QString&)));
  imageLayout->addWidget(filename);
  imageLayout->addWidget(m_imageProperties);

  pyramidGroup->setLayout(pyramidLayout);
  mapGroup->setLayout(mapLayout);
  tileGroup->setLayout(tileLayout);
  imageGroup->setLayout(imageLayout);

  layout->addWidget(pyramidGroup);
  layout->addWidget(mapGroup);
  layout->addWidget(tileGroup);
  layout->addWidget(imageGroup);

  widget->setLayout(layout);
  return widget;
}
//------------------------------------------------------------------------------
void CControler::tiffInfo(const QString & filename)
{
  m_imageProperties->clear();
  QList<QString> list = imageProperties();
  for(int i=0; i<list.size();++i)
    new QListWidgetItem(list[i], m_imageProperties);
}
//------------------------------------------------------------------------------
QList<QString> CControler::imageProperties()
{
  if(m_imageName.isEmpty()) 
    return QList<QString>();

  CImageTiff* image = new CImageTiff(m_imageName.toStdString());
 
  fogrimmi::TIFF_Properties tiffP = image->getProperties();
  QString name(name.fromStdString(image->fileName.Filename()));
  
  QList<QString> list;
  list.append(QString(tr("Image name : %1")).arg(name));
  list.append(QString(tr("Number of pages : %1")).arg(image->getNbPages()));
  list.append(QString(tr("Image width : %1")).arg(tiffP.width));
  list.append(QString(tr("Image height : %1")).arg(tiffP.height));
  list.append(QString(tr("Component width : %1")).arg(tiffP.componentWidth));
  list.append(QString(tr("Component height : %1")).arg(tiffP.componentHeight));
  list.append(QString(tr("BPS : %1")).arg(tiffP.bps));
  list.append(QString(tr("Color Mode : %1")).arg(tiffP.colorMode));
  list.append(QString(tr("Compression : %1")).arg(tiffP.compression));
  list.append(QString(tr("Jpeg quality : %1")).arg(tiffP.jpeg_q));
  list.append(QString(tr("Jpeg cm : %1")).arg(tiffP.jpeg_cm));
  list.append(QString(tr("Photomet : %1")).arg(tiffP.photomet));
  list.append(QString(tr("Planar : %1")).arg(tiffP.planar));
  list.append(QString(tr("Pixel organisation : %1")).arg(tiffP.pixOrganisation));
  list.append(QString(tr("Horizontal resolution : %1")).arg(tiffP.xRes));
  list.append(QString(tr("Vertical resolution : %1")).arg(tiffP.yRes));
  list.append(QString(tr("Res unit : %1")).arg(tiffP.resUnit));

  return list;
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
