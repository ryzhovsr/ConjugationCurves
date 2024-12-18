
FUNCTION(_PRIVATE_RPLM_INSTALL TARGET_NAME INSTALL_BIN_PATH INSTALL_LIB_PATH INSTALL_INCLUDE_PATH 
	#COMPONENTS
	)
	IF (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Include" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Include")
		SET(INCLUDE_PATH_EXISTS 1)
	ENDIF()
	
	IF (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
		SET(RESOURCES_PATH_EXISTS 1)
	ENDIF()
	
	#FOREACH (COMPONENT ${COMPONENTS})
		#Install includes only for SDK component.		
		FOREACH(CONFIG ${CMAKE_CONFIGURATION_TYPES})
			#Copy targets
			INSTALL(TARGETS ${TARGET_NAME}
					RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_PATH} 
						#COMPONENT ${COMPONENT} 
						CONFIGURATIONS ${CONFIG}
					LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_PATH} 
						#COMPONENT ${COMPONENT} 
						CONFIGURATIONS ${CONFIG}
					ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_PATH}
						#COMPONENT ${COMPONENT} 
						CONFIGURATIONS ${CONFIG}
					)
		ENDFOREACH()
		IF (INCLUDE_PATH_EXISTS 
			#AND ${COMPONENT} STREQUAL "SDK"
			)
			INSTALL(DIRECTORY Include/ 
						DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_PATH} 
						#COMPONENT ${COMPONENT}
						)
		ENDIF()
		#Copy resources folder 
		IF (RESOURCES_PATH_EXISTS)
				#message(${TARGET_NAME} Resources ${INSTALL_BIN_PATH}/${CONFIG})
			
				INSTALL(DIRECTORY Resources/ 
						DESTINATION ${INSTALL_BIN_PATH}/Resources
						#COMPONENT ${COMPONENT} 
						)
		ENDIF()
	#ENDFOREACH()
ENDFUNCTION()

#------------------------------------------------------------------------------

FUNCTION(RPLM_INSTALL_APPLICATION) 
	SET(ONE_VALUE_ARGS TARGET)
	SET(MULTI_VALUE_ARGS FILES)

	CMAKE_PARSE_ARGUMENTS(PARAM "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
	
	IF (PARAM_TARGET)
		_PRIVATE_RPLM_INSTALL(${PARAM_TARGET} ${APP_INSTALL_BIN} ${APP_INSTALL_LIB} ${APP_INSTALL_INCLUDE} 
			#"DEMO"
			)
	ENDIF()

	IF (PARAM_FILES)
		FOREACH(FILE_PATH ${PARAM_FILES})
			#Check path type and correct. 
			IF(IS_ABSOLUTE ${FILE_PATH})
				SET(COPY ${FILE_PATH})
			ELSE()
				SET(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH})
			ENDIF()
		
			FOREACH(CONFIG ${CMAKE_CONFIGURATION_TYPES})
				IF(IS_DIRECTORY "${COPY}")
					INSTALL(DIRECTORY ${COPY} DESTINATION ${APP_INSTALL_BIN}/${CONFIG} 
						#COMPONENT "DEMO"	
						CONFIGURATIONS ${CONFIG})
				ELSE()
					INSTALL(FILES ${COPY} DESTINATION ${APP_INSTALL_BIN}/${CONFIG} 
						#COMPONENT "DEMO"	
						CONFIGURATIONS ${CONFIG})
				ENDIF()
			ENDFOREACH()
		ENDFOREACH()
	ENDIF()
	
ENDFUNCTION()

#------------------------------------------------------------------------------

FUNCTION(RPLM_INSTALL_CORE)
	SET(ONE_VALUE_ARGS TARGET POSTFIX_INCLUDE POSTFIX_BIN POSTFIX_LIB)
	
	CMAKE_PARSE_ARGUMENTS(PARAM "" "${ONE_VALUE_ARGS}" "" ${ARGN})
	
	SET(BIN ${APP_INSTALL_BIN})
	SET(LIB ${APP_INSTALL_LIB})
	SET(INCLUDE ${APP_INSTALL_INCLUDE})
	
	IF(PARAM_POSTFIX_BIN)
		SET(BIN ${BIN}/${PARAM_POSTFIX_BIN})
	ENDIF()
	
	IF(PARAM_POSTFIX_LIB)
		SET(LIB ${LIB}/${PARAM_POSTFIX_LIB})
	ENDIF()
	
	IF(PARAM_POSTFIX_INCLUDE)
		SET(INCLUDE ${INCLUDE}/${PARAM_POSTFIX_INCLUDE})
	ENDIF()
	
	_PRIVATE_RPLM_INSTALL(${PARAM_TARGET} ${BIN} ${LIB} ${INCLUDE} 
		#"DEMO;SDK"
		)
