set(SOURCES
  KCtree.cpp
  KMdata.cpp
  KMrand.cpp
  KCutil.cpp
  KMeans.cpp
  KMterm.cpp
  KM_ANN.cpp
  KMfilterCenters.cpp
  KMcenters.cpp
  KMlocal.cpp
)

set(HEADERS
  KCtree.h
  KMdata.h
  KMrand.h
  KCutil.h
  KMeans.h
  KMterm.h
  KM_ANN.h
  KMfilterCenters.h
  KMcenters.h
  KMlocal.h
)

add_library(kmlocal SHARED ${SOURCES})
install (TARGETS kmlocal DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)
