set(SOURCES
  criterion-split.cc
  criterion-merge.cc
  split-nbpix.cc
  split-grey.cc
  split-hole.cc
  split-deviation.cc
  #split-histo.cc
  #merge-kmeans.cc
  merge-grey.cc
)

set(HEADERS
  criterion-split.hh
  criterion-merge.hh
  split-nbpix.hh
  split-grey.hh
  split-hole.hh
  split-deviation.hh
  #split-histo.hh
  #merge-kmeans.hh
  merge-grey.hh
)

include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/pyramid)
include_directories(${CMAKE_SOURCE_DIR}/lib-utils)

add_library(clustering SHARED ${SOURCES})
install (TARGETS clustering DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)
