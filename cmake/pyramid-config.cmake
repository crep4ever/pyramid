set(PROJECT_NAME pyramid)

# If ${SOURCE_DIR} is a git repository VERSION is set to
# `git describe` later.
set(VERSION devel)

set(CODENAME "")

option(GENERATE_MANPAGES "generate manpages" ON)
option(COMPRESS_MANPAGES "compress manpages" ON)
option(OPENMP "multithreading with openmp" ON)
option(RELEASE "release mode" ON)
option(PROFILE "profile mode" OFF)

# {{{ CFLAGS
add_definitions(-DNO_FREETYPE)

if (PROFILE)
  set(RELEASE OFF)
  add_definitions(-DNDEBUG)
endif ()

if (RELEASE)
  add_definitions(-DINLINE_FLAG -O2 -finline-functions -DNDEBUG -march=native -Wno-unused-result) #-ftree-vectorize)
  set_property(DIRECTORY PROPERTY
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM 
    "INCLUDE_INLINE(%)=%")
else ()
  add_definitions(
    -ggdb3 -fno-strict-aliasing -Wall -Wextra
    -Wchar-subscripts -Wundef -Wcast-align -Wwrite-strings
    -Wunused -Wno-unused-parameter 
    -Wuninitialized -Winit-self -Wpointer-arith 
    -Wformat-nonliteral -Wsign-compare  -Wmissing-format-attribute #-Wredundant-decls
    )
  set_property(DIRECTORY PROPERTY
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM 
    "INCLUDE_NON_INLINE(%)=%;INCLUDE_HEADER(%)=%")
endif ()

if (PROFILE)
  add_definitions(-DNDEBUG)
endif ()

#------------------------------------------------------------------------------
# manage inline functions from icc files
if (RELEASE)
else (RELEASE)
endif (RELEASE)

# {{{ Find external utilities
macro(a_find_program var prg req)
  set(required ${req})
  find_program(${var} ${prg})
  if(NOT ${var})
    message(STATUS "${prg} not found.")
    if(required)
      message(FATAL_ERROR "${prg} is required to build ${PROJECT_NAME}")
    endif()
  else()
    message(STATUS "${prg} -> ${${var}}")
  endif()
endmacro()

if (OPENMP)
  find_package(OpenMP)
  if(OPENMP_FOUND)
    add_definitions(-fopenmp)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgomp")
  endif()
endif ()
# }}}

a_find_program(HOSTNAME_EXECUTABLE hostname FALSE)
# programs needed for man pages
a_find_program(ASCIIDOC_EXECUTABLE asciidoc FALSE)
a_find_program(XMLTO_EXECUTABLE xmlto FALSE)
a_find_program(GZIP_EXECUTABLE gzip FALSE)

# pkg-config
include(FindPkgConfig)
# }}}

if(RELEASE)
  message(STATUS "entering release mode")
else()
  message(STATUS "entering debug mode")
endif()

if(OPENMP)
  message(STATUS "openmp multithreading enabled")
else()
  message(STATUS "openmp multithreading disabled")
endif()

# {{{ Check if documentation can be build
if(GENERATE_MANPAGES)
  if(NOT ASCIIDOC_EXECUTABLE OR NOT XMLTO_EXECUTABLE OR (COMPRESS_MANPAGES AND NOT GZIP_EXECUTABLE)
      OR NOT EXISTS ${SOURCE_DIR}/debian/manpages/ )
    if(NOT ASCIIDOC_EXECUTABLE)
      SET(missing "asciidoc")
    endif()
    if(NOT XMLTO_EXECUTABLE)
      SET(missing ${missing} " xmlto")
    endif()
    if(COMPRESS_MANPAGES AND NOT GZIP_EXECUTABLE)
      SET(missing ${missing} " gzip")
    endif()

    message(STATUS "Not generating manpages. Missing: " ${missing})
    set(GENERATE_MANPAGES OFF)
  endif()
endif()
# }}}

# {{{ Version stamp
if(EXISTS ${SOURCE_DIR}/.git/HEAD AND GIT_EXECUTABLE)
  # get current version
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe
    WORKING_DIRECTORY ${SOURCE_DIR}
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(BUILD_FROM_GIT TRUE)
endif()
# }}}

# {{{ Get hostname
execute_process(
  COMMAND ${HOSTNAME_EXECUTABLE}
  WORKING_DIRECTORY ${SOURCE_DIR}
  OUTPUT_VARIABLE BUILDHOSTNAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)
# }}}

# {{{ Install path and configuration variables
if(DEFINED PREFIX)
  set(PREFIX ${PREFIX} CACHE PATH "install prefix")
  set(CMAKE_INSTALL_PREFIX ${PREFIX})
else()
  set(PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "install prefix")
endif()

# set man path
if(DEFINED PYRAMID_MAN_PATH)
  set(PYRAMID_MAN_PATH ${PYRAMID_MAN_PATH} CACHE PATH "pyramid manpage directory")
else()
  set(PYRAMID_MAN_PATH ${PREFIX}/share/man CACHE PATH "pyramid manpage directory")
endif()

find_package(ImageMagick REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
find_package(IMTIFF REQUIRED)

# Hide to avoid confusion
mark_as_advanced(CMAKE_INSTALL_PREFIX)

set(PYRAMID_APPLICATION_NAME ${PROJECT_NAME})
set(PYRAMID_VERSION          ${VERSION})
set(PYRAMID_COMPILE_MACHINE  ${CMAKE_SYSTEM_PROCESSOR})
set(PYRAMID_COMPILE_HOSTNAME ${BUILDHOSTNAME})
set(PYRAMID_COMPILE_BY       $ENV{USER})
set(PYRAMID_RELEASE          ${CODENAME})
set(PYRAMID_DATA_PATH        ${PREFIX}/share/${PYRAMID_APPLICATION_NAME})
# }}}

# {{{ Configure files
set(PYRAMID_CONFIGURE_FILES config.hh.in)

macro(a_configure_file file)
  string(REGEX REPLACE ".in\$" "" outfile ${file})
  message(STATUS "Configuring ${outfile}")
  configure_file(${SOURCE_DIR}/${file}
    ${BUILD_DIR}/${outfile}
    ESCAPE_QUOTE
    @ONLY)
endmacro()

foreach(file ${PYRAMID_CONFIGURE_FILES})
  a_configure_file(${file})
endforeach()
#}}}
