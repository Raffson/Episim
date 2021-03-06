#################################################################################################
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
#  Copyright 2018, Willem L, Kuylen E & Broeckhove J
#################################################################################################

#================================================================================================
# Install Python scripts to the bin folder.
#================================================================================================

file(GLOB DEPLOY_FILES "seedTester")
INSTALL( DIRECTORY ${DEPLOY_FILES} DESTINATION ${BIN_INSTALL_LOCATION} )

INSTALL(FILES
		create_contactmatrix.py
		experimenter.py
		experimenter_r0.py
		interactive_maps.py
		log2csv.py
		LogParser.py
		plot_maps.py
		seedTester.py
		seedGenerator.py
	DESTINATION ${BIN_INSTALL_LOCATION}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ
)

#################################################################################################
