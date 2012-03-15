set(SOURCES
  dk-doublet.cc
  pyramid.cc
  level.cc
  pyramidal-region.cc
  pyramidal-dart.cc
  tile.cc
  tile-swap.cc
  tile-extract.cc
  tile-projection.cc
  array.cc
  tile-burst.cc
  tile-test.cc
  svg-export.cc

  image/imageTiff.cc
  image/omp_kmeans.cc
  )

set(HEADERS
  dk-doublet.hh
  pyramid.hh
  level.hh
  pyramidal-region.hh
  pyramidal-dart.hh
  tile.hh
  array.hh
  svg-export.hh
  image/kmeans.h
  image/imageTiff.hh
  )

include_directories(${BUILD_DIR}) #for config.hh
include_directories(${CMAKE_SOURCE_DIR}/lib-clustering)
include_directories(${CMAKE_SOURCE_DIR}/lib-kmlocal)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories(${CMAKE_SOURCE_DIR}/lib-pyramid)

include_directories(${CMAKE_SOURCE_DIR}/lib-utils)

include_directories(${IMTIFF_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib-image2D)
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ImageMagick_Magick++_INCLUDE_DIRS})

add_library (pyramid STATIC ${SOURCES})
target_link_libraries(pyramid mapkernel utils clustering image2D ${IMTIFF_LIBRARY} kmlocal X11 Xext)
install (TARGETS pyramid DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)
