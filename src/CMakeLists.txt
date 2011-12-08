#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6.2)
project (pyramid)
set(PROJECT_NAME pyramid)


set(DOCUMENTATION_DIR "${CMAKE_SOURCE_DIR}/../doc/")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Magick++ REQUIRED)

#-------------------------------------------------------------------------------
# Des options de compilation que l'on peut activer avec ADD_DEFINITIONS(xxx).

# -DNO_ACCENTED_CHARS pour ne pas utiliser de caractères accentués;
# -DDEBUG_DSS pour activer le mode debug des calculs dans dss; 
#   (utilisé seulement si l'option DSS_RECONSTRUCTION est on).
# -DDEBUG_EXTRACT_IMAGE pour tracer l'extraction d'une image;
# -DDISPLAY_TIME_EXTRACT_IMAGE pour afficher les temps de calcul;
# -DIMTIFF pour utiliser la libImTIff plutot que ImageMagick (obligatoire pour les pyramides);

option(OPENMP "multithreading with openmp" ON)
if (OPENMP)
  add_definitions("-fopenmp")
endif ()

add_definitions("-DNO_FREETYPE")
#-------------------------------------------------------------------------------
# Types de distributions :
# Debug       = non inline + assert 
# Profile     = non inline + no-assert 
# Release     = inline     + no-assert + O2

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_PROFILE "-g -DINLINE_FLAG -O2 -finline-functions -DNDEBUG -march=native -ftree-vectorize")
set(CMAKE_CXX_FLAGS_RELEASE "-DINLINE_FLAG -O2 -finline-functions -DNDEBUG -march=native -ftree-vectorize")

#------------------------------------------------------------------------------
# manage inline functions from icc files

#Mode debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set_property(DIRECTORY PROPERTY
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM 
    "INCLUDE_NON_INLINE(%)=%;INCLUDE_HEADER(%)=%")
  message( STATUS "**** Mode Debug ****")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

#Mode release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(DIRECTORY PROPERTY
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM 
    "INCLUDE_INLINE(%)=%")
  message( STATUS "***** Mode Release ****" )
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

#Mode profile
if (CMAKE_BUILD_TYPE STREQUAL "Profile")
  set_property(DIRECTORY PROPERTY
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM 
    "INCLUDE_NON_INLINE(%)=%;INCLUDE_HEADER(%)=%")
  message( STATUS "***** Mode Profile ****" )
endif (CMAKE_BUILD_TYPE STREQUAL  "Profile")

  
#-------------------------------------------------------------------------------
add_subdirectory (lib-utils)
add_subdirectory (lib-kmlocal)
add_subdirectory (lib-simplekmeans)
add_subdirectory (lib-clustering)
add_subdirectory (lib-imtiff)
add_subdirectory (lib-image2D)
add_subdirectory (lib-mapkernel)
add_subdirectory (ihm)
add_subdirectory (test)
#-------------------------------------------------------------------------------
