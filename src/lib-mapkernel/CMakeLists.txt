set(SOURCES
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

set(HEADERS
  map/coverage.hh
  map/static-coverage.hh
  map/dynamic-coverage.hh
  map/vertex-coverage.hh
  map/dart.hh 
  map/edge-attribute.hh 
  map/map-basic.hh 
  map/map.hh
  map/kernel-types.hh
  # 
  map-generic/map-generic.hh 
  #
  topological-map/coverage-region-pixels.hh
  topological-map/region.hh 
  topological-map/doublet.hh 
  topological-map/orders.hh 
  topological-map/tm-extract-images.hh
  topological-map/tm-deformation.hh
  topological-map/topological-dart.hh 
  topological-map/topological-map.hh 
  topological-map/khalimsky.hh 
  #
  pyramid/dk-doublet.hh
  pyramid/traversal-region-pixels.hh
  pyramid/pyramid.hh 
  pyramid/level.hh 
  pyramid/pyramidal-region.hh
  pyramid/pyramidal-dart.hh 
  pyramid/tile.hh 
  pyramid/array.hh
  pyramid/svg-export.hh
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

# If file stdint.h does not exist, define some types
include(CheckIncludeFile)
check_include_file(stdint.h HAVE_STD_INT)
if (HAVE_STD_INT)
  add_definitions(-DHAVE_STD_INT)
endif (HAVE_STD_INT)

add_library (mapkernel SHARED ${SOURCES})
target_link_libraries(mapkernel utils clustering image2D X11 Xext) 
install (TARGETS mapkernel DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)

