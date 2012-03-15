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
#include "image/imageTiff.hh"

CPanel::CPanel( CControler *a_controler )
  : QWidget()
  , m_controler(a_controler)
  , m_imageProperties(new QListWidget)
  , m_imageFileChooser(new CFileChooser())
{
  m_imageProperties->setAlternatingRowColors(true);

  QBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(createPyramidWidget());
  layout->addWidget(createMapWidget());
  layout->addWidget(createTileWidget());
  layout->addWidget(createImageWidget());
  setLayout(layout);
}

CPanel::~CPanel()
{
  delete m_controler;
  m_controler = 0;
}

void CPanel::update()
{}

QWidget* CPanel::createPyramidWidget()
{
  QGroupBox* pyramidGroup = new QGroupBox(tr("Tiled top-down pyramid"));
  QFormLayout *pyramidLayout = new QFormLayout;

  m_levels = new QSpinBox;
  m_levels->setRange(2,10);
  m_levels->setValue(controler()->nbLevels());
  connect(m_levels, SIGNAL(valueChanged(int)),
	  controler(), SLOT(setNbLevels(int)));
  pyramidLayout->addRow(new QLabel(tr("Levels:")), m_levels);

  m_build  = new QComboBox;
  m_build->addItem(tr("Constant number"));
  m_build->addItem(tr("Constant size"));
  m_build->setCurrentIndex(controler()->extractMode());
  connect(m_build, SIGNAL(currentIndexChanged(int)),
	  controler(), SLOT(setExtractMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Pyramid of tiles:")), m_build);

  m_projection  = new QComboBox;
  m_projection->addItem(tr("Direct"));
  m_projection->addItem(tr("Dijkstra"));
  m_projection->addItem(tr("None"));
  m_projection->setCurrentIndex(controler()->projectionMode());
  connect(m_projection, SIGNAL(currentIndexChanged(int)),
	  controler(), SLOT(setProjectionMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Projection:")), m_projection);

  m_focus  = new QComboBox;
  m_focus->addItem(tr("Size"));
  m_focus->addItem(tr("Grey"));
  m_focus->addItem(tr("Homogeneous"));
  m_focus->addItem(tr("Histology"));
  m_focus->addItem(tr("None"));
  m_focus->addItem(tr("All"));
  m_focus->addItem(tr("Red"));
  m_focus->addItem(tr("Blue"));
  m_focus->setCurrentIndex(controler()->focusAttentionMode());
  connect(m_focus, SIGNAL(currentIndexChanged(int)),
	  controler(), SLOT(setFocusAttentionMode(int)));
  pyramidLayout->addRow(new QLabel(tr("Focus of attention:")), m_focus);
  pyramidGroup->setLayout(pyramidLayout);
  return pyramidGroup;
}

QWidget* CPanel::createMapWidget()
{
  QGroupBox* mapGroup = new QGroupBox(tr("Tiled map"));
  QFormLayout *mapLayout = new QFormLayout;

  m_fictive = new QCheckBox;
  m_fictive->setChecked((bool)controler()->detectFictiveBordersMode());
  connect(m_fictive,  SIGNAL(stateChanged(int)),
	  controler(), SLOT(setDetectFictiveBordersMode(int)));
  mapLayout->addRow(new QLabel(tr("Detect fictive borders:")), m_fictive);

  m_segmentation  = new QComboBox;
  m_segmentation->addItem(tr("Grey value"));
  m_segmentation->addItem(tr("Thresholds"));
  m_segmentation->addItem(tr("Classif"));
  m_segmentation->addItem(tr("Histology"));
  m_segmentation->addItem(tr("None"));
  m_segmentation->setCurrentIndex(controler()->segmentationMode());
  connect(m_segmentation, SIGNAL(currentIndexChanged(int)),
	  controler(), SLOT(setSegmentationMode(int)));
  mapLayout->addRow(new QLabel(tr("Segmentation oracle:")), m_segmentation);
  mapGroup->setLayout(mapLayout);
  return mapGroup;
}

QWidget* CPanel::createTileWidget()
{
  QGroupBox* tileGroup = new QGroupBox(tr("Topological tile"));
  QFormLayout *tileLayout = new QFormLayout;

  m_width = new QSpinBox;
  m_width->setRange(0,40000);
  m_width->setValue(controler()->tileWidth());
  connect(m_width, SIGNAL(valueChanged(int)),
	  controler(), SLOT(setTileWidth(int)));
  tileLayout->addRow(new QLabel(tr("Width:")), m_width);

  m_height = new QSpinBox;
  m_height->setRange(0,40000);
  m_height->setValue(controler()->tileHeight());
  connect(m_height, SIGNAL(valueChanged(int)),
	  controler(), SLOT(setTileHeight(int)));
  tileLayout->addRow(new QLabel(tr("Height:")), m_height);
  tileGroup->setLayout(tileLayout);
  return tileGroup;
}

QWidget* CPanel::createImageWidget()
{
  QGroupBox* imageGroup = new QGroupBox(tr("Image"));
  QBoxLayout * imageLayout = new QVBoxLayout;

  m_imageFileChooser->setType(CFileChooser::OpenFileChooser);
  m_imageFileChooser->setPath(controler()->imageFilename());
  if(!controler()->imageFilename().isEmpty())
    tiffInfo(controler()->imageFilename());
  m_imageFileChooser->setCaption(tr("Image"));
  m_imageFileChooser->setFilter(tr("Images (*.tif)"));
  connect(m_imageFileChooser, SIGNAL(pathChanged(const QString&)),
          controler(), SLOT(setImageFilename(const QString&)));
  connect(m_imageFileChooser, SIGNAL(pathChanged(const QString&)),
          this, SLOT(tiffInfo(const QString&)));

  imageLayout->addWidget(m_imageFileChooser);
  imageLayout->addWidget(m_imageProperties);
  imageGroup->setLayout(imageLayout);
  return imageGroup;
}

void CPanel::tiffInfo(const QString & filename)
{
  m_imageProperties->clear();
  QString property;
  QStringList list = imageProperties();
  foreach(property, list)
    m_imageProperties->addItem(new QListWidgetItem(property));
}

QStringList CPanel::imageProperties()
{
  if(!controler()
     || controler()->imageFilename().isEmpty()
     || !QFile(controler()->imageFilename()).exists())
    return QStringList();

  CImageTiff* image = new CImageTiff(controler()->imageFilename().toStdString());
  if(!image)
    {
      qWarning() << "CPanel::imageProperties unable to open image " << controler()->imageFilename();
      return QStringList();
    }

  fogrimmi::TIFF_Properties tiffP = image->getProperties();
  QString name(name.fromStdString(image->fileName.Filename()));

  return QStringList()
    << QString(tr("Image name : %1")).arg(name)
    << QString(tr("Number of pages : %1")).arg(image->getNbPages())
    << QString(tr("Image width : %1")).arg(tiffP.width)
    << QString(tr("Image height : %1")).arg(tiffP.height)
    << QString(tr("Component width : %1")).arg(tiffP.componentWidth)
    << QString(tr("Component height : %1")).arg(tiffP.componentHeight)
    << QString(tr("BPS : %1")).arg(tiffP.bps)
    << QString(tr("Color Mode : %1")).arg(tiffP.colorMode)
    << QString(tr("Compression : %1")).arg(tiffP.compression)
    << QString(tr("Jpeg quality : %1")).arg(tiffP.jpeg_q)
    << QString(tr("Jpeg cm : %1")).arg(tiffP.jpeg_cm)
    << QString(tr("Photomet : %1")).arg(tiffP.photomet)
    << QString(tr("Planar : %1")).arg(tiffP.planar)
    << QString(tr("Pixel organisation : %1")).arg(tiffP.pixOrganisation)
    << QString(tr("Horizontal resolution : %1")).arg(tiffP.xRes)
    << QString(tr("Vertical resolution : %1")).arg(tiffP.yRes)
    << QString(tr("Res unit : %1")).arg(tiffP.resUnit);
}

CControler* CPanel::controler() const
{
  return m_controler;
}

void CPanel::changeControler(CControler* a_controler)
{
  m_controler = a_controler;

  controler()->setNbLevels(m_levels->value());
  controler()->setExtractMode(m_build->currentIndex());
  controler()->setProjectionMode(m_projection->currentIndex());
  controler()->setFocusAttentionMode(m_focus->currentIndex());

  controler()->setDetectFictiveBordersMode((int) m_fictive->isChecked());
  controler()->setSegmentationMode(m_segmentation->currentIndex());

  controler()->setTileWidth(m_width->value());
  controler()->setTileHeight(m_height->value());

  controler()->setImageFilename(m_imageFileChooser->path());

  update();
}
