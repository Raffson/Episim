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
#  Copyright 2017, Willem L, Kuylen E, Stijven S & Broeckhove J
#############################################################################

#============================================================================
# Config related to using Google test (src in resources/lib/gtest) for cpp
#============================================================================
set(GTEST_RESOURCE_DIR	   ${CMAKE_SOURCE_DIR}/test/resources/lib/gtest/)
include_directories(SYSTEM ${GTEST_RESOURCE_DIR})
add_definitions(-DGTEST_LANG_CXX11)

#============================================================================
# Build & install the (OpenMP enabled if OpenMP available) executable.
#============================================================================
set(EXEC gtester)
set(SRC
    main.cpp
    BatchRuns.cpp
    ScenarioData.cpp
    UnitSegmentedVector.cpp
)
if (OPENMP_FOUND)
    set(SRC ${SRC} ParallelRuns.cpp)
endif()

add_executable(${EXEC}   ${SRC} )
target_link_libraries( ${EXEC}    ${LIBS} gtest pthread libstride)
install(TARGETS ${EXEC}  DESTINATION   ${BIN_INSTALL_LOCATION})


#output isn't written, gotta figure this out...
#also, need to figure out a way to copy only the files that have been changed...
add_custom_target( episim-test
	COMMAND ${CMAKE_COMMAND} -E remove_directory
		${CMAKE_BINARY_DIR}/test/cpp/gtester/config
	COMMAND ${CMAKE_COMMAND} -E remove_directory
		${CMAKE_BINARY_DIR}/test/cpp/gtester/data
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/main/resources/EpiSim-test-files/config/
		${CMAKE_BINARY_DIR}/test/cpp/gtester/config
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/main/resources/EpiSim-test-files/data/
		${CMAKE_BINARY_DIR}/test/cpp/gtester/data
	COMMAND gtester-episim 
	${CTEST_VERBOSE_OPTION} -O ${TESTS_DIR}/EpiSim/etests.txt
	WORKING_DIR ${TESTS_DIR}
	COMMENT "Running EpiSim's tests..."
)

set( EXEC2       gtester-episim     )
set( SRC2
		EpiSim-tests/main.cpp
		EpiSim-tests/Generate-Community-Test.cpp
		EpiSim-tests/Generate-Colleges-Test.cpp
		EpiSim-tests/Generate-Schools-Test.cpp
		EpiSim-tests/Generate-Workplaces-Test.cpp
		EpiSim-tests/GeoGrid-Constructor-Test.cpp
        EpiSim-tests/Defrag-Cities-Test.cpp)

add_executable(${EXEC2}   ${SRC2} )
target_link_libraries( ${EXEC2}    ${LIBS} gtest pthread libstride)
install(TARGETS ${EXEC2}  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Define tests.
#============================================================================
add_test(NAME  ${EXEC}
    WORKING_DIRECTORY  ${TESTS_DIR}
    COMMAND   ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/${EXEC}
        --gtest_output=xml:gtester_all.xml
)

#output is still not written to install-dir/tests
add_test( NAME  ${EXEC2}
        WORKING_DIRECTORY  ${CMAKE_INSTALL_PREFIX}
		COMMAND   ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/${EXEC2}
)

#============================================================================
# Clean up.
#============================================================================
unset( EXEC      )
unset( SRC       )
unset( EXEC2      )
unset( SRC2       )

#############################################################################
