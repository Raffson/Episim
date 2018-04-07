#############################################################################
#  This is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or any 
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2017, Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# Test verbosity:
#============================================================================
unset(CTEST_VERBOSE_OPTION)
if( STRIDE_VERBOSE_TESTING )
	set( CTEST_VERBOSE_OPTION "-V" )
endif()

#============================================================================
# Make test directory in install tree:
#============================================================================
set(TESTS_DIR ${CMAKE_INSTALL_PREFIX}/${TESTS_INSTALL_LOCATION})
install(DIRECTORY DESTINATION  ${TESTS_DIR})

#============================================================================
# Top level custom target for instactive use through make:
#============================================================================
add_custom_target(run_ctest
	COMMAND rm -rf ${TESTS_DIR}/smoke_test
	COMMAND ctest ${CTEST_VERBOSE_OPTION} -O ${TESTS_DIR}/ctests.txt
	WORKING_DIR ${TESTS_DIR}
	)

add_custom_target(run_ctest_py
	COMMAND ctest ${CTEST_VERBOSE_OPTION} -R py* -O ${TESTS_DIR}/ctests.txt
	WORKING_DIR ${TESTS_DIR}
	)

#output isn't written, gotta figure this out...
#also, need to figure out a way to copy only the files that have been changed...
add_custom_target( episim-test
	COMMAND ${CMAKE_COMMAND} -E remove_directory
		${CMAKE_BINARY_DIR}/test/config
	COMMAND ${CMAKE_COMMAND} -E remove_directory
		${CMAKE_BINARY_DIR}/test/data
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/main/resources/EpiSim-test-files/config/
		${CMAKE_BINARY_DIR}/test/config
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/main/resources/EpiSim-test-files/data/
		${CMAKE_BINARY_DIR}/test/data
	COMMAND gtester-episim 
	${CTEST_VERBOSE_OPTION} -O ${TESTS_DIR}/EpiSim/etests.txt
	WORKING_DIR ${TESTS_DIR}
	COMMENT "Running EpiSim's tests..."
)

#============================================================================
# Config related to using Google test (src in resources/lib/gtest) for cpp
#============================================================================
set(GTEST_RESOURCE_DIR	   ${CMAKE_SOURCE_DIR}/test/resources/lib/gtest/)
include_directories(SYSTEM ${GTEST_RESOURCE_DIR})
add_definitions(-DGTEST_LANG_CXX11)

#============================================================================
# Add subdirectories:
#============================================================================
add_subdirectory( cpp       )
add_subdirectory( python    )
add_subdirectory( resources )

#############################################################################
