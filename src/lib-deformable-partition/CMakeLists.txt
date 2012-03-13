#-------------------------------------------------------------------------------
if (NOT ${DEFORMABLE_METHOD} EQUAL 0)
  add_library (deformable-partition 
#     c4c-iterator-edge-linels.cc
#     c4c-iterator-edge-linels-surface.cc
    deformable-partition.cc)

  if (NOT ${DEFORMABLE_METHOD} EQUAL 1)
    set(CMAKE_MODULE_PATH "/home/gdamiand/sources/codes-autres/imagene")  
    message(CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH})
    find_package(ImaGene REQUIRED)    
    include_directories("${ImaGene_INCLUDE_DIRS}")
    target_link_libraries(deformable-partition ${ImaGene_LIBRARIES})
  endif (NOT ${DEFORMABLE_METHOD} EQUAL 1)

endif (NOT ${DEFORMABLE_METHOD} EQUAL 0)
#-------------------------------------------------------------------------------
