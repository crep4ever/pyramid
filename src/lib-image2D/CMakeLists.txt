set(SOURCES
image2D.cc
generate-images.cc
coverage-pixels.cc
coverage-pixels-fishkin-barsky.cc
coverage-pixels-jou-tsai.cc
coverage-pixels-jou-tsai-v2.cc
imageTiff.cc
omp_kmeans.cc
)

set(HEADERS
image2D.hh
generate-images.hh
coverage-pixels.hh
coverage-pixels-fishkin-barsky.hh
coverage-pixels-jou-tsai.hh
coverage-pixels-jou-tsai-v2.hh
imageTiff.hh
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

add_library(image2D SHARED ${SOURCES})
target_link_libraries(image2D
  utils
  kmlocal
  ${IMTIFF_LIBRARY}
  ${Magick++_LIBRARIES}
  )
install (TARGETS image2D DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)

