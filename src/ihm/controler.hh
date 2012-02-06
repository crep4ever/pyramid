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
#ifndef __CONTROLER_HH__
#define __CONTROLER_HH__
//******************************************************************************
#include <QObject>
#include <QString>
#include <QMultiMap>
#include <QSet>
#include <QDockWidget>
#include <QList>
#include <QVector>
#include <QVariant>
//******************************************************************************

namespace pyramid
{
  class CPyramid;
  class CTile;
}

class QAction;
class QProgressBar;
class QDialog;
class QCheckBox;
class QRadioButton;
class QListWidget;
class CMainWindow;
class CFileChooser;

/**
 * \file pyramid.hh
 * \class CControler "controler.hh"
 * \author Romain Goffe
 * \brief CControlerPyramid is a controler for tiled top-down pyramids
 */

class CControler : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString imageFilename
	     READ imageFilename
	     WRITE setImageFilename)

    Q_PROPERTY(int nbLevels
	       READ nbLevels
	       WRITE setNbLevels)

    Q_PROPERTY(QList<QVariant> merge
	       READ merge
	       WRITE setMerge)

    Q_PROPERTY(int tileWidth
	       READ tileWidth
	       WRITE setTileWidth)

    Q_PROPERTY(int tileHeight
	       READ tileHeight
	       WRITE setTileHeight)

    Q_PROPERTY(int extractMode
	       READ extractMode
	       WRITE setExtractMode)

    Q_PROPERTY(int segmentationMode
	       READ segmentationMode
	       WRITE setSegmentationMode)

    Q_PROPERTY(int projectionMode
	       READ projectionMode
	       WRITE setProjectionMode)

    Q_PROPERTY(int focusAttentionMode
	       READ focusAttentionMode
	       WRITE setFocusAttentionMode)

    Q_PROPERTY(int detectFictiveBordersMode
	       READ detectFictiveBordersMode
	       WRITE setDetectFictiveBordersMode)

    public:
  /// Constructor
    CControler( CMainWindow * parent );

  /// Destructor
  virtual ~CControler();

  /// read settings of the scene
  virtual void readSettings();

  /// getter on the controler's pyramid
  pyramid::CPyramid * pyramid();

  /// getter on the nbLevels property
  int nbLevels() const;

  /// getter on the tileWidth property
  int tileWidth() const;

  /// getter on the tileHeight property
  int tileHeight() const;

  /// getter on the extractMode property
  int extractMode() const;

  /// getter on the segmentationMode property
  int segmentationMode() const;

  /// getter on the merge threshold property
  int mergeThreshold(int pos);
  QList<QVariant> merge() const;

public slots:

  /// write settings of the scene
  virtual void writeSettings();

  void sendStatus(const QString & str);

  CMainWindow* parent() const;
  void setParent(CMainWindow* AParent);

  /// setter on the nbLevels property
  void setNbLevels(int nbLevels);

  /// setter on the tileWidth property
  void setTileWidth(int width);

  /// setter on the tileHeight property
  void setTileHeight(int height);

  /// setter on the extractMode property
  void setExtractMode(int value);

  /// setter on the segmentationMode property
  void setSegmentationMode(int value);

  /// getter on the projectionMode property
  int projectionMode() const;

  /// setter on the projectionMode property
  void setProjectionMode(int value);

  /// getter on the focus of attention property
  int focusAttentionMode() const;

  /// setter on the focus of attention property
  void setFocusAttentionMode(int value);

  /// getter on the focus of attention property
  int detectFictiveBordersMode() const;

  /// setter on the focus of attention property
  void setDetectFictiveBordersMode(int value);

  /// setter on the mergeThreshold property
  void setMergeThreshold(int pos, int value);
  void setMerge(QList<QVariant>& values);

  void extractProgressBar();

  /// getter on the image name property
  QString imageFilename() const;

  /// setter on the image name property
  void setImageFilename(const QString & str);

  /// Nombre de régions dans un niveau de la pyramide
  /// @param ALevel: le niveau
  uint getNbRegions(int ALevel) const;

  /// Nombre de brins dans un niveau de la pyramide
  /// @param ALevel: le niveau
  uint getNbDarts(int ALevel) const;

  /// extraction of a pyramid  (asking for a file)
  bool extract( const QString & filename );

  /// extraction of a pyramid
  bool extract();

  /// open an existing pyramid
  void open(const QString & path);

  /// save the current pyramid
  void save(const QString & path);

  /// Draw a pyramid for svg export
  //void drawImage();


  /// svg export of the pyramid
  void exportPyramidSVG();
  void exportLevelsSVG();
  void exportSvg();
  void resetSVG();
  void closeSVG();

signals:
  void wasModified();

private:
  void infos() const;

  CMainWindow* m_parent;
  pyramid::CPyramid* m_pyramid;

  int  m_nbLevels;
  int  m_tileWidth;
  int  m_tileHeight;
  int  m_extractMode;
  int  m_segmentationMode;
  int  m_projectionMode;
  int  m_focusAttentionMode;
  int  m_detectFictiveBordersMode;
  QString m_imageFilename;
  QList<QVariant> m_merge;

  QProgressBar* m_progressBar;

  //export svg
  QDialog*   m_dialogSvg;
  QCheckBox* m_perspective;
  QCheckBox* m_topology;
  QCheckBox* m_geometry;
  QCheckBox* m_topology2;
  QCheckBox* m_geometry2;
};
#endif  // __CONTROLER_HH__
