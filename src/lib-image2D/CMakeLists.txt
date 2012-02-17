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

include_directories(${CMAKE_SOURCE_DIR}/lib-utils)
include_directories(${CMAKE_SOURCE_DIR}/lib-kmlocal)
include_directories(${IMTIFF_INCLUDE_DIR})
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ImageMagick_Magick++_INCLUDE_DIRS})
include_directories(${BUILD_DIR}) #for config.hh

add_library(image2D SHARED ${SOURCES})
target_link_libraries(image2D
  utils
  kmlocal
  ${IMTIFF_LIBRARY}
  ${ImageMagick_LIBRARIES}
  ${ImageMagick_Magick++_LIBRARIES}
  )
install (TARGETS image2D DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)

