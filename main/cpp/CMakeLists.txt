#############################################################################
#  This file is part of the Stride software. 
#  It is free software: you can redistribute it and/or modify
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
# Sources.
#============================================================================
set(LIB_SRC
#---
    calendar/Calendar.cpp
#---
	core/ContactLogMode.cpp
	core/ContactPool.cpp
	core/ContactPoolType.cpp
	core/ContactProfile.cpp
	core/DiseaseProfile.cpp
	core/Health.cpp
	core/Infector.cpp
#---
	immunity/Immunizer.cpp
	immunity/Vaccinator.cpp
#---
	output/AdoptedFile.cpp
	output/CasesFile.cpp
	output/PersonsFile.cpp
	output/SummaryFile.cpp
#---	
	pop/Person.cpp
	pop/Population.cpp
	pop/PopulationBuilder.cpp
#---
	sim/CliController.cpp
	sim/SimRunner.cpp
	sim/Simulator.cpp
	sim/SimulatorBuilder.cpp
	#sim/TestsController.cpp
	sim/event/Id.cpp
	sim/event/Subject.cpp
	sim/python/StrideRunner.cpp
#---
	util/CSV.cpp
	util/CSVRow.cpp
	util/FileSys.cpp
	util/LogUtils.cpp
	util/PtreeUtils.cpp
	util/RNEngineType.cpp
	util/RNManager.cpp
	util/RunConfigPtree.cpp
	util/RunConfigString.cpp
	util/Safeline.cpp

	popgen-Episim/main.cpp
	popgen-Episim/GeoGen/Coordinate.cpp
	popgen-Episim/GeoGen/GeoGrid.cpp
	popgen-Episim/GeoGen/Community.cpp
	popgen-Episim/GeoGen/Household.cpp
	popgen-Episim/GeoGen/City.cpp
	popgen-Episim/GeoGen/Parser.cpp
	#popgen-Episim/GeoGen/CommunityTypes/School.cpp
	#popgen-Episim/GeoGen/CommunityTypes/ContactPool.cpp
	#popgen-Episim/GeoGen/CommunityTypes/PrimSec.cpp
	popgen-Episim/GeoGen/Utility.cpp
#---
	viewers/AdoptedViewer.cpp
	viewers/CasesViewer.cpp
	viewers/CliViewer.cpp
	viewers/PersonsViewer.cpp
	viewers/SummaryViewer.cpp
)

#============================================================================
# Build the Stride library.
#============================================================================
add_library(libstride ${LIB_SRC})
target_link_libraries(libstride ${LIBS})
target_compile_options(libstride PUBLIC "-fPIC")
set_target_properties(libstride PROPERTIES PREFIX "")

#============================================================================
# Build and install the stride executable.
#============================================================================
add_executable(stride sim/main.cpp)
include_directories(SYSTEM ${CMAKE_HOME_DIRECTORY}/main/resources/lib/tclap/include)
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Build and install the Episim executable.
#============================================================================
if(Qt5_FOUND)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	include_directories(${Qt5Core_INCLUDE_DIRS})
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	set(LIBS ${LIBS} Qt5::Core Qt5::Qml Qt5::Network Qt5::Quick Qt5::Positioning Qt5::Location)
	add_definitions(-DUSING_QT=TRUE)
endif()

add_executable(popgen-Episim popgen-Episim/main.cpp)
target_link_libraries(popgen-Episim ${LIBS} libstride)
install(TARGETS popgen-Episim  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(MAIN_SRC)

#############################################################################
