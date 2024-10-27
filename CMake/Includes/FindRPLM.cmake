# Distributed under the <...> Clause License.  See accompanying
# file Copyright.txt or https://<...>/licensing for details.

#.rst:
# FindRPLM
# -------
#
# Finds the RPLM components
#

# This will define the following variables::
#
#   RPLM_FOUND    - True if the system has the RPLM components
#   RPLM_VERSION  - The version of the RPLM components which was found
#
# and the following imported targets::
#
#   RPLM::OS            - The RPLM.Utils.OS library
#   RPLM::FreeImage     - The RPLM.Utils.FreeImage library
#   RPLM::XML           - The RPLM.Utils.XML library
#   RPLM::Kernel3D      - The RPLM.Math.Kernel3D library
#   RPLM::Geometry2D    - The RPLM.Math.Geometry2D library
#   RPLM::TestMain      - The gtest_main library
#   RPLM::Tests         - The gtest library
#   RPLM::SZip          - The RPLM.Utils.SZip library
#   RPLM::Graphics3D    - The RPLM.Graphics.3D library
#   RPLM::EPCommon      - The RPLM.EP.Common library
#   RPLM::EPModel       - The RPLM.EP.Model library
#   RPLM::EPMesh        - The RPLM.EP.Mesh library
#   RPLM::BaseFramework - The RPLM.Base.Framework library
#   RPLM::ShellUI       - The RPLM.Shell.UI library
#   RPLM::UIWidgets     - The RPLM.UI.Widgets library
#   RPLM::EPUI          - The RPLM.EP.UI library
#   RPLM::EPWidgets     - The RPLM.EP.Widgets library
#   RPLM::EPGraphics    - The RPLM.EP.Graphics library
#   RPLM::CADModel      - The RPLM.CAD.Model library
#   RPLM::CADUI         - The RPLM.CAD.UI library
#   RPLM::CADWidgets    - The RPLM.CAD.Widgets library
#   RPLM::ConstraintSolver - The RPLM.Math.ConstraintSolver

#find_package(PkgConfig)
#pkg_check_modules(PC_RPLM QUIET RPLM)

#set(RPLM_VERSION ${PC_RPLM_VERSION})

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set (ARCH "x64")
else ()
  set (ARCH "x86")
endif()

# In-source build
set(RPLM_BUILD_DIR_IN ${PROJECT_SOURCE_DIR}/../../install)
#
if(NOT EXISTS ${RPLM_BUILD_DIR_IN})
	set(RPLM_BUILD_DIR_IN ${PROJECT_SOURCE_DIR}/../../../../install)
endif()
#
if(NOT EXISTS ${RPLM_BUILD_DIR_IN})
	set(RPLM_BUILD_DIR_IN ${PROJECT_SOURCE_DIR}/../../../../../install)
endif()

# Out-of source build
set(RPLM_BUILD_DIR_OUT ${PROJECT_SOURCE_DIR}/../../../install)

if(WIN32)
	if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
		if(MSVC_VERSION GREATER_EQUAL 1910)
			set (COMPILER "vs17")
		endif()
	endif()
	
	set(RPLM_BUILD_DIR_IN ${RPLM_BUILD_DIR_IN}/${COMPILER}/${ARCH})
	set(RPLM_BUILD_DIR_OUT ${RPLM_BUILD_DIR_OUT}/${COMPILER}/${ARCH})

	if(EXISTS ${RPLM_BUILD_DIR_IN}/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_IN}/Include)
	elseif(EXISTS ${RPLM_BUILD_DIR_OUT}/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_OUT}/Include)
	endif()
elseif (UNIX)
	set (COMPILER "unix")
	set(RPLM_BUILD_DIR_IN ${RPLM_BUILD_DIR_IN}/${COMPILER})
	set(RPLM_BUILD_DIR_OUT ${RPLM_BUILD_DIR_OUT}/${COMPILER})

	if(EXISTS ${RPLM_BUILD_DIR_IN}/Release/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_IN}/Release/Include)
	elseif(EXISTS ${RPLM_BUILD_DIR_OUT}/Release/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_OUT}/Release/Include)
	elseif(EXISTS ${RPLM_BUILD_DIR_IN}/RelWithDebInfo/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_IN}/RelWithDebInfo/Include)
	elseif(EXISTS ${RPLM_BUILD_DIR_OUT}/RelWithDebInfo/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_OUT}/RelWithDebInfo/Include)
	elseif(EXISTS ${RPLM_BUILD_DIR_IN}/Debug/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_IN}/Debug/Include)
	elseif(EXISTS ${RPLM_BUILD_DIR_OUT}/Debug/Include)
		set(RPLM_INCLUDE_DIR  ${RPLM_BUILD_DIR_OUT}/Debug/Include)
	endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(RPLM
		  FOUND_VAR RPLM_FOUND
		  REQUIRED_VARS
			RPLM_INCLUDE_DIR
		  VERSION_VAR
			RPLM_VERSION
		)

