set (NOQT_SRCS main.cc)

include_directories (${BUILD_DIR}) #for config.hh
include_directories (${CMAKE_SOURCE_DIR}/lib-clustering)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/pyramid)
include_directories (${CMAKE_SOURCE_DIR}/lib-image2D)
include_directories (${CMAKE_SOURCE_DIR}/lib-kmlocal)
include_directories (${CMAKE_SOURCE_DIR}/lib-utils)
include_directories (${IMTIFF_INCLUDE_DIR})
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ImageMagick_Magick++_INCLUDE_DIRS})

add_executable(../../noqt ${NOQT_SRCS} )
target_link_libraries(../../noqt mapkernel)
install(TARGETS ../../noqt RUNTIME DESTINATION bin)
