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
    sim/ControlHelper.cpp
	sim/SimRunner.cpp
    sim/Sim.cpp
    sim/SimBuilder.cpp
	sim/StanController.cpp
    sim/event/Id.cpp
	sim/event/Subject.cpp
#---
    util/Any.h
	util/BoxPlotData.h
    util/ConfigInfo.cpp
    util/CSV.cpp
	util/CSVRow.cpp
    util/FileSys.cpp
	util/is_iterator.h
	util/LogUtils.cpp
	util/PtreeUtils.h
    util/RNEngineType.cpp
    util/RNManager.cpp
	util/RnPcg.cpp
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
    viewers/AdoptedFileViewer.cpp
    viewers/InfectedFileViewer.cpp
    viewers/InfectedViewer.cpp
    viewers/CliViewer.cpp
	viewers/PersonsFileViewer.cpp
	viewers/SummaryFileViewer.cpp
#---
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
	set(CMAKE_AUTOMOC  ON)
	set(LIB_SRC ${LIB_SRC}
		viewers/MapViewer.cpp
			popgen-Episim/gui/QTBackEnd.cpp
			popgen-Episim/gui/QTCity.cpp
			popgen-Episim/gui/QTCommuter.cpp
			popgen-Episim/gui/QTCommunity.cpp
			popgen-Episim/gui/QTHouseHold.cpp
			popgen-Episim/gui/QTPopulation.cpp
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
	set(LIBS ${LIBS} Qt5::Core Qt5::Qml Qt5::Network Qt5::Quick Qt5::Positioning Qt5::Location Qt5::Charts)
	add_definitions(-DUSING_QT=TRUE)
endif()

#============================================================================
# Build and install the stride executable.
#============================================================================
add_executable(stride execs/main.cpp)
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Build the Myhayai library.
#============================================================================
add_subdirectory(myhayai)

#============================================================================
# Build and install charts showing the populations characteristics
#============================================================================
if(Qt5_FOUND)
	add_executable(PopChart popgen-Episim/PopVisualiser.cpp)
	target_link_libraries(PopChart ${LIBS} libstride ${Qt5Positioning_LIBRARIES} ${Qt5Charts_LIBRARIES} )
	install(TARGETS PopChart  DESTINATION   ${BIN_INSTALL_LOCATION})
endif()

#============================================================================
# Clean up.
#============================================================================
unset(LIB_SRC)

#############################################################################
