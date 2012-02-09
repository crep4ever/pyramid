#-------------------------------------------------------------------------------
FIND_PACKAGE(PNG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib-utils)
include_directories(${PNG_INCLUDE_DIR})

add_library(utils SHARED
  math-extension.cc
  point2d.cc
  point3d.cc
  vertex.cc
  pngwriter.cc
  svg-creator.cc
  )

target_link_libraries(utils ${PNG_LIBRARY})
#-------------------------------------------------------------------------------