mark_as_advanced(
  RPLM_INCLUDE_DIR
  )

function(find_rplm_component rplm_component_name rplm_library_name)
	list(FIND RPLM_FIND_COMPONENTS ${rplm_component_name} ${rplm_component_name}_requested)
	if(${${rplm_component_name}_requested} GREATER -1)

		set(RPLM_${rplm_component_name}_INCLUDE_DIR ${RPLM_INCLUDE_DIR}/${rplm_library_name})

		if(WIN32)
			find_library(RPLM_${rplm_component_name}_LIBRARY_RELEASE
				NAMES
					${rplm_library_name}.lib
				PATHS
					${RPLM_BUILD_DIR_IN}/Lib
					${RPLM_BUILD_DIR_OUT}/Lib)
			find_library(RPLM_${rplm_component_name}_LIBRARY_DEBUG
				NAMES
					${rplm_library_name}.d.lib
					${rplm_library_name}d.lib
				PATHS
					${RPLM_BUILD_DIR_IN}/Lib
					${RPLM_BUILD_DIR_OUT}/Lib)
			find_library(RPLM_${rplm_component_name}_LIBRARY_RELWITHDEBINFO
				NAMES
					${rplm_library_name}.rd.lib
				PATHS
					${RPLM_BUILD_DIR_IN}/Lib
					${RPLM_BUILD_DIR_OUT}/Lib)

		elseif (UNIX)

			find_library(RPLM_${rplm_component_name}_LIBRARY_RELEASE
				NAMES
					${rplm_library_name}
					lib${rplm_library_name} 
					${rplm_library_name}.so
					lib${rplm_library_name}.so
					${rplm_library_name}.a
					lib${rplm_library_name}.a
				PATHS
					${RPLM_BUILD_DIR_IN}/Release/Bin
					${RPLM_BUILD_DIR_OUT}/Release/Bin
					${RPLM_BUILD_DIR_IN}/Release/Lib
					${RPLM_BUILD_DIR_OUT}/Release/Lib)

			find_library(RPLM_${rplm_component_name}_LIBRARY_DEBUG
				NAMES
					${rplm_library_name}
					lib${rplm_library_name} 
					${rplm_library_name}.so
					lib${rplm_library_name}.so
					${rplm_library_name}.a
					lib${rplm_library_name}.a
				PATHS
					${RPLM_BUILD_DIR_IN}/Debug/Bin
					${RPLM_BUILD_DIR_OUT}/Debug/Bin
					${RPLM_BUILD_DIR_IN}/Debug/Lib
					${RPLM_BUILD_DIR_OUT}/Debug/Lib
					${RPLM_BUILD_DIR_IN}/RelWithDebInfo/Bin
					${RPLM_BUILD_DIR_OUT}/RelWithDebInfo/Bin
					${RPLM_BUILD_DIR_IN}/RelWithDebInfo/Lib
					${RPLM_BUILD_DIR_OUT}/RelWithDebInfo/Lib)

			find_library(RPLM_${rplm_component_name}_LIBRARY_RELWITHDEBINFO
				NAMES
					${rplm_library_name}
					lib${rplm_library_name} 
					${rplm_library_name}.so
					lib${rplm_library_name}.so
					${rplm_library_name}.a
					lib${rplm_library_name}.a
				PATHS
					${RPLM_BUILD_DIR_IN}/RelWithDebInfo/Bin
					${RPLM_BUILD_DIR_OUT}/RelWithDebInfo/Bin
					${RPLM_BUILD_DIR_IN}/RelWithDebInfo/Lib
					${RPLM_BUILD_DIR_OUT}/RelWithDebInfo/Lib)

		endif()

		if(RPLM_${rplm_component_name}_LIBRARY_RELEASE)
			find_package_handle_standard_args(RPLM_${rplm_component_name}
			  FOUND_VAR RPLM_${rplm_component_name}_FOUND
			  REQUIRED_VARS
				RPLM_${rplm_component_name}_LIBRARY_RELEASE
			  VERSION_VAR RPLM_VERSION
			)
		elseif(RPLM_${rplm_component_name}_LIBRARY_DEBUG)
			find_package_handle_standard_args(RPLM_${rplm_component_name}
				  FOUND_VAR RPLM_${rplm_component_name}_FOUND
				  REQUIRED_VARS
					RPLM_${rplm_component_name}_LIBRARY_DEBUG
				  VERSION_VAR RPLM_VERSION
				)
		elseif(RPLM_${rplm_component_name}_LIBRARY_RELWITHDEBINFO)
			find_package_handle_standard_args(RPLM_${rplm_component_name}
				  FOUND_VAR RPLM_${rplm_component_name}_FOUND
				  REQUIRED_VARS
					RPLM_${rplm_component_name}_LIBRARY_RELWITHDEBINFO
				  VERSION_VAR RPLM_VERSION
				)
		endif()

		mark_as_advanced(
			RPLM_${rplm_component_name}_INCLUDE_DIR
			RPLM_${rplm_component_name}_LIBRARY_RELEASE
			RPLM_${rplm_component_name}_LIBRARY_DEBUG
			RPLM_${rplm_component_name}_LIBRARY_RELWITHDEBINFO)

		if (NOT TARGET RPLM::${rplm_component_name})
			add_library(RPLM::${rplm_component_name} UNKNOWN IMPORTED)
		endif()


		if(RPLM_${rplm_component_name}_INCLUDE_DIR)
			set_property(TARGET RPLM::${rplm_component_name} APPEND PROPERTY
				IMPORTED_CONFIGURATIONS RELEASE
			)
			set_target_properties(RPLM::${rplm_component_name} PROPERTIES
				IMPORTED_LOCATION_RELEASE ${RPLM_${rplm_component_name}_LIBRARY_RELEASE}
			)
		endif()
		if(RPLM_${rplm_component_name}_LIBRARY_RELEASE)
			set_property(TARGET RPLM::${rplm_component_name} APPEND PROPERTY
				IMPORTED_CONFIGURATIONS RELEASE
			)
			set_target_properties(RPLM::${rplm_component_name} PROPERTIES
				IMPORTED_LOCATION_RELEASE ${RPLM_${rplm_component_name}_LIBRARY_RELEASE}
			)
		endif()
		if(RPLM_${rplm_component_name}_LIBRARY_DEBUG)
			set_property(TARGET RPLM::${rplm_component_name} APPEND PROPERTY
				IMPORTED_CONFIGURATIONS DEBUG
			)
			set_target_properties(RPLM::${rplm_component_name} PROPERTIES
				IMPORTED_LOCATION_DEBUG ${RPLM_${rplm_component_name}_LIBRARY_DEBUG}
			)
		endif()
		if(RPLM_${rplm_component_name}_LIBRARY_RELWITHDEBINFO)
			set_property(TARGET RPLM::${rplm_component_name} APPEND PROPERTY
				IMPORTED_CONFIGURATIONS RELWITHDEBINFO
			)
			set_target_properties(RPLM::${rplm_component_name} PROPERTIES
				IMPORTED_LOCATION_RELWITHDEBINFO ${RPLM_${rplm_component_name}_LIBRARY_RELWITHDEBINFO}
			)
		endif()

		set_target_properties(RPLM::${rplm_component_name} PROPERTIES
		INTERFACE_COMPILE_OPTIONS "${PC_RPLM_CFLAGS_OTHER}"
		INTERFACE_INCLUDE_DIRECTORIES "${RPLM_INCLUDE_DIR}")
		
	endif()
