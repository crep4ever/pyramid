IF(UNIX OR APPLE)
	FIND_PATH(XMLPP_INCLUDE_DIR NAMES libxml++ PATHS /usr/local/include/libxml++-2.6 /usr/include/libxml++-2.6)
	IF(APPLE)
		FIND_FILE(XMLPP_LIBRARY libxml++-2.6.dylib PATHS /usr/local/lib /usr/lib)
		FIND_PATH(XMLPP_INCLUDE_LIBDIR NAMES libxml++config.h PATHS /usr/local/lib/libxml++-2.6/include /usr/lib/libxml++-2.6/include)		
	ENDIF(APPLE)
	IF(UNIX)
		FIND_FILE(XMLPP_LIBRARY libxml++-2.6.so PATHS /usr/lib /usr/local/lib)
		FIND_PATH(XMLPP_INCLUDE_LIBDIR NAMES libxml++config.h PATHS /usr/local/lib/libxml++-2.6/include /usr/lib/libxml++-2.6/include)
		IF(NOT XMLPP_INCLUDE_LIBDIR)
			SET(XMLPP_INCLUDE_LIBDIR ${XMLPP_INCLUDE_DIR})
		ENDIF(NOT XMLPP_INCLUDE_LIBDIR)
	ENDIF(UNIX)
ENDIF(UNIX OR APPLE)



IF(XMLPP_INCLUDE_DIR AND XMLPP_LIBRARY)
	IF(UNIX)
		SET(XMLPP_FOUND TRUE)
	ENDIF(UNIX)
	IF(APPLE AND XMLPP_INCLUDE_LIBDIR)
		SET(XMLPP_FOUND TRUE)
	ENDIF(APPLE AND XMLPP_INCLUDE_LIBDIR)
ENDIF(XMLPP_INCLUDE_DIR AND XMLPP_LIBRARY)

IF(XMLPP_FOUND)
		MESSAGE(STATUS "Found XML++ : ${XMLPP_LIBRARY}")
ELSE(XMLPP_FOUND)
		MESSAGE(FATAL_ERROR "Could not find XML++")
		MESSAGE("SET LIBXMLPP location by adding -DLIBXMLPP=<path>")
ENDIF(XMLPP_FOUND)


## - Try to find LibXML++ 2.6
## Once done, this will define
##
##  LibXML++_FOUND - system has LibXML++
##  LibXML++_INCLUDE_DIRS - the LibXML++ include directories
##  LibXML++_LIBRARIES - link these to use LibXML++
#
#include(LibFindMacros)
#
## Dependencies
#libfind_package(LibXML++ LibXML2)
#libfind_package(LibXML++ Glibmm)
#
## Use pkg-config to get hints about paths
#libfind_pkg_check_modules(LibXML++_PKGCONF libxml++-2.6)
#
## Main include dir
#find_path(LibXML++_INCLUDE_DIR
#  NAMES libxml++
#  PATHS /usr/include/libxml++-2.6
#  PATH_SUFFIXES libxml++-2.6
#)
#
## Glib-related libraries also use a separate config header, which is in lib dir
#find_path(LibXML++Config_INCLUDE_DIR
#  NAMES libxml++config.h
#  PATHS ${LibXML++_PKGCONF_INCLUDE_DIRS} /usr
#  PATH_SUFFIXES lib/libxml++-2.6/include
#)
#
## Finally the library itself
#find_library(LibXML++_LIBRARY
#  NAMES xml++-2.6
#  PATHS ${LibXML++_PKGCONF_LIBRARY_DIRS}
#)
#
## Set the include dir variables and the libraries and let libfind_process do the rest.
## NOTE: Singular variables for this library, plural for libraries this this lib depends on.
#set(LibXML++_PROCESS_INCLUDES LibXML++_INCLUDE_DIR LibXML++Config_INCLUDE_DIR LibXML2_INCLUDE_DIRS Glibmm_INCLUDE_DIRS)
#set(LibXML++_PROCESS_LIBS LibXML++_LIBRARY LibXML2_LIBRARIES Glibmm_LIBRARIES)
#libfind_process(LibXML++)