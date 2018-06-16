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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Main program: command line handling.
 */

#include "sim/CliController.h"
#include "util/FileSys.h"
#include "util/RunConfigManager.h"
#include "util/StringUtils.h"
#include "util/TimeStamp.h"

#include <tclap/CmdLine.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace TCLAP;
using namespace boost::property_tree;

/// Main program of the stride simulator.
int main(int argc, char** argv)
{
        int exitStatus = EXIT_SUCCESS;

        try {
                // -----------------------------------------------------------------------------------------
                // Parse command line.
                // -----------------------------------------------------------------------------------------
                CmdLine cmd("stride", ' ', "1.0");

                vector<string>           execs{"clean", "dump", "sim", "simgui", "geopop"};
                ValuesConstraint<string> vc(execs);
                string se = "Execute the indicated function. The clean function takes the configuration file "
                            "specified by the --config file=<file> parameter, cleans it (indent, sort) and "
                            "rewrites it to a new file. The dump function takes the built-in configuration "
                            "(used for testing/benchmarking) specified by the --config name=<name> parameter "
                            "and writes it, cleanly (i.e. indented, sorted) to a new file. The sim function "
                            "runs the simulator using the configuration specified by the --config parameter."
                            "The latter may use either --config file=<file> or --config name=<name>. The sim"
                            "function is the default. The simgui function runs the simulator with a graphical"
                            "interface. The geopop executes the geospatial synthetic population generator";
                ValueArg<string> execArg("e", "exec", se, false, "sim", &vc, cmd);

                string so = "Override configuration parameters in the configuration file. The format is "
                            "--override <name>=<value>. It can be used multiple times.";
                MultiArg<string> overrideArg("o", "override", so, false, "<NAME>=<VALUE>", cmd);

                string sc = "Specifies the run configuration parameters to be used. It may be either "
                            "-c file=<file> or -c name=<name>. The first is most commonly used and may be "
                            "shortened to -c <file>.";
                ValueArg<string> configArg("c", "config", sc, false, "run_default.xml", "RUN CONFIGURATION", cmd);

                string si = "Look for configuration file specified by the -c file=<file> or -c <file> in the "
                            "stride install directories";
                SwitchArg installedArg("i", "installed", si, cmd, true);

                vector<string>           opt1{"none", "step", "end"};
                ValuesConstraint<string> vc2(opt1);
                string sv = "Indicates which option should be passed to the mapviewer. By default, the mapviewer will "
                            "not be used. When using the step option, a map will be showed during each step of the "
                            "simulation. The end option will only show the map at the end of the simulation. "
                            "Note that this will automatically introduce a syntheticly generator population "
                            "since the populations for the regular simulator do not have geographical information.";
                ValueArg<string> mapArg("m", "mapviewer", sv, false, "none", &vc2, cmd);

                string sp = "Indicates whether or not snapshots should be taken from the map. By default, "
                        "no snapshots will be taken. If step is chosen, an image will be written to the "
                        "output folder within a seperate folder called 'png'. Each image will be called "
                        "X.png where X represents the day that was simulated. The end option will only "
                        "take a snapshot at the end of the simulation. "
                        "Note that this will automatically introduce a syntheticly generator population "
                        "since the populations for the regular simulator do not have geographical information.";
                ValueArg<string> pngArg("p", "png", sp, false, "none", &vc2, cmd);

                cmd.parse(argc, static_cast<const char* const*>(argv));

                // -----------------------------------------------------------------------------------------
                // Get configuration and path with overrides (if any).
                // -----------------------------------------------------------------------------------------
                auto  config = configArg.getValue();
                ptree configPt;

                if (regex_search(config, regex("^name="))) {
                        config   = regex_replace(config, regex(string("^name=")), string(""));
                        configPt = RunConfigManager::Create(config);
                } else {
                        config = regex_replace(config, regex(string("^file=")), string(""));
                        const boost::filesystem::path configPath =
                            (installedArg.getValue()) ? FileSys::GetConfigDir() /= config : config;
                        configPt = FileSys::ReadPtreeFile(configPath);
                }

                for (const auto& p_assignment : overrideArg.getValue()) {
                        const auto v = util::Tokenize(p_assignment, "=");
                        configPt.put("run." + v[0], v[1]);
                }

                // -----------------------------------------------------------------------------------------
                // If geopop ...
                // -----------------------------------------------------------------------------------------
                if (execArg.getValue() == "geopop" or mapArg.getValue() != "none" or pngArg.getValue() != "none") {
                        if (configPt.get<string>("run.output_prefix", "").empty()) {
                                configPt.put("run.output_prefix", TimeStamp().ToTag().append("/"));
                        }
                        configPt.put("run.random_geopop", true);
                        configPt.put("run.map_option", mapArg.getValue());
                        configPt.put("run.png_option", pngArg.getValue());
                        configPt.sort();

                        CliController(configPt).Control();
                }
                // -----------------------------------------------------------------------------------------
                // If run simulation in cli ...
                // -----------------------------------------------------------------------------------------
                else if (execArg.getValue() == "sim") {
                        if (configPt.get<string>("run.output_prefix", "").empty()) {
                                configPt.put("run.output_prefix", TimeStamp().ToTag().append("/"));
                        }
                        configPt.put("run.random_geopop", false); //override in case someone tries to mess with this...
                        configPt.sort();

                        CliController(configPt).Control();
                }
                // -----------------------------------------------------------------------------------------
                // If run simulation in gui ...
                // -----------------------------------------------------------------------------------------
                else if (execArg.getValue() == "simgui") {
                        if (configPt.get<string>("run.output_prefix", "").empty()) {
                            configPt.put("run.output_prefix", TimeStamp().ToTag().append("/"));
                        }
                        configPt.put("run.random_geopop", true);
                        configPt.sort();

                        CliController(configPt).ControlGui();
                }
                // -----------------------------------------------------------------------------------------
                // If clean/dump ...
                // -----------------------------------------------------------------------------------------
                else if (execArg.getValue() == "clean" || execArg.getValue() == "dump") {
                        RunConfigManager::CleanConfigFile(configPt);
                }
        } catch (exception& e) {
                exitStatus = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exitStatus = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }
        return exitStatus;
}
