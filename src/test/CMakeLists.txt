#-------------------------------------------------------------------------------
set (NOQT_SRCS 
  main.cc
  )

#-------------------------------------------------------------------------------

include_directories (${CMAKE_SOURCE_DIR}/lib-clustering)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/pyramid)
include_directories (${CMAKE_SOURCE_DIR}/lib-image2D)
include_directories (${CMAKE_SOURCE_DIR}/lib-kmlocal)
include_directories(${IMTIFF_SOURCE_DIR}/src/config)
include_directories(${IMTIFF_SOURCE_DIR}/src/core)
include_directories(${IMTIFF_SOURCE_DIR}/src/debug)
include_directories(${IMTIFF_SOURCE_DIR}/src/transform)
include_directories(${IMTIFF_SOURCE_DIR}/src/utilities)
include_directories(${Magick++_INCLUDE_DIRS})
include_directories (${PANDORE_INCLUDE_DIR})
include_directories (${CMAKE_SOURCE_DIR}/lib-utils)

#-------------------------------------------------------------------------------
add_executable(../../noqt ${NOQT_SRCS} )
target_link_libraries(../../noqt mapkernel)
#-------------------------------------------------------------------------------
