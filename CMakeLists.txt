cmake_minimum_required(VERSION 2.6.2)
project (pyramid)
set(PROJECT_NAME pyramid)


set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CONFIG_PATH "${SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${SOURCE_DIR}/cmake/modules")
set(CMAKE_SOURCE_DIR "${SOURCE_DIR}/src")
set(DOCUMENTATION_DIR "{SOURCE_DIR}/doc")

if(NOT EXISTS ${CMAKE_CONFIG_PATH}/pyramid-config.cmake)
  message(FATAL_ERROR "Missing configuration file : ${CMAKE_CONFIG_PATH}/pyramid-config.cmake")
endif()

include(${CMAKE_CONFIG_PATH}/pyramid-config.cmake)

#-------------------------------------------------------------------------------
add_subdirectory (src/lib-utils)
add_subdirectory (src/lib-kmlocal)
add_subdirectory (src/lib-clustering)
add_subdirectory (src/lib-imtiff)
add_subdirectory (src/lib-image2D)
add_subdirectory (src/lib-mapkernel)
add_subdirectory (src/ihm)
add_subdirectory (src/test)
#-------------------------------------------------------------------------------

# {{{ Manpages
if(GENERATE_MANPAGES)

  set(PYRAMID_MAN_SRCS
    ${SOURCE_DIR}/debian/manpages/pryamid.1.txt
    )

  set(PYRAMID_MAN_LANGS default fr)

  foreach(lang ${PYRAMID_MAN_LANGS})

    foreach(txtfile ${PYRAMID_MAN_SRCS})
      # figure the base name of the file (ie "songbook-client.1")
      GET_FILENAME_COMPONENT(tmpname ${txtfile} NAME)
      string(REGEX REPLACE ".txt\$" "" basename ${tmpname})

      # figure the relative path of the file
      GET_FILENAME_COMPONENT(tmppath ${txtfile} PATH)
      string(REPLACE ${SOURCE_DIR}/ "" relpath ${tmppath})

      # figure the manpage section to install to from filename
      string(REGEX REPLACE "^.*\\.([0-9])$" "\\1" section ${basename})

      # construct the language specific versions of the basename and path
      if (lang STREQUAL default)
        set(basename2 ${basename})
        set(relpath2 ${relpath}/man${section})
      else()
        set(basename2 ${basename}.${lang})
        set(relpath2 ${relpath}/${lang}/man${section})
      endif()

      # create the build directory (if it does not exist)
      file(MAKE_DIRECTORY ${BUILD_DIR}/${relpath2})

      # set the final filenames
      set(txtfile ${SOURCE_DIR}/${relpath}/${basename2}.txt)
      set(xmlfile ${BUILD_DIR}/${relpath2}/${basename}.xml)
      set(gzfile  ${BUILD_DIR}/${relpath2}/${basename}.gz)
      set(manfile ${BUILD_DIR}/${relpath2}/${basename})

      add_custom_command(
        COMMAND ${ASCIIDOC_EXECUTABLE} -d manpage -b docbook -o ${xmlfile} - < ${txtfile}
        WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
        OUTPUT  ${xmlfile}
        DEPENDS ${txtfile}
        VERBATIM)
      add_custom_command(
        COMMAND ${XMLTO_EXECUTABLE} man ${xmlfile}
        OUTPUT  ${manfile}
        WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
        DEPENDS ${xmlfile})

      if(COMPRESS_MANPAGES)
        add_custom_command(
          COMMAND ${GZIP_EXECUTABLE} < ${manfile} > ${gzfile}
          OUTPUT  ${gzfile}
          WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
          DEPENDS ${manfile}
          VERBATIM)

        set(MAN_FILES ${MAN_FILES} ${gzfile})
      else()
        set(MAN_FILES ${MAN_FILES} ${manfile})
      endif()
    endforeach()

  endforeach()

  add_custom_target(man ALL DEPENDS ${MAN_FILES})
endif()
# }}}

if(GENERATE_MANPAGES)
  if(COMPRESS_MANPAGES)
    set(regex "\\.(xml|txt|[0-9])$")
  else()
    set(regex "\\.(xml|txt|gz)$")
  endif()
  install(DIRECTORY ${BUILD_DIR}/${relpath}/ DESTINATION ${PYRAMID_MAN_PATH}
    REGEX ${regex} EXCLUDE)
endif()
install(FILES ${PYRAMID_APPLICATION_NAME}.desktop DESTINATION share/applications)
install(FILES src/ihm/icons/pyramid.png DESTINATION share/icons)
#install(FILES ${COMPILED_TRANSLATIONS} DESTINATION ${PYRAMID_DATA_PATH}/lang)
