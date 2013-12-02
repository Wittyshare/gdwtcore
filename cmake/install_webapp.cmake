IF(CONNECTOR_FCGI)
   ADD_EXECUTABLE(
                  ${APPNAME}.wt
                  ${APPNAME_SRCS} 
                 )
   
   TARGET_LINK_LIBRARIES(
                         ${APPNAME}.wt
 #                        ${GDWT_FCGI}
   	                 ${APP_LIBRARIES}
                        )
ENDIF(CONNECTOR_FCGI)

IF(CONNECTOR_HTTP)
ADD_EXECUTABLE(
               ${APPNAME}.wthttpd
               ${APPNAME_SRCS} 
              )

TARGET_LINK_LIBRARIES(
                      ${APPNAME}.wthttpd
#                      ${GDWT_HTTP}
		      ${APP_LIBRARIES}
                     )
ENDIF(CONNECTOR_HTTP)


IF(CONNECTOR_FCGI)
   IF(CMAKE_BUILD_TYPE MATCHES "Debug")
      INSTALL(PROGRAMS  ${CMAKE_CURRENT_BINARY_DIR}/${APPNAME}.wt       DESTINATION ${DEPLOYROOT}/${APPNAME} CONFIGURATIONS Debug    RENAME ${APPNAME}d.wt)
   ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")
   IF(CMAKE_BUILD_TYPE MATCHES "Release")
      INSTALL(PROGRAMS  ${CMAKE_CURRENT_BINARY_DIR}/${APPNAME}.wt       DESTINATION ${DEPLOYROOT}/${APPNAME} CONFIGURATIONS Release)
   ENDIF(CMAKE_BUILD_TYPE MATCHES "Release")
ENDIF(CONNECTOR_FCGI)

IF(CONNECTOR_HTTP)
   IF(CMAKE_BUILD_TYPE MATCHES "Debug")
      INSTALL(PROGRAMS  ${CMAKE_CURRENT_BINARY_DIR}/${APPNAME}.wthttpd  DESTINATION ${DEPLOYROOT}/${APPNAME} CONFIGURATIONS Debug    RENAME ${APPNAME}d.wthttpd)
   ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")
   IF(CMAKE_BUILD_TYPE MATCHES "Release")
      INSTALL(PROGRAMS  ${CMAKE_CURRENT_BINARY_DIR}/${APPNAME}.wthttpd  DESTINATION ${DEPLOYROOT}/${APPNAME} CONFIGURATIONS Release)
   ENDIF(CMAKE_BUILD_TYPE MATCHES "Release")
ENDIF(CONNECTOR_HTTP)


INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/install_resources.cmake)
