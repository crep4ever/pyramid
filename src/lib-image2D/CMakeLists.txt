#-------------------------------------------------------------------------------
file(
  GLOB_RECURSE
  sources_image2D
  ./*
  )

include_directories(${IMTIFF_INCLUDE_DIR})
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
  utils
  kmlocal
  ${IMTIFF_LIBRARY}
  ${Magick++_LIBRARIES}
  )
#-------------------------------------------------------------------------------
