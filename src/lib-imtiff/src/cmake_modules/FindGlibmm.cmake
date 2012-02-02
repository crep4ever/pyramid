IF(UNIX OR APPLE)
	FIND_PATH(GLIBMM_INCLUDE_DIR NAMES glibmm.h PATHS /usr/local/include/glibmm-2.4 /usr/include/glibmm-2.4)
	FIND_PATH(GLIBMM_INCLUDE_LIBDIR NAMES glibmmconfig.h PATHS /usr/local/lib/glibmm-2.4/include /usr/lib/glibmm-2.4/include /usr/lib/x86_64-linux-gnu/glibmm-2.4/include /usr/lib/i386-linux-gnu/glibmm-2.4/include)
	IF(APPLE)
		FIND_FILE(GLIBMM_LIBRARY libglibmm-2.4.dylib PATHS /usr/local/lib /usr/lib)
	ENDIF(APPLE)
	IF(UNIX)
		FIND_FILE(GLIBMM_LIBRARY libglibmm-2.4.so PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/lib/i386-linux-gnu/)
	ENDIF(UNIX)
ENDIF(UNIX OR APPLE)



IF(GLIBMM_INCLUDE_DIR AND GLIBMM_LIBRARY AND GLIBMM_INCLUDE_LIBDIR)
	SET(GLIBMM_FOUND TRUE)
ENDIF(GLIBMM_INCLUDE_DIR AND GLIBMM_LIBRARY AND GLIBMM_INCLUDE_LIBDIR)

IF(GLIBMM_FOUND)
		MESSAGE(STATUS "Found GLIBMM : ${GLIBMM_LIBRARY}")
ELSE(GLIBMM_FOUND)
		MESSAGE(FATAL_ERROR "Could not find GLIBMM")
		MESSAGE("SET LIBGLIBMM location by adding -DLIBGLIBMM=<path>")
ENDIF(GLIBMM_FOUND)
