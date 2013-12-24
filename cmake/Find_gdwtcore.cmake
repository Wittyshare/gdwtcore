#
# We set:
# - GDWTCORE_INCLUDE_DIR
# - GDWTCORE_INCLUDE_DIRS
# - GDWTCORE_LIBRARY
# - GDWTCORE_LIBRARIES
# - GDWTCORE_FOUND

FIND_PATH(GDWTCORE_INCLUDE_DIR gdwtcore/gdSqlModel.h PATHS
    /usr/include
    /usr/local/include
    ${GDWTCORE_PREFIX}/include
)

IF( GDWTCORE_INCLUDE_DIR )
  IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(GDWTCORE_LIBRARY       NAMES gdwtcored PATHS ${GDWTCORE_PREFIX}/lib)
   ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
     FIND_LIBRARY(GDWTCORE_LIBRARY       NAMES gdwtcore  PATHS ${GDWTCORE_PREFIX}/lib)
   ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

   IF( GDWTCORE_LIBRARY )
     SET(GDWTCORE_FOUND TRUE)
   ELSE(GDWTCORE_LIBRARY)
     SET(GDWTCORE_FOUND FALSE)
   ENDIF(GDWTCORE_LIBRARY)

   IF(GDWTCORE_FOUND)
      MESSAGE(STATUS "Found the gdwtcore ${CMAKE_BUILD_TYPE} libraries at ${GDWTCORE_LIBRARY}")
      MESSAGE(STATUS "Found the gdwtcore ${CMAKE_BUILD_TYPE} headers   at ${GDWTCORE_INCLUDE_DIR}")
   ELSE(GDWTCORE_FOUND)
      MESSAGE(FATAL_ERROR "Could NOT find gdwtcore ${CMAKE_BUILD_TYPE} libraries")
   ENDIF(GDWTCORE_FOUND)

ENDIF( GDWTCORE_INCLUDE_DIR )

IF(GDWTCORE_FOUND)
   INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_gdcore.cmake)
   INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_gdwt.cmake)
   #INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_vmime.cmake)
   INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_GraphicsMagick.cmake)

   IF(NOT GDCORE_FOUND)
     MESSAGE(FATAL_ERROR "** these program needs gdcore libraries... Abord.")
   ENDIF(NOT GDCORE_FOUND)
   
   IF(NOT GDWT_FOUND)
     MESSAGE(FATAL_ERROR "** these program needs Wt libraries... Abord.")
   ENDIF(NOT GDWT_FOUND)
   
   #IF(NOT VMIME_FOUND)
   #  MESSAGE(FATAL_ERROR "** these program needs vmime libraries... Abord.")
   #ENDIF(NOT VMIME_FOUND)
   
   SET(GDWTCORE_INCLUDE_DIRS
        ${GDCORE_INCLUDE_DIRS}
        ${GDWT_INCLUDE_DIR}
        #${VMIME_INCLUDE_DIRS}
        ${GDWTCORE_INCLUDE_DIR}
   )

   SET(GDWTCORE_LIBRARIES
        ${GDCORE_LIBRARIES}
        ${GDWT_LIBRARY}
        #${VMIME_LIBRARIES}
        ${GDWTCORE_LIBRARY}
   )
#message(STATUS "Found gdwtcore all : ${GDWTCORE_LIBRARIES}")
ENDIF(GDWTCORE_FOUND)

