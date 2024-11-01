﻿
FUNCTION(GENERATE_FOLDER_SG _FILES RELATIVE_DIR PREFIX_FOLDER_NAME)
	FOREACH(FILE_PATH ${_FILES})
		FILE(RELATIVE_PATH SRC_GR ${RELATIVE_DIR} ${FILE_PATH})
		#message(${FILE_PATH})
		SET(SRC_GR ${PREFIX_FOLDER_NAME}${SRC_GR})
		#message(${SRC_GR})
		#Extract the folder, ie remove the filename part
		STRING(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GR ${SRC_GR})
		#Source_group expects \\ (double antislash), not / (slash)
		STRING(REPLACE / \\ SRC_GR ${SRC_GR})
		SOURCE_GROUP("${SRC_GR}" FILES ${FILE_PATH})
	ENDFOREACH()
ENDFUNCTION()

FUNCTION(GENERATE_HEADERS SRC_PATH DST_PATH REL_PATH)
	FILE(GLOB_RECURSE API_FILES ${SRC_PATH}/*.h ${SRC_PATH}/*.hpp)
	#message(${SRC_PATH})
	#message(${DST_PATH})
	FOREACH(HFILE ${API_FILES})
		FILE(RELATIVE_PATH INCLUDE_PATH ${REL_PATH} ${HFILE})
		FILE(RELATIVE_PATH PATH_FILE ${SRC_PATH} ${HFILE})
		#message(${PATH_FILE})
		CONFIGURE_FILE(api.h.in ${DST_PATH}/${PATH_FILE})
	ENDFOREACH()
ENDFUNCTION()

MACRO(ADD_SUPPORT_OPENMP TARGET_NAME)
	FIND_PACKAGE(OpenMP)
	IF(OPENMP_FOUND)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	ENDIF()
ENDMACRO()

FUNCTION(ADD_COPY_FILES_COMMAND TARGET_NAME DST_PATH RELATIVE_COPY_PATH)
	FOREACH(it_PATH ${ARGN})
		IF(IS_ABSOLUTE ${it_PATH})
			SET(SRC_PATH ${it_PATH})
			#message(${SRC_PATH})
		ELSE()
			SET(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${it_PATH})
		ENDIF()
		
		IF(IS_DIRECTORY "${SRC_PATH}")
			FILE(GLOB_RECURSE SRC_FILES ${SRC_PATH}/*.*)
			FOREACH(it_FILE ${SRC_FILES})
				FILE(RELATIVE_PATH SRC_RELATIVE_FILE "${RELATIVE_COPY_PATH}" ${it_FILE})
				ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${it_FILE} "${DST_PATH}/${SRC_RELATIVE_FILE}")
			ENDFOREACH()
		ELSE()
			FILE(RELATIVE_PATH SRC_RELATIVE_FILE "${RELATIVE_COPY_PATH}" ${SRC_PATH})
			ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC_PATH} "${DST_PATH}/${SRC_RELATIVE_FILE}")
		ENDIF()
	ENDFOREACH()
ENDFUNCTION()

###############################################################################

FUNCTION(RPLM_ADD_SUBDIRECTORY SUB_DIR_NAME)
	IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIR_NAME})
        LIST(FIND RPLM_SKIP_DIRECTORIES ${SUB_DIR_NAME} RET)
        IF(RET EQUAL -1)
            ADD_SUBDIRECTORY(${SUB_DIR_NAME})
        ENDIF()
	ENDIF()
ENDFUNCTION()

FUNCTION(ADD_COPY_DEV_RESOURCES_COMMANDS TARGET_NAME)
	IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
		ADD_COPY_FILES_COMMAND(${TARGET_NAME} "$<TARGET_FILE_DIR:${TARGET_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}" "Resources")
	ENDIF()
ENDFUNCTION()

FUNCTION(ADD_COPY_DEV_FILES_COMMANDS)
	SET(ONE_VALUE_ARGS RELATIVE TARGET)
	SET(MULTI_VALUE_ARGS FILES)
	CMAKE_PARSE_ARGUMENTS(PARAM "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
	
	SET(RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
	IF(PARAM_RELATIVE)
		SET(RELATIVE_PATH ${PARAM_RELATIVE})
	ENDIF()
	
	ADD_COPY_FILES_COMMAND(${PARAM_TARGET} "$<TARGET_FILE_DIR:${PARAM_TARGET}>" "${RELATIVE_PATH}" "${PARAM_FILES}")
ENDFUNCTION()

