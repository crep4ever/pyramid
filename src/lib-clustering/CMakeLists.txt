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

include_directories(${BUILD_DIR}) #for config.hh
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories(${CMAKE_SOURCE_DIR}/lib-pyramid)
include_directories(${CMAKE_SOURCE_DIR}/lib-utils)

include_directories(${CMAKE_SOURCE_DIR}/lib-image2D)
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ImageMagick_Magick++_INCLUDE_DIRS})


add_library(clustering STATIC ${SOURCES})
target_link_libraries(clustering mapkernel pyramid)

install (TARGETS clustering DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)
