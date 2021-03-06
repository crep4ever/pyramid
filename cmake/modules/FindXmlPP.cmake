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
