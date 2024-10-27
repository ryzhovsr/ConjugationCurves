 
 FIND_PATH(FPLATFORM_INCLUDE_DIR
	NAMES
        FPlatform.h
    PATHS
			${FPLATFORM_LOCATION}
			$ENV{FPLATFORM_LOCATION}
			
            ${PROJECT_SOURCE_DIR}/extern/FPlatform
			${PROJECT_SOURCE_DIR}/external/FPlatform
			${PROJECT_SOURCE_DIR}/external/FPlatform/Include

            ${PROJECT_SOURCE_DIR}/Extern/FPlatform
			${PROJECT_SOURCE_DIR}/External/FPlatform
			${PROJECT_SOURCE_DIR}/External/FPlatform/Include

			DOC "The directory where FPlatform.h resides")