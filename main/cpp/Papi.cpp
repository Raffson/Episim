/*
 * A simple example of the use of the high level PAPI_flops call.
 * PAPI_flops measures elapsed time, process time, floating point
 * instructions and MFLOP/s for code bracketted by calls to this routine.
 * For the following matrix multiply you should get 2*(INDEX^3) flpins
 * on Intel Pentium processors.
 */

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cerrno>
#include <sys/types.h>
#include <memory.h>
#include <malloc.h>
#include <vector>

#include "papi.h"
#include "omp.h"

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"
#include "util/FileSys.h"
#include "sim/SimRunner.h"

using namespace std;

int main() {
    boost::property_tree::ptree pt = stride::util::FileSys::ReadPtreeFile(
            string("../config/run_default.xml"));

    boost::property_tree::ptree pt_geo = stride::util::FileSys::ReadPtreeFile(
            string("../config/geogen_default.xml"));

    string base_command = "mkdir -p mem_data" ;
    string base = "mem_data";
    system(base_command.c_str());
    vector<string> rngs{"yarn2"}; // mrg3

    PAPI_dmem_info_t dmem;
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_thread_init(reinterpret_cast<unsigned long (*)(void)>(omp_get_thread_num));

    unsigned long pop = 1000000; // Test cases
        for (auto &rng: rngs){
            pt.put("run.rng_type", rng);
            cout << "Starting " << rng << endl;
            for (unsigned threads = 3; threads <= 3; threads++) {
                cout << "At thread " << to_string(threads) << endl;
                pt.put("run.num_threads", threads);
                string dir = base_command + "/" + rng + "/" + to_string(threads);
                system(dir.c_str());

                ofstream myfile;
                string filename = base + "/" + rng + "_" + to_string(threads)+ ".dat";
                myfile.open (filename);

                for (unsigned i = 0; i < 10; i++) { // Pop loop

                    cout << "With pop count " << to_string((i + 1) * pop) << endl;
                    pt_geo.put("pop_info.pop_total", (i + 1) * pop);




                    stride::GeoGridGenerator geo_gen;
                    auto grid = geo_gen.Generate(pt);
                    stride::PopulationGenerator pop_gen(*grid);
                    pop_gen.Generate();
                    stride::SimRunner rn(pt, grid->GetPopulation());
                    rn.Run();

                    PAPI_get_dmem_info(&dmem);
                    printf("Mem Size:\t\t%lld\n", dmem.size);
                    myfile << to_string((i + 1) * pop) << " " << to_string(dmem.size) << endl ;

                    ;
                }
                myfile.close();
            }
            PAPI_shutdown();
        }
    }