#-------------------------------------------------------------------------------
# Si on n'a pas trouvé tbb, on n'active pas la reconstruction discrète.
if (CONTOUR_SIMPLIFICATION_ON)
   add_library (contour-simplification contour-simplification.cc
                dss-reconstruction.cc polygonal-approximation.cc
		moment-preserving-simplification.cc)

   include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel)
   include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map)
   include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/map-generic)
   include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/topological-map)
   include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/dss)

   if (TBB_ACTIVE)
      INCLUDE_DIRECTORIES(${TBB_INCLUDE})
      target_link_libraries(contour-simplification ${TBB_LIBRARY})
   endif (TBB_ACTIVE)
   
   include_directories (${CMAKE_SOURCE_DIR}/lib-mapkernel/libImage2D)
endif (CONTOUR_SIMPLIFICATION_ON)
#-------------------------------------------------------------------------------
