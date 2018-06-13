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
#  Copyright 2018, Willem L, Kuylen E, Stijven S & Broeckhove J
#############################################################################

#============================================================================
# Build & install executable.
#============================================================================
set(SRC class_delivery.cpp
        flex_delivery.cpp
        main.cpp
)

add_executable(episim-bench ${SRC})
target_link_libraries(episim-bench myhayai libstride)
install(TARGETS episim-bench  DESTINATION   ${BIN_INSTALL_LOCATION})
unset(SRC)

#############################################################################
