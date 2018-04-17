/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 *
 *  This software has been (completely) rewritten starting from
 *  the hayai code by Nick Bruun. The original copyright, to be
 *  found in this directory still aplies.
 */
/**
 * @file
 * Impleme.
 */

#include "myhayai/CliController.hpp"

#include <exception>
#include <iostream>

using namespace std;
using namespace myhayai;

void MeaslesBench();

int main(int argc, char** argv)
{
        int exit_status = EXIT_SUCCESS;

        MeaslesBench();

        try {
                CliController controller;
                controller.ParseArgs(argc, argv);
                controller.Control();
                // Done.
        } catch (exception& e) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }

        return exit_status;
}
