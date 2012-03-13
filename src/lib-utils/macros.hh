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
#ifndef MACROS_HH
#define MACROS_HH
//******************************************************************************

typedef unsigned int uint;
typedef unsigned int TDartId;

enum SegmentationMode {GreyValue, Thresholds, Classif, Histology, SegmentationOff};
enum ExtractMode {ConstantTileNumber, ConstantTileSize};
enum ProjectionMode {DirectProjection, DijkstraProjection, ProjectionOff};
enum FocusAttentionMode {FocusSize, FocusGrey, FocusHomogeneous, FocusHisto, FocusNone, FocusAll, FocusRed, FocusBlue};
enum DetectFictiveBordersMode {DetectionOff, DetectionOn};

#ifndef MIN
#define	MIN(x,y) (((x)<(y))? (x) : (y))
#endif
#ifndef MAX
#define	MAX(x,y) (((x)>(y))? (x) : (y))
#endif
#ifndef SQR
#define	SQR(x) ((x)*(x))
#endif
#ifndef CUB
#define	CUB(x) ((x)*(x)*(x))
#endif
#ifndef ABS
#define	ABS(x) (((x)>0)? (x) : (-1*(x)))
#endif

//******************************************************************************
#endif // MACROS_HH
//******************************************************************************
