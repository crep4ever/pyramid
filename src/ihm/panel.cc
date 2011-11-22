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

#include <QtGui>
#include "panel.hh"
#include "file-chooser.hh"
#include "controler.hh"
#include "imageTiff.hh"

CPanel::CPanel( CControler *a_controler )
  : QWidget()
  , m_controler(a_controler)
  , m_imageName(QString())
  , m_imageProperties(new QListWidget)
  , m_imageFileChooser(new CFileChooser())
{
  m_imageProperties->setAlternatingRowColors(true);

  QBoxLayout* layout = new QVBoxLayout;

  //Pyramid
  QGroupBox* pyramidGroup = new QGroupBox(tr("Tiled top-down pyramid"));
  QFormLayout *pyramidLayout = new QFormLayout;

  QSpinBox* levels = new QSpinBox;
  levels->setRange(0,10);
  levels->setValue(controler()->nbLevels());
  connect(levels, SIGNAL(valueChanged(int)), 
	  controler(), SLOT(setNbLevels(int)));
  pyramidLayout->addRow(new QLabel(tr("Levels:")), levels);

  QComboBox* build  = new QComboBox;
  build->addItem(tr("Constant number"));
  build->addItem(tr("Constant size"));
  build->setCurrentIndex(controler()->extractMode());
  connect(build, SIGNAL(currentIndexChanged(int)), 
	  controler(), SLOT(setExtractMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Pyramid of tiles:")), build);

  QComboBox* projection  = new QComboBox;
  projection->addItem(tr("Direct"));
  projection->addItem(tr("Dijkstra"));
  projection->addItem(tr("None"));
  projection->setCurrentIndex(controler()->projectionMode());
  connect(projection, SIGNAL(currentIndexChanged(int)), 
	  controler(), SLOT(setProjectionMode(int)));
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
  focus->setCurrentIndex(controler()->focusAttentionMode());
  connect(focus, SIGNAL(currentIndexChanged(int)), 
	  controler(), SLOT(setFocusAttentionMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Focus of attention:")), focus);
  
  //Map
  QGroupBox* mapGroup = new QGroupBox(tr("Tiled map"));
  QFormLayout *mapLayout = new QFormLayout;

  QCheckBox* fictive = new QCheckBox;
  fictive->setChecked((bool)controler()->detectFictiveBordersMode());
  connect(fictive,  SIGNAL(stateChanged(int)), 
	  controler(), SLOT(setDetectFictiveBordersMode(int)));
  mapLayout->addRow(new QLabel(tr("Detect fictive borders:")), fictive);

  QComboBox* segmentation  = new QComboBox;
  segmentation->addItem(tr("Grey value"));
  segmentation->addItem(tr("Thresholds"));
  segmentation->addItem(tr("Classif"));
  segmentation->addItem(tr("Histology"));
  segmentation->addItem(tr("None"));
  segmentation->setCurrentIndex(controler()->segmentationMode());
  connect(segmentation, SIGNAL(currentIndexChanged(int)), 
	  controler(), SLOT(setSegmentationMode(int)));
  mapLayout->addRow(new QLabel(tr("Segmentation oracle:")), segmentation);


  //Tile
  QGroupBox* tileGroup = new QGroupBox(tr("Topological tile"));
  QFormLayout *tileLayout = new QFormLayout;

  QSpinBox* width = new QSpinBox;
  width->setRange(0,40000);
  width->setValue(controler()->tileWidth());
  connect(width, SIGNAL(valueChanged(int)), 
	  controler(), SLOT(setTileWidth(int)));
  tileLayout->addRow(new QLabel(tr("Width:")), width);

  QSpinBox* height = new QSpinBox;
  height->setRange(0,40000);
  height->setValue(controler()->tileHeight());
  connect(height, SIGNAL(valueChanged(int)), 
	  controler(), SLOT(setTileHeight(int)));
  tileLayout->addRow(new QLabel(tr("Height:")), height);


  //Image
  QGroupBox* imageGroup = new QGroupBox(tr("Image"));
  QBoxLayout * imageLayout = new QVBoxLayout;

  m_imageFileChooser->setType(CFileChooser::OpenFileChooser);
  m_imageFileChooser->setPath(controler()->imageName());
  if(!controler()->imageName().isEmpty())  
    tiffInfo(controler()->imageName());
  m_imageFileChooser->setCaption(tr("Image"));
  m_imageFileChooser->setFilter(tr("Images (*.tif)"));
  connect(m_imageFileChooser, SIGNAL(pathChanged(const QString&)),
          controler(), SLOT(setImageName(const QString&)));
  connect(m_imageFileChooser, SIGNAL(pathChanged(const QString&)),
          this, SLOT(tiffInfo(const QString&)));

  imageLayout->addWidget(m_imageFileChooser);
  imageLayout->addWidget(m_imageProperties);

  pyramidGroup->setLayout(pyramidLayout);
  mapGroup->setLayout(mapLayout);
  tileGroup->setLayout(tileLayout);
  imageGroup->setLayout(imageLayout);

  layout->addWidget(pyramidGroup);
  layout->addWidget(mapGroup);
  layout->addWidget(tileGroup);
  layout->addWidget(imageGroup);

  setLayout(layout);
}

CPanel::~CPanel()
{}

void CPanel::update()
{
  m_imageFileChooser->setPath(controler()->imageName());
}

void CPanel::tiffInfo(const QString & filename)
{
  m_imageProperties->clear();
  QList<QString> list = imageProperties();
  for(int i=0; i<list.size();++i)
    new QListWidgetItem(list[i], m_imageProperties);
}

QList<QString> CPanel::imageProperties()
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

CControler* CPanel::controler() const
{
  return m_controler;
}

void CPanel::setControler(CControler* a_controler)
{
  m_controler = a_controler;
  update();
}
