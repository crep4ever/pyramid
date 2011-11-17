#-------------------------------------------------------------------------------
add_library (clustering 
  SHARED
  criterion-split.cc 
  split-nbpix.cc
  split-grey.cc 
  split-hole.cc
  split-deviation.cc
#  split-histo.cc
  criterion-merge.cc 
  merge-grey.cc
#  merge-kmeans.cc
  )

include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/pyramid)
include_directories(${CMAKE_SOURCE_DIR}/lib-image2D)
include_directories(${IMTIFF_SOURCE_DIR}/src/config)
include_directories(${IMTIFF_SOURCE_DIR}/src/core)
include_directories(${IMTIFF_SOURCE_DIR}/src/debug)
include_directories(${IMTIFF_SOURCE_DIR}/src/transform)
include_directories(${IMTIFF_SOURCE_DIR}/src/utilities)
include_directories(${Magick++_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/lib-utils)

#-------------------------------------------------------------------------------
