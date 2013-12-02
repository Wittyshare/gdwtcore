#
# We set:
# - GDWT_INCLUDE_DIR
# - GDWT_LIBRARY
# - GDWT_FOUND

# TODO REMOVE this file
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${GDWT_PREFIX})

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindWt.cmake)

IF( Wt_INCLUDE_DIR )
  SET(GDWT_INCLUDE_DIR ${Wt_INCLUDE_DIR})
  IF(CMAKE_BUILD_TYPE MATCHES "Debug")
     SET(GDWT_LIBRARY ${Wt_DEBUG_LIBRARIES})
     IF(CONNECTOR_FCGI)
#        SET(GDWT_FCGI wtfcgid)
     ENDIF(CONNECTOR_FCGI)
     IF(CONNECTOR_HTTP)
#        SET(GDWT_HTTP wthttpd)
     ENDIF(CONNECTOR_HTTP)
   ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
     SET(GDWT_LIBRARY ${Wt_LIBRARIES})
     IF(CONNECTOR_FCGI)
#        SET(GDWT_FCGI wtfcgi)
     ENDIF(CONNECTOR_FCGI)
     IF(CONNECTOR_HTTP)
#        SET(GDWT_HTTP wthttp)
     ENDIF(CONNECTOR_HTTP)
   ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

   IF( GDWT_LIBRARY )
     SET(GDWT_FOUND TRUE)
   ELSE(GDWT_LIBRARY)
     SET(GDWT_FOUND FALSE)
   ENDIF(GDWT_LIBRARY)

   IF(GDWT_FOUND)
      MESSAGE(STATUS "Found the wt ${CMAKE_BUILD_TYPE} libraries at ${GDWT_LIBRARY}")
      MESSAGE(STATUS "Found the wt ${CMAKE_BUILD_TYPE} headers   at ${GDWT_INCLUDE_DIR}")
   ELSE(GDWT_FOUND)
      MESSAGE(FATAL_ERROR "Could NOT find wt ${CMAKE_BUILD_TYPE} libraries")
   ENDIF(GDWT_FOUND)

 ELSE( Wt_INCLUDE_DIR )
      MESSAGE(FATAL_ERROR "Could NOT find wt ${CMAKE_BUILD_TYPE} include files")
ENDIF( Wt_INCLUDE_DIR )