ENDFUNCTION()

#------------------------------------------------------------------------------

FUNCTION(RPLM_INSTALL_BINARY)
	SET(ONE_VALUE_ARGS TARGET CONFIG)
	SET(MULTI_VALUE_ARGS FILES DIRECTORY)
	
	CMAKE_PARSE_ARGUMENTS(PARAM "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

	FOREACH(CONFIG ${CMAKE_CONFIGURATION_TYPES})
		IF ((NOT DEFINED PARAM_CONFIG) OR (${PARAM_CONFIG} MATCHES ${CONFIG}))
		#FOREACH (component SDK DEMO)
			SET(LIB ${CMAKE_INSTALL_PREFIX}/${APP_INSTALL_LIB})
			SET(BIN ${CMAKE_INSTALL_PREFIX}/${APP_INSTALL_BIN})
		
			#IF(PARAM_POSTFIX_DEST)
				#SET(BIN ${BIN}/${PARAM_POSTFIX_DEST})
				#SET(LIB ${LIB}/${PARAM_POSTFIX_DEST})
			#ENDIF()
		
			IF(PARAM_FILES)
				INSTALL(FILES ${PARAM_FILES} 
						DESTINATION ${BIN} 
						#COMPONENT ${component} 
						CONFIGURATIONS ${CONFIG}
						)
			ENDIF()
			
			IF(PARAM_DIRECTORY)
				INSTALL(DIRECTORY ${PARAM_DIRECTORY} 
						DESTINATION ${BIN} 
						#COMPONENT ${component} 
						CONFIGURATIONS ${CONFIG}
						)
			ENDIF()
			
			IF(PARAM_TARGET)
				INSTALL(TARGETS ${PARAM_TARGET}
						RUNTIME DESTINATION ${BIN} 
							#COMPONENT ${component} 
							CONFIGURATIONS ${CONFIG}
						LIBRARY DESTINATION ${BIN} 
							#COMPONENT ${component} 
							CONFIGURATIONS ${CONFIG}
						)	
			ENDIF()
		#ENDFOREACH()
		ENDIF()
	ENDFOREACH()
ENDFUNCTION()

#------------------------------------------------------------------------------

FUNCTION(RPLM_INSTALL_QT5_BIN TARGET_NAME INSTALL_BIN_PATH)

	SET(QT5_BINARY_DIR ${_qt5Core_install_prefix}/bin)
	
	IF(WIN32)
		FOREACH(CONFIG ${CMAKE_CONFIGURATION_TYPES})
			SET(DEBUG_FILE_POSTFIX)
			IF(${CONFIG} STREQUAL Debug)
				SET(DEBUG_FILE_POSTFIX "d")
			ELSE()
				IF(${CONFIG} STREQUAL RelWithDebInfo)
					SET(DEBUG_FILE_POSTFIX "d")
				ENDIF()
			ENDIF()

			FOREACH(QT_FILE_NAME Qt5Core Qt5Gui Qt5Widgets Qt5PrintSupport Qt5Svg Qt5Xml libGLESv2 libEGL)
				SET(FILE_PATH ${QT5_BINARY_DIR}/${QT_FILE_NAME}${DEBUG_FILE_POSTFIX}.dll)
				IF(EXISTS "${FILE_PATH}")
   					INSTALL(FILES ${FILE_PATH} 
							DESTINATION ${INSTALL_BIN_PATH} 
							#COMPONENT DEMO
							CONFIGURATIONS ${CONFIG}
							)
				ENDIF()
			ENDFOREACH()
		
			FOREACH(QT_PLUGIN_FOLIDER imageformats platforms printsupport)
				IF(EXISTS "${QT5_BINARY_DIR}/../plugins/${QT_PLUGIN_FOLIDER}")
					SET(COPY_FILES)
					FILE(GLOB PLUGIN_FILES ${QT5_BINARY_DIR}/../plugins/${QT_PLUGIN_FOLIDER}/*.dll)

					#FOREACH(f ${PLUGIN_FILES})
					#	#string(REPLACE "d.dll" "${DEBUG_FILE_POSTFIX}.dll" out ${f})
					#	LIST(APPEND COPY_FILES ${out}) 
					#ENDFOREACH()

					LIST(APPEND COPY_FILES ${PLUGIN_FILES}) 

   					INSTALL(FILES ${COPY_FILES} 
							DESTINATION ${INSTALL_BIN_PATH}/${QT_PLUGIN_FOLIDER} 
							#COMPONENT DEMO
							CONFIGURATIONS ${CONFIG}
							)
				ENDIF()
			ENDFOREACH()
			
			INSTALL(DIRECTORY ${QT5_BINARY_DIR}/.
					DESTINATION ${INSTALL_BIN_PATH}
					#COMPONENT DEMO
					CONFIGURATIONS ${CONFIG}
					FILES_MATCHING PATTERN "icu*.dll"
						#PATTERN "icu*.dll"
						#PATTERN "icu*.dll"
					 
					)
		ENDFOREACH()
	ENDIF()
ENDFUNCTION()

FUNCTION(RPLM_INSTALL_POCO_BIN TARGET_NAME INSTALL_BIN_PATH)

	IF(WIN32)
		FOREACH(CONFIG ${CMAKE_CONFIGURATION_TYPES})
			SET(DEBUG_FILE_POSTFIX)
			IF(${CONFIG} STREQUAL Debug)
				SET(DEBUG_FILE_POSTFIX "d")
			ELSE()
				IF(${CONFIG} STREQUAL RelWithDebInfo)
					SET(DEBUG_FILE_POSTFIX "d")
				ENDIF()
			ENDIF()
		
			FOREACH(POCO_FILE_NAME libssl libcrypto PocoUtil64 PocoFoundation64 PocoSQL64 PocoCppUnit64 PocoNet64 PocoJSON64 PocoNetSSL64 PocoSQLPostgreSQL64 PocoXML64 PocoCrypto64)
				SET(FILE_PATH ${Poco_ROOT_DIR}/bin/${POCO_FILE_NAME}${DEBUG_FILE_POSTFIX}.dll)
				IF(EXISTS "${FILE_PATH}")
					INSTALL(FILES ${FILE_PATH} 
							DESTINATION ${INSTALL_BIN_PATH} 
							#COMPONENT DEMO
							CONFIGURATIONS ${CONFIG}
							)
				ENDIF()
			ENDFOREACH()

		ENDFOREACH()		
	ENDIF()
ENDFUNCTION()

FUNCTION(RPLM_INSTALL_POSTGRESQL_BIN TARGET_NAME INSTALL_BIN_PATH RPLM_CMAKE)
	
	IF(WIN32)	
		FOREACH(POSTGRESQL_FILE_NAME libeay32 libiconv libintl libpq ssleay32)
			SET(FILE_PATH ${RPLM_CMAKE}/External/PostgreSQL/${POSTGRESQL_FILE_NAME}.dll)
			IF(EXISTS "${FILE_PATH}")
				INSTALL(FILES ${FILE_PATH} 
						DESTINATION ${INSTALL_BIN_PATH} 
						#COMPONENT DEMO
						CONFIGURATIONS ${CONFIG}
						)
			
			ENDIF()
			ENDFOREACH()
	
	ENDIF()
ENDFUNCTION()

FUNCTION(RPLM_INSTALL_SERVER_CONFIG TARGET_NAME INSTALL_BIN_PATH)
	INSTALL(DIRECTORY  ${PROJECT_SOURCE_DIR}/${TARGET_NAME}/cert DESTINATION ${INSTALL_BIN_PATH} CONFIGURATIONS ${CONFIG})
	INSTALL(DIRECTORY  ${PROJECT_SOURCE_DIR}/${TARGET_NAME}/config DESTINATION ${INSTALL_BIN_PATH} CONFIGURATIONS ${CONFIG})
ENDFUNCTION()

FUNCTION(RPLM_INSTALL_FILESERVER_CONFIG TARGET_NAME INSTALL_BIN_PATH)
	INSTALL(DIRECTORY  ${PROJECT_SOURCE_DIR}/${TARGET_NAME}/config DESTINATION ${INSTALL_BIN_PATH} CONFIGURATIONS ${CONFIG})
ENDFUNCTION()

FUNCTION(RPLM_INSTALL_WIDGETS_CONFIG TARGET_NAME INSTALL_BIN_PATH)
	INSTALL(DIRECTORY  ${PROJECT_SOURCE_DIR}/${TARGET_NAME}/config DESTINATION ${INSTALL_BIN_PATH} CONFIGURATIONS ${CONFIG})
ENDFUNCTION()

FUNCTION(RPLM_INSTALL_FILE_CLIENT_CONFIG TARGET_NAME INSTALL_BIN_PATH)
	INSTALL(DIRECTORY  ${PROJECT_SOURCE_DIR}/${TARGET_NAME}/config DESTINATION ${INSTALL_BIN_PATH} CONFIGURATIONS ${CONFIG})
ENDFUNCTION()