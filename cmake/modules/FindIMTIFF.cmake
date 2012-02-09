# - Try to find IMTIFF library
# Once done, this will define
#
#  IMTIFF_FOUND - if a version of the IMTIFF library is found
#  IMTIFF_INCLUDE_DIR - IMTIFF include directories
#  IMTIFF_LIBRARY - the library you should link to use IMTIFF

find_path(IMTIFF_core_INCLUDE_DIR
  NAMES
  IM_Tiff.h image-memory.h
  PATHS
  "$ENV{FOGRIMMI_CODE}/libimtiff/src/core"
  "${CMAKE_SOURCE_DIR}/lib-imtiff/include/core"
  )

find_path(IMTIFF_config_INCLUDE_DIR
  NAMES
  IM_Export.h export.h
  PATHS
  "$ENV{FOGRIMMI_CODE}/libimtiff/src/config"
  "${CMAKE_SOURCE_DIR}/lib-imtiff/include/config"
  )

find_path(IMTIFF_debug_INCLUDE_DIR
  NAMES IM_DebugNew.h logger.h
  PATHS
  "$ENV{FOGRIMMI_CODE}/libimtiff/src/debug"
  "${CMAKE_SOURCE_DIR}/lib-imtiff/include/debug"
  )

find_path(IMTIFF_transform_INCLUDE_DIR
  NAMES IM_Transform.h rotation.h
  PATHS
  "$ENV{FOGRIMMI_CODE}/libimtiff/src/transform"
  "${CMAKE_SOURCE_DIR}/lib-imtiff/include/transform"
  )

find_path(IMTIFF_utilities_INCLUDE_DIR
  NAMES IM_File.h file.h
  PATHS
  "$ENV{FOGRIMMI_CODE}/libimtiff/src/utilities"
  "${CMAKE_SOURCE_DIR}/lib-imtiff/include/utilities"
  )

find_file(IMTIFF_LIBRARY
  NAMES libimtiff.so
  PATHS
  "$ENV{FOGRIMMI_CODE}/libimtiff/debug"
  "${CMAKE_SOURCE_DIR}/lib-imtiff"
  "/usr/lib"
  "/usr/local/lib"
  "/opt/local/lib"
  )

if(APPLE)
  find_file(IMTIFF_LIBRARY
    NAMES libIMTiff_test.dylib
    PATHS
    "$ENV{FOGRIMMI_CODE}/libimtiff/debug"
    )
endif(APPLE)

if( IMTIFF_config_INCLUDE_DIR
    AND IMTIFF_debug_INCLUDE_DIR
    AND IMTIFF_core_INCLUDE_DIR
    AND IMTIFF_transform_INCLUDE_DIR
    AND IMTIFF_utilities_INCLUDE_DIR
    AND IMTIFF_LIBRARY)

  set( IMTIFF_FOUND TRUE )
  set( IMTIFF_INCLUDE_DIR
    ${IMTIFF_config_INCLUDE_DIR}
    ${IMTIFF_core_INCLUDE_DIR}
    ${IMTIFF_debug_INCLUDE_DIR}
    ${IMTIFF_transform_INCLUDE_DIR}
    ${IMTIFF_utilities_INCLUDE_DIR}
    )
endif()

if(NOT IMTIFF_FOUND)
  message(STATUS "Could not find IMTIFF")
  message(" config: ${IMTIFF_config_INCLUDE_DIR}")
  message(" core: ${IMTIFF_core_INCLUDE_DIR}")
  message(" debug: ${IMTIFF_debug_INCLUDE_DIR}")
  message(" transform: ${IMTIFF_transform_INCLUDE_DIR}")
  message(" utilities: ${IMTIFF_utilities_INCLUDE_DIR}")
  message(" library: ${IMTIFF_LIBRARY}")
  message(WARNING "Set IMTIFF location by adding -DLIBIMTIFF=<path>")
else()
  message(STATUS "imtiff -> ${IMTIFF_LIBRARY}")
endif()
