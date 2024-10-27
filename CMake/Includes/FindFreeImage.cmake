#

# Try to find FREEIMAGE library and include path.
# Once done this will define
#
# FREEIMAGE_FOUND
# FREEIMAGE_INCLUDE_DIR
# FREEIMAGE_LIBRARY
# FREEIMAGE_SOURCE
#

INCLUDE(FindPackageHandleStandardArgs)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(ARCH "x64" CACHE STRING "CPU Architecture")
else()
  SET(ARCH "x86" CACHE STRING "CPU Architecture")
endif()

if (WIN32)
    FIND_PATH(FREEIMAGE_INCLUDE_DIR
        NAMES
            FreeImage.h
        PATHS
			${FREEIMAGE_LOCATION}
            ${FREEIMAGE_LOCATION}/Dist/x32
			
			$ENV{FREEIMAGE_LOCATION}
            $ENV{FREEIMAGE_LOCATION}/Dist/x32
			
            ${PROJECT_SOURCE_DIR}/extern/FreeImage/Include
			
			${PROJECT_SOURCE_DIR}/../../RPLM.Common/CMake/External/FreeImage/Include
			${PROJECT_SOURCE_DIR}/../../RPLM.Common/CMake/External/FreeImage/x32
			${PROJECT_SOURCE_DIR}/../../RPLM.Common/CMake/External/FreeImage/x64
            
            DOC "The directory where FreeImage.h resides")
			
    if(ARCH STREQUAL "x86")
      FIND_LIBRARY(FREEIMAGE_LIBRARY
          NAMES
              FreeImage
          PATHS
			  ${FREEIMAGE_LOCATION}
              ${FREEIMAGE_LOCATION}/Dist/x32
			  
			  $ENV{FREEIMAGE_LOCATION}
              $ENV{FREEIMAGE_LOCATION}/Dist/x32
			  
			  ${PROJECT_SOURCE_DIR}/extern/FreeImage/Dist/x32
              ${PROJECT_SOURCE_DIR}/extern/FreeImage/bin
              ${PROJECT_SOURCE_DIR}/extern/FreeImage/lib
              ${PROJECT_SOURCE_DIR}/extern/FreeImage/lib/x86
              ${PROJECT_SOURCE_DIR}/extern/FreeImage/lib/win32

			  ${PROJECT_SOURCE_DIR}/../../RPLM.Common/CMake/External/FreeImage/x32
              
              DOC "The FREEIMAGE library")
    else()
      FIND_LIBRARY(FREEIMAGE_LIBRARY
          NAMES
              FreeImage
          PATHS
			  ${FREEIMAGE_LOCATION}
              ${FREEIMAGE_LOCATION}/Dist/x64
			  
			  $ENV{FREEIMAGE_LOCATION}
              $ENV{FREEIMAGE_LOCATION}/Dist/x64
			  
  			  ${PROJECT_SOURCE_DIR}/extern/FreeImage/Dist/x64
			  ${PROJECT_SOURCE_DIR}/extern/FreeImage/bin
              ${PROJECT_SOURCE_DIR}/extern/FreeImage/lib
              ${PROJECT_SOURCE_DIR}/extern/FreeImage/lib/x64
              ${PROJECT_SOURCE_DIR}/extern/FreeImage/lib/win64
              
			  ${PROJECT_SOURCE_DIR}/../../RPLM.Common/CMake/External/FreeImage/x64
              
              DOC "The FREEIMAGE library")
    endif()
endif ()

if (${CMAKE_HOST_UNIX})
    find_path( FREEIMAGE_INCLUDE_DIR
        NAMES
            FreeImage.h freeimage.h
        PATHS
            ${FREEIMAGE_LOCATION}/include
            $ENV{FREEIMAGE_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
            DOC "The directory where FREEIMAGE resides"
    )
    find_library( FREEIMAGE_LIBRARY
        NAMES
            freeimage FreeImage
        PATHS
            ${FREEIMAGE_LOCATION}/lib
            $ENV{FREEIMAGE_LOCATION}/lib
			/usr/lib/x86_64-linux-gnu
            /usr/lib64
            /usr/lib
            /usr/local/lib64
            /usr/local/lib
            /sw/lib
            /opt/local/lib
            NO_DEFAULT_PATH
            DOC "The FREEIMAGE library")
endif ()

if(FREEIMAGE_SOURCE)
find_package_handle_standard_args(FREEIMAGE DEFAULT_MSG
    FREEIMAGE_INCLUDE_DIR
    FREEIMAGE_SOURCE
)
else()

find_package_handle_standard_args(FREEIMAGE DEFAULT_MSG
    FREEIMAGE_INCLUDE_DIR
    FREEIMAGE_LIBRARY
)
endif()

mark_as_advanced( FREEIMAGE_FOUND )