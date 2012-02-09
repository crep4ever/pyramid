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
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/pyramid)
include_directories(${CMAKE_SOURCE_DIR}/lib-utils)

#-------------------------------------------------------------------------------
