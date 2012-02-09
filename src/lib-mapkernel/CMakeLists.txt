add_library (mapkernel SHARED
  map/coverage.cc 
  map/dart.cc 
  map/edge-attribute.cc 
  map/map-basic.cc 
  map/map.cc 
  # 
  map-generic/map-generic.cc 
  map-generic/mg-selections.cc
  map-generic/mg-topological-infos.cc 
  #
  topological-map/coverage-region-pixels.cc
  topological-map/region.cc 
  topological-map/doublet.cc 
  topological-map/tm-extract-images.cc
  topological-map/topological-dart.cc 
  topological-map/khalimsky.cc 
  topological-map/topological-map.cc 
  #
  pyramid/dk-doublet.cc
  #pyramid/dp-maxrow.cc
  pyramid/traversal-region-pixels.cc
  pyramid/pyramid.cc 
  pyramid/level.cc 
  pyramid/pyramidal-region.cc
  pyramid/pyramidal-dart.cc 
  pyramid/tile.cc 
  pyramid/tile-swap.cc 
  pyramid/tile-extract.cc 
  pyramid/tile-projection.cc 
  pyramid/array.cc
  pyramid/tile-burst.cc
  pyramid/tile-test.cc
  pyramid/svg-export.cc
  )

include_directories(${CMAKE_SOURCE_DIR}/lib-clustering)
include_directories(${CMAKE_SOURCE_DIR}/lib-image2D)
include_directories(${CMAKE_SOURCE_DIR}/lib-svgcreator)

include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/pyramid)

include_directories(${CMAKE_SOURCE_DIR}/lib-utils)

include_directories(${IMTIFF_INCLUDE_DIR})
include_directories(${Magick++_INCLUDE_DIRS})

#-------------------------------------------------------------------------------
# Teste si le fichier stdint.h est présent sur le système. 
# Si non, on va redéfinir les quelques types qui nous sont nécessaires.
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE(stdint.h HAVE_STD_INT)
if (HAVE_STD_INT)
  ADD_DEFINITIONS(-DHAVE_STD_INT)
endif (HAVE_STD_INT)
#---------------------------------------------

TARGET_LINK_LIBRARIES(mapkernel utils clustering image2D X11 Xext) 
