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
#include "papi.h"

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"
#include "util/InstallDirs.h"

static void test_fail(char *file, int line, char *call, int retval);

int main(int argc, char **argv) {
    extern void dummy(void *);
    float real_time, proc_time, mflops;
    long long flpins;
    int retval;
    int i,j,k;


    /* Setup PAPI library and begin collecting data from the counters */
    if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK)
        test_fail(__FILE__, __LINE__, "PAPI_flops", retval);


    stride::GeoGridGenerator gen;
    auto grid = gen.Generate("../config/run_default.xml");
    stride::PopulationGenerator pop(*grid);
    pop.Generate();
    
    /* Collect the data into the variables passed in */
    if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK)
        test_fail(__FILE__, __LINE__, "PAPI_flops", retval);

    printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",
           real_time, proc_time, flpins, mflops);
    printf("%s\tPASSED\n", __FILE__);
    PAPI_shutdown();
    exit(0);
}

static void test_fail(char *file, int line, char *call, int retval){
    printf("%s\tFAILED\nLine # %d\n", file, line);
    if ( retval == PAPI_ESYS ) {
        char buf[128];
        memset( buf, '\0', sizeof(buf) );
        sprintf(buf, "System error in %s:", call );
        perror(buf);
    }
    else if ( retval > 0 ) {
        printf("Error calculating: %s\n", call );
    }
    else {
        printf("Error in %s: %s\n", call, PAPI_strerror(retval) );
    }
    printf("\n");
    exit(1);
}