find_package(PNG REQUIRED)

set(SOURCES
  math-extension.cc
  point2d.cc
  point3d.cc
  vertex.cc
  pngwriter.cc
  svg-creator.cc
  )

set(HEADERS
  math-extension.hh
  point2d.hh
  point3d.hh
  vertex.hh
  pngwriter.h
  svg-creator.hh
  volume.hh
  matrix.hh
  chrono.hh
  macros.hh
  )

include_directories(${CMAKE_SOURCE_DIR}/lib-utils)
include_directories(${PNG_INCLUDE_DIR})

add_library(utils SHARED ${SOURCES})
target_link_libraries(utils ${PNG_LIBRARY})
install (TARGETS utils DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)
