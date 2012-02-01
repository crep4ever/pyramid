#-------------------------------------------------------------------------------
set (IHM_SRCS
  main.cc
  script-engine.cc
  controler.cc
  scene.cc
  view.cc
  view-decorator.cc
  tab-controler.cc
  main-window.cc
  controler.cc
  scene.cc
  settings-tree.cc
  variant-delegate.cc
  dart-path.cc
  file-chooser.cc
  panel.cc
  )
set (NEED_MOCS
  main-window.hh
  tab-controler.hh
  script-engine.hh
  controler.hh
  scene.hh
  view.hh
  view-decorator.hh
  settings-tree.hh
  variant-delegate.hh
  file-chooser.hh
  panel.hh
  )
set (IHM_RCS
  pyramid.qrc
  )

#-------------------------------------------------------------------------------
include_directories (${BUILD_DIR}) #for config.hh
include_directories (${CMAKE_SOURCE_DIR}/lib-utils)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/pyramid)

include_directories (${CMAKE_SOURCE_DIR}/lib-image2D)
include_directories (${CMAKE_SOURCE_DIR}/lib-clustering)

include_directories(${IMTIFF_SOURCE_DIR}/src/config)
include_directories(${IMTIFF_SOURCE_DIR}/src/core)
include_directories(${IMTIFF_SOURCE_DIR}/src/debug)
include_directories(${IMTIFF_SOURCE_DIR}/src/transform)
include_directories(${IMTIFF_SOURCE_DIR}/src/utilities)
include_directories(${Magick++_INCLUDE_DIRS})

#-------------------------------------------------------------------------------
# We need X11 library
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

include_directories (${QT_QTSCRIPT_INCLUDE_DIR})

QT4_WRAP_CPP(MOC_SRCS ${NEED_MOCS})
QT4_ADD_RESOURCES( IHM_RC_RCS ${IHM_RCS} )

# {{{ Internationalization
set (TRANSLATIONS ${SOURCE_DIR}/lang/pyramid_fr.ts)

set (COMPILED_TRANSLATIONS ${SOURCE_DIR}/lang/pyramid_fr.qm)

add_custom_target(
  translations_target
  COMMAND "${QT_LRELEASE_EXECUTABLE}" ${TRANSLATIONS}
  DEPENDS ${TRANSLATIONS}
  COMMENT "Updating translations"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#uncomment and run `make translations_target` to update translations ts files from sources
#warning, if uncommented, running make clean will remove ts files
#qt4_create_translation(COMPILED_TRANSLATIONS ${IHM_SRCS} ${TRANSLATIONS} )
# }}}

#-------------------------------------------------------------------------------
add_executable(../../${PYRAMID_APPLICATION_NAME} ${IHM_SRCS} ${MOC_SRCS} ${IHM_RC_RCS} ${TRANSLATIONS})
target_link_libraries(../../${PYRAMID_APPLICATION_NAME} mapkernel)
target_link_libraries(../../${PYRAMID_APPLICATION_NAME} ${QT_LIBRARIES})
target_link_libraries(../../${PYRAMID_APPLICATION_NAME} ${QT_QTSCRIPT_LIBRARY})
#-------------------------------------------------------------------------------

install(TARGETS ../../${PYRAMID_APPLICATION_NAME} RUNTIME DESTINATION bin)
