#-------------------------------------------------------------------------------
add_library (mapkernel coordinate.cc vertex.cc math-extension.cc 
	     map/coverage.cc map/dart.cc map/edge-attribute.cc map/map-basic.cc 
	     map/map.cc map-generic/map-generic.cc map-generic/mg-selections.cc
	     map-generic/mg-topological-infos.cc 
	     topological-map/coverage-region-pixels.cc topological-map/region.cc 
	     topological-map/doublet.cc topological-map/tm-extract-images.cc
	     topological-map/topological-dart.cc topological-map/khalimsky.cc 
	     topological-map/topological-map.cc geometrical-attributes/dss.cc
	     geometrical-attributes/moments2D.cc
 	     topological-map/coverage-edge-linels.cc
 	     topological-map/tm-extract-images.cc
 	     topological-map/tm-segmentation.cc
 	     topological-map/tm-deformation.cc
 	     topological-map/tm-isomorphism.cc
	     )

#-------------------------------------------------------------------------------
# Teste si le fichier stdint.h est présent sur le système. 
# Si non, on va redéfinir les quelques types qui nous sont nécessaires.
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE(stdint.h HAVE_STD_INT)
if (HAVE_STD_INT)
   ADD_DEFINITIONS(-DHAVE_STD_INT)
endif (HAVE_STD_INT)

#-------------------------------------------------------------------------------
