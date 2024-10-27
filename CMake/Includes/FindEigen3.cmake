 
 FIND_PATH(EIGEN3_INCLUDE_DIR
    NAMES
        signature_of_eigen3_matrix_library
    PATHS
            ${EIGEN3_LOCATION}
            $ENV{EIGEN3_LOCATION}

            ${PROJECT_SOURCE_DIR}/extern/eigen
            ${PROJECT_SOURCE_DIR}/external/eigen

            ${PROJECT_SOURCE_DIR}/Extern/eigen
            ${PROJECT_SOURCE_DIR}/External/eigen

            # For RPLM.<Project name>/<Main|Dev>
            ${PROJECT_SOURCE_DIR}/../../../RPLM.Common/CMake/External/eigen
			${PROJECT_SOURCE_DIR}/../CMake/External/eigen

            DOC "The directory where Eigen3 resides")

if (EIGEN3_INCLUDE_DIR)
    set(EIGEN3_FOUND TRUE)
else()
    message(FATAL_ERROR "Eigen3 not found")
endif()