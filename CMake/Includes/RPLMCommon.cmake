set(RPLM_EXTERNAL ${RPLM_CMAKE}/External)

if (WIN32)
	set(CMAKE_DEBUG_POSTFIX ".d" CACHE STRING "Set RelWithDebInfo library postfix" FORCE)
	set(CMAKE_RELWITHDEBINFO_POSTFIX ".rd" CACHE STRING "Set RelWithDebInfo library postfix" FORCE)
endif()

include(CheckPlatform)
include(PCH)
include(RPLMDevMacros)
include(RPLMInstallMacros)
include(RPLMMiscMacros)
include(FindFreeImage)

option(RPLM_USE_PCH "Use precompiled headers." ON)
option(RPLM_UNIT_TEST "Generate projesct with unit test." ON)

##
option(USE_VS_SOLUTION_FOLDER "Use solution folder for vs" ON)
##
set(RPLM_SKIP_DIRECTORIES "" CACHE STRING "RPLM_ADD_SUBDIRECTORY ignores directories in this list")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Output/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Output/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Output/Lib)

set(CMAKE_INSTALL_RPATH "$ORIGIN/")

#Fix error create debug/release folder for VS. 
if(MSVC)
	foreach(config ${CMAKE_CONFIGURATION_TYPES})
		file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${config})
		file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${config})
	endforeach()
endif()

#Fix error for Qt glu32. 
if(WIN32)
    set(CMAKE_LIBRARY_PATH "C:/program files(x86)/windows kits/8.0/lib/win8/um/x64")
	list(APPEND CMAKE_LIBRARY_PATH "C:/program files (x86)/windows kits/8.0/lib/win8/um/x64")
endif()

include(${RPLM_CMAKE}/CMakeCompilerFlags.txt)

set(FOLDER_GROUP ${RPLM_FOLDER_GROUP})
set(USE_PCH ${RPLM_USE_PCH})
set(UNIT_TEST ${RPLM_UNIT_TEST})

###Application install
set(COMPILED_NAME)
GET_COMPILED_NAME(COMPILED_NAME)

set(APP_INSTALL_INCLUDE Include)
if(NOT APP_INSTALL_BIN)
	set(APP_INSTALL_BIN Bin)
endif()
set(APP_INSTALL_LIB Lib)

###API paths
set(API_PATH ${CMAKE_BINARY_DIR}/API)

set_property(GLOBAL PROPERTY USE_FOLDERS ${USE_VS_SOLUTION_FOLDER})

set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;RelWithDebInfo")

#------------------------------------------------
