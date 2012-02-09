#-------------------------------------------------------------------------------
file(
  GLOB_RECURSE
  sources_image2D
  ./*
  )

include_directories(${IMTIFF_SOURCE_DIR}/src/config)
include_directories(${IMTIFF_SOURCE_DIR}/src/core)
include_directories(${IMTIFF_SOURCE_DIR}/src/debug)
include_directories(${IMTIFF_SOURCE_DIR}/src/transform)
include_directories(${IMTIFF_SOURCE_DIR}/src/utilities)
include_directories(${Magick++_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/lib-utils)
include_directories(${CMAKE_SOURCE_DIR}/lib-kmlocal)

include_directories(
  ${GLIB2_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIR}
  ${GLIBMM_INCLUDE_LIBDIR}
  ${XMLPP_INCLUDE_DIR}
  ${XMLPP_INCLUDE_LIBDIR}
  )

add_library(image2D SHARED ${sources_image2D})

TARGET_LINK_LIBRARIES(image2D
#  ${GLIB_LIBRARIES}
#  ${GLIBMM_LIBRARY}
#  ${XMLPP_LIBRARY}
  imtiff
  utils
  kmlocal
  ${Magick++_LIBRARIES}
  )
#-------------------------------------------------------------------------------