endfunction(find_rplm_component)

if(RPLM_FOUND)
	find_rplm_component("OS" "RPLM.Utils.OS")
	find_rplm_component("FreeImage" "RPLM.Utils.FreeImage")
	find_rplm_component("XML" "RPLM.Utils.XML")
	find_rplm_component("Kernel3D" "RPLM.Math.Kernel3D")
	find_rplm_component("Geometry2D" "RPLM.Math.Geometry2D")
	find_rplm_component("TestMain" "gtest_main")
	find_rplm_component("Tests" "gtest")
	find_rplm_component("SZip" "RPLM.Utils.SZip")
	find_rplm_component("Graphics3D" "RPLM.Graphics.3D")
	find_rplm_component("EPCommon" "RPLM.EP.Common")
	find_rplm_component("EPModel" "RPLM.EP.Model")    
	find_rplm_component("EPMesh" "RPLM.EP.Mesh")     
	find_rplm_component("BaseFramework" "RPLM.Base.Framework")
	find_rplm_component("ShellUI" "RPLM.Shell.UI")
	find_rplm_component("UIWidgets" "RPLM.UI.Widgets")
	find_rplm_component("EPUI" "RPLM.EP.UI")
	find_rplm_component("EPWidgets" "RPLM.EP.Widgets")
	find_rplm_component("EPGraphics" "RPLM.EP.Graphics")
	find_rplm_component("CADModel" "RPLM.CAD.Model")
	find_rplm_component("CADUI" "RPLM.CAD.UI")
	find_rplm_component("CADWidgets" "RPLM.CAD.Widgets")
	find_rplm_component("ConstraintSolver" "RPLM.Math.ConstraintSolver")
endif()
