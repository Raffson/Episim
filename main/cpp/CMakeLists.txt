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
#  Copyright 2017, 2018 Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# Sources. Includes headers to enable code insight by CLion.
#============================================================================
set(LIB_SRC
#---
	behaviour/BeliefSeeder.cpp
	behaviour/behaviour_policies/NoBehaviour.h
	behaviour/behaviour_policies/Vaccination.h
	behaviour/belief_policies/Belief.h
	behaviour/belief_policies/HBM.h
	behaviour/belief_policies/Imitation.h
	behaviour/belief_policies/NoBelief.h
	behaviour/belief_policies/Threshold.h
	behaviour/information_policies/LocalDiscussion.h
	behaviour/information_policies/NoGlobalInformation.h
	behaviour/information_policies/NoLocalInformation.h
#---
    calendar/Calendar.cpp
    calendar/DaysOffAll.h
    calendar/DaysOffInterface.h
    calendar/DaysOffNone.h
    calendar/DaysOffSchool.h
    calendar/DaysOffStandard.h
#---
	contact/AgeContactProfile.cpp
	contact/AgeContactProfiles.h
	contact/ContactHandler.h
	contact/ContactLogMode.cpp
	contact/Infector.cpp
	contact/InfectorExec.h
	contact/InfectorMap.h
    contact/TransmissionProfile.cpp
#---
    disease/DiseaseSeeder.cpp
	disease/Health.cpp
    disease/HealthSeeder.cpp
	disease/Immunizer.cpp
#---
	output/AdoptedFile.cpp
    output/InfectedFile.cpp
    output/PersonsFile.cpp
	output/SummaryFile.cpp
#---
	pool/ContactPool.cpp
	pool/ContactPoolSys.h
	pool/ContactPoolType.cpp
    pool/IdSubscriptArray.h
#---
    pop/Age.h
    pop/Person.cpp
    pop/Population.cpp
	pop/PopBuilder.cpp
    pop/SurveySeeder.cpp
#---
    sim/CliController.cpp
	sim/SimRunner.cpp
    sim/Sim.cpp
    sim/SimBuilder.cpp
    sim/event/Id.cpp
	sim/event/Subject.cpp
#---
    util/Any.h
    util/ConfigInfo.cpp
    util/CSV.cpp
	util/CSVRow.cpp
    util/FileSys.cpp
	util/LogUtils.cpp
	util/RangeIndexer.h
    util/RNEngineType.cpp
    util/RNManager.cpp
    util/RunConfigManager.cpp
	util/Safeline.cpp
    util/SegmentedVector.h
    util/SliceIndexer.h
    util/Stopwatch.h
    util/StringUtils.h
    util/Subject.h
    util/SVIterator.h
    util/TimeStamp.h
    util/TimeToString.h
#---
	viewers/AdoptedViewer.cpp
	viewers/InfectedViewer.cpp
    viewers/CliViewer.cpp
    viewers/PersonsViewer.cpp
	viewers/SummaryViewer.cpp
#---
    popgen-Episim/main.cpp
    popgen-Episim/generator/GeoGridGenerator.cpp
    popgen-Episim/generator/PopulationGenerator.cpp
    popgen-Episim/model/Coordinate.cpp
    popgen-Episim/model/GeoGrid.cpp
    popgen-Episim/model/Community.cpp
    popgen-Episim/model/CommunityType.cpp
    popgen-Episim/model/Household.cpp
    popgen-Episim/model/City.cpp
    popgen-Episim/util/GeoGridFileWriter.cpp
    popgen-Episim/util/Parser.cpp
    popgen-Episim/util/Utility.cpp
	popgen-Episim/util/TestSummarizer.cpp
)

#============================================================================
# Add specific sources if Qt was found.
#============================================================================
if(Qt5_FOUND)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(CMAKE_AUTOUIC ON)
	set(LIB_SRC ${LIB_SRC}
		viewers/MapViewer.cpp
		RobbeGui/QTBackEnd.cpp
		RobbeGui/QTCity.cpp
		RobbeGui/QTCommuter.cpp
		)
endif()

#============================================================================
# Build the Stride library.
#============================================================================
add_library(libstride ${LIB_SRC})
set_source_files_properties(BeliefSeeder.cpp PROPERTIES COMPILE_FLAGS "-Wno-unused-variable")
target_link_libraries(libstride ${LIBS})
target_compile_options(libstride PUBLIC "-fPIC")
target_compile_definitions(libstride PUBLIC "-DSTRIDE_GIT_HASH=${STRIDE_WC_REVISION_HASH}")
set_target_properties(libstride PROPERTIES PREFIX "")

#============================================================================
# Link Qt stuff.
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

#============================================================================
# Build and install the stride executable.
#============================================================================
add_executable(stride sim/main.cpp)
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Build and install the Episim executable.
#============================================================================
add_executable(popgen-Episim popgen-Episim/main.cpp)
target_link_libraries(popgen-Episim ${LIBS} libstride)
install(TARGETS popgen-Episim  DESTINATION   ${BIN_INSTALL_LOCATION})

add_executable(seg_searcher popgen-Episim/seg_searcher.cpp)
target_link_libraries(seg_searcher ${LIBS} libstride)
install(TARGETS seg_searcher  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Build and install Robbe"s Testing excecutable
#============================================================================
add_executable(SandboxRobbe popgen-Episim/SandboxRobbe.cpp)
target_link_libraries(SandboxRobbe ${LIBS} libstride)
install(TARGETS SandboxRobbe  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(LIB_SRC)

#############################################################################
