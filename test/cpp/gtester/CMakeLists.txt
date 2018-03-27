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
# Build & install the (OpenMP enabled if OpenMP available) executable.
#============================================================================
set( EXEC       gtester     )
set( SRC
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

set( EXEC2       gtester-episim     )
set( SRC2
		EpiSim-tests/main.cpp
		EpiSim-tests/Generate-Community-Test.cpp
		EpiSim-tests/Generate-Colleges-Test.cpp
		EpiSim-tests/Generate-Schools-Test.cpp
		EpiSim-tests/Generate-Workplaces-Test.cpp
		EpiSim-tests/GeoGrid-Constructor-Test.cpp
)

add_executable(${EXEC2}   ${SRC2} )
target_link_libraries( ${EXEC2}    ${LIBS} gtest pthread libstride)
install(TARGETS ${EXEC2}  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Define tests.
#============================================================================
add_test( NAME  ${EXEC}_all
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
