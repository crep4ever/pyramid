################################################################################
project (IMTIFF)
################################################################################

SET(CMAKE_MODULE_PATH   "${IMTIFF_SOURCE_DIR}/src/cmake_modules")
SET(DOCUMENTATION_DIR   "${IMTIFF_DIR}/doc/")
SET(ENV{IMTIFF_DIR}     "${IMTIFF_SOURCE_DIR}/src")
SET(ENV{IMTIFF_LIB_DIR} "${IMTIFF_SOURCE_DIR}/lib")
INCLUDE(${CMAKE_MODULE_PATH}/FindLibXML++.cmake)

################################################################################
# Dependencies
################################################################################


FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(TIFF REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Glib REQUIRED)
FIND_PACKAGE(Glibmm REQUIRED)

################################################################################
# Includes and libraries
################################################################################

# Seek at library dependencies
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

include_directories(
  ${JPEG_INCLUDE_DIR}
  ${QT_INCLUDE_DIR}
  ${QT_QTOPENGL_INCLUDE_DIR}
  ${TIFF_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLIB2_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIR}
  ${GLIBMM_INCLUDE_LIBDIR}
  ${XMLPP_INCLUDE_DIR}
  ${XMLPP_INCLUDE_LIBDIR}
  )

SET(LINK_CORE_LIBRARIES
  ${JPEG_LIBRARIES}
  ${TIFF_LIBRARY}
  ${ZLIB_LIBRARY}
  ${GLIB2_LIBRARIES}
  ${GLIBMM_LIBRARY}
  ${XMLPP_LIBRARY}
  )

#-------------------------------------------------------------------------------
include_directories (${IMTIFF_SOURCE_DIR}/src/config)
include_directories (${IMTIFF_SOURCE_DIR}/src/core)
include_directories (${IMTIFF_SOURCE_DIR}/src/debug)
include_directories (${IMTIFF_SOURCE_DIR}/src/transform)
include_directories (${IMTIFF_SOURCE_DIR}/src/utilities)


################################################################################
# build
################################################################################

# Set source file for IMTiff
SET(IMTIFF_SRCS
  $ENV{IMTIFF_DIR}/core/image.cpp
  $ENV{IMTIFF_DIR}/core/image-memory.cpp
  $ENV{IMTIFF_DIR}/core/lignel.cpp
  $ENV{IMTIFF_DIR}/core/page.cpp
  $ENV{IMTIFF_DIR}/core/strip.cpp
  $ENV{IMTIFF_DIR}/core/tiff.cpp
  $ENV{IMTIFF_DIR}/core/tile.cpp
  $ENV{IMTIFF_DIR}/core/tools.cpp
  $ENV{IMTIFF_DIR}/debug/exceptions.cpp
  $ENV{IMTIFF_DIR}/debug/logger.cpp
  $ENV{IMTIFF_DIR}/debug/logger-debug.cpp
  $ENV{IMTIFF_DIR}/debug/logger-file.cpp
  $ENV{IMTIFF_DIR}/debug/memory-manager.cpp 
  $ENV{IMTIFF_DIR}/transform/projection.cpp
  $ENV{IMTIFF_DIR}/transform/rotation.cpp
  $ENV{IMTIFF_DIR}/transform/zoom.cpp
  $ENV{IMTIFF_DIR}/utilities/dom-parser.cpp 
  $ENV{IMTIFF_DIR}/utilities/file.cpp
  $ENV{IMTIFF_DIR}/utilities/sax-parser.cpp
  )

# Set output path
SET(LIBRARY_OUTPUT_PATH $ENV{IMTIFF_LIB_DIR})

# Create a Shared library
add_library(imtiff STATIC ${IMTIFF_SRCS})

# Link the IMTIff to dependence library.
target_link_libraries(imtiff ${LINK_CORE_LIBRARIES})


