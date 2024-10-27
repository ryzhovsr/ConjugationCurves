 
 FIND_PATH(NLOHMANN_INCLUDE_DIR
    NAMES
        signature_of_nlohmann_library
    PATHS
            ${NLOHMANN_LOCATION}
            $ENV{NLOHMANN_LOCATION}

            ${PROJECT_SOURCE_DIR}/extern/nlohmann
            ${PROJECT_SOURCE_DIR}/external/nlohmann

            ${PROJECT_SOURCE_DIR}/Extern/nlohmann
            ${PROJECT_SOURCE_DIR}/External/nlohmann

            # For RPLM.<Project name>/<Main|Dev>
            ${PROJECT_SOURCE_DIR}/../../../RPLM.Common/CMake/External/nlohmann
			${PROJECT_SOURCE_DIR}/../CMake/External/nlohmann

            DOC "The directory where nlohmann resides")

if (NLOHMANN_INCLUDE_DIR)
    set(NLOHMANN_FOUND TRUE)
else()
    message(FATAL_ERROR "Nlohmann not found")
endif()