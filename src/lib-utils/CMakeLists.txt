#-------------------------------------------------------------------------------
file(
  GLOB_RECURSE
  sources_utils
  ./*
  )

FIND_PACKAGE(PNG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories(${CMAKE_SOURCE_DIR}/lib-utils)
include_directories(${PNG_INCLUDE_DIR})

add_library(utils
  ${sources_utils}
  )

target_link_libraries(utils ${PNG_LIBRARY})
#-------------------------------------------------------------------------------
