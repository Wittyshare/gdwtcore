#- Find vmime includes and library
 #
 # This module defines
 #  VMIME_INCLUDE_DIR
 #  VMIME_LIBRARIES, the libraries to link against to use VMIME.
 #  VMIME_LIB_DIR, the location of the libraries
 #  VMIME_FOUND, If false, do not try to use VMIME
 #
 # Redistribution and use is allowed according to the terms of the BSD license.
 # For details see the accompanying COPYING-CMAKE-SCRIPTS file.
 
 IF (VMIME_LIBRARIES AND VMIME_INCLUDE_DIR)
    SET(VMIME_FIND_QUIETLY TRUE) # Already in cache, be silent
 ENDIF (VMIME_LIBRARIES AND VMIME_INCLUDE_DIR)
 
 
 FIND_PATH(VMIME_INCLUDE_DIR vmime/vmime.hpp 
     /usr/include
     /usr/local/include
     /usr/local/vmime/include
 )
 
 FIND_LIBRARY(VMIME_LIBRARY NAMES vmime PATHS
     /usr/lib
     /usr/local/lib
     /usr/local/vmime/lib
 )
 
 # Copy the results to the output variables.
 IF (VMIME_INCLUDE_DIR AND VMIME_LIBRARY)
         SET(VMIME_FOUND 1)
         SET(VMIME_LIBRARIES ${VMIME_LIBRARY})
         SET(VMIME_INCLUDE_DIRS ${VMIME_INCLUDE_DIR})
         
         MESSAGE(STATUS "Found these vmime libs: ${VMIME_LIBRARIES}")
         
 ELSE (VMIME_INCLUDE_DIR AND VMIME_LIBRARY)
         SET(VMIME_FOUND 0)
         SET(VMIME_LIBRARIES)
         SET(VMIME_INCLUDE_DIRS)
 ENDIF (VMIME_INCLUDE_DIR AND VMIME_LIBRARY)
 
 # Report the results.
 IF (NOT VMIME_FOUND)
         SET(VMIME_DIR_MESSAGE "vmime was not found. Make sure VMIME_LIBRARY and VMIME_INCLUDE_DIR are set.")
         IF (NOT VMIME_FIND_QUIETLY)
                 MESSAGE(STATUS "${VMIME_DIR_MESSAGE}")
         ELSE (NOT VMIME_FIND_QUIETLY)
                 IF (VMIME_FIND_REQUIRED)
                         MESSAGE(FATAL_ERROR "${VMIME_DIR_MESSAGE}")
                 ENDIF (VMIME_FIND_REQUIRED)
         ENDIF (NOT VMIME_FIND_QUIETLY)
 ENDIF (NOT VMIME_FOUND)
 
 
 MARK_AS_ADVANCED(
     VMIME_INCLUDE_DIRS
     VMIME_LIBRARIES
 )
