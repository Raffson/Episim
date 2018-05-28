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
    UnitRange.cpp
    UnitSlice.cpp
    UnitSegmentedVector.cpp
    UnitSVIterator.cpp
)
if (OPENMP_FOUND)
    set(SRC ${SRC} ParallelRuns.cpp)
endif()

add_executable(${EXEC}   ${SRC} )
target_link_libraries( ${EXEC}    ${LIBS} gtest pthread libstride)
install(TARGETS ${EXEC}  DESTINATION   ${BIN_INSTALL_LOCATION})


set( EXEC2       gtester-episim     )
set( SRC2
		EpiSim-tests/main.cpp
		EpiSim-tests/Generate-Community-Test.cpp
		EpiSim-tests/Generate-Colleges-Test.cpp
		EpiSim-tests/Generate-Schools-Test.cpp
		EpiSim-tests/Generate-Workplaces-Test.cpp
		EpiSim-tests/GeoGrid-Constructor-Test.cpp
        EpiSim-tests/Defrag-Cities-Test.cpp
		EpiSim-tests/GeneratePopTest.cpp)

add_executable(${EXEC2}   ${SRC2} )
target_link_libraries( ${EXEC2}    ${LIBS} gtest pthread libstride)
install(TARGETS ${EXEC2}  DESTINATION   ${BIN_INSTALL_LOCATION})

add_custom_target( episim-test
	COMMAND ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/${EXEC2}
		--gtest_output=xml:${TESTS_DIR}/all_tests_episim.xml
	WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
	COMMENT "Running EpiSim's tests..."
)

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
