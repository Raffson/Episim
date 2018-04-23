import pystride
from pystride.Simulation import Simulation
from subprocess import Popen, PIPE
import sys
import os
from datetime import datetime

def set_influenza(simulator):
    simulator.runConfig.setParameter("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml")
    simulator.runConfig.setParameter("run.behaviour_policy", "NoBehaviour")
    simulator.runConfig.setParameter("run.belief_policy.name", "NoBelief")
    simulator.runConfig.setParameter("run.contact_log_level", "None")
    simulator.runConfig.setParameter("run.contact_output_file", "false")
    simulator.runConfig.setParameter("run.disease_config_file", "disease_influenza.xml")
    simulator.runConfig.setParameter("run.global_information_policy", "NoGlobalInformation")
    simulator.runConfig.setParameter("run.holidays_file", "holidays_none.json")
    simulator.runConfig.setParameter("run.immunity_link_probability", 0)
    simulator.runConfig.setParameter("run.immunity_profile", "Random")
    simulator.runConfig.setParameter("run.immunity_rate", 0.0)
    simulator.runConfig.setParameter("run.local_information_policy", "NoLocalInformation")
    simulator.runConfig.setParameter("run.num_days", 30)
    simulator.runConfig.setParameter("run.num_participants_survey", 10)
    simulator.runConfig.setParameter("run.num_threads", 1)
    simulator.runConfig.setParameter("run.output_prefix", "BatchRuns")
    simulator.runConfig.setParameter("run.population_file", "pop_flanders600.csv")
    simulator.runConfig.setParameter("run.rng_seed", 2015)
    simulator.runConfig.setParameter("run.r0", 3.0)
    simulator.runConfig.setParameter("run.seeding_rate", 0.0009)
    simulator.runConfig.setParameter("run.seeding_age_min", 1)
    simulator.runConfig.setParameter("run.seeding_age_max", 99)
    simulator.runConfig.setParameter("run.start_date", "2017-01-01")
    simulator.runConfig.setParameter("run.stride_log_level", "info")
    simulator.runConfig.setParameter("run.track_index_case", False)
    simulator.runConfig.setParameter("run.use_install_dirs", True)
    simulator.runConfig.setParameter("run.vaccine_profile", "None")

def run_simulator(seed, num_days):
    """
    Runs the simulator once.
    Configures the num_days and seed parameter of the simulator.

    :param seed for the random number generator, used by the simulator
    :param num_days the number of days (steps) the simulator needs to run.
    """

    simulator = Simulation()
    simulator.loadRunConfig("../config/run_default_nolog.xml")
    set_influenza(simulator)
    simulator.runConfig.setParameter("rng_seed", seed)
    simulator.run(genFiles=False)


def main_runner(f, days):
    """
    reads the config file and test the seeds.
    :param file: config file we read from.

    """

    if( not os.path.exists(f) ):
        print("\033[91mError:\033[0m Input file \"%s\" for seeds not found, aborting...\n" % f)
        return

    writer = ""
    f = open(f, 'r')
    for line in f:
        line = line.rstrip()
        if line.isdigit():

            p1 = Popen(["python3", "seedTester.py", "--p", line, days], stdout=PIPE)
            output = p1.communicate()[0]
            p1.stdout.close()
            output = str(output)
            output = output.split("\\n")

            stride_out = output[len(output) - 8]
            output = line + "\t" + stride_out.split(":")[2].replace(" ", "").split("A")[0] + "\n"
            writer += output

    if not os.path.exists("seedTester/results"):
        os.makedirs("seedTester/results")
    time = str(datetime.now())
    time = time.split(".")[0].replace("-", "").replace(" ", "_").replace(":", "")
    with open("seedTester/results/"+time+".dat",'w') as f:
        f.write("# Seed\tInfected count\n")
        f.write(writer)


if __name__ == '__main__':

    if len(sys.argv) == 2 and sys.argv[1] == "--help":
        print("Usage of seedTester...\n\n" + \
                "First method, runs multiple seeds: python3 seedTester.py <path-to-file-with-seeds> <num-of-days>\n" + \
                "Where <path-to-file-with-seeds> is optional (default=seedTester/config/default_seeds.txt) " +\
                "and <num-of-days> optional (default=50).\n" +\
                "This method writes the results to a file called TIME.dat in a folder called seedTester/results.\n\n" +\
                "Second method, runs 1 seed only: python3 seedTester.py --p <seed> <num-of-days>\n" +\
                "Where <seed> is mandatory and <num-of-days> optional (default=50).\n")

    elif len(sys.argv) == 4 and sys.argv[1] == "--p":
        run_simulator(int(sys.argv[2]), int(sys.argv[3]))

    elif len(sys.argv) == 3 and sys.argv[1] == "--p":
        run_simulator(int(sys.argv[2]), 30)

    elif len(sys.argv) == 3:
        main_runner(sys.argv[1], sys.argv[2])

    elif len(sys.argv) == 2:
        main_runner(sys.argv[1], "30") #default 50 days...

    elif len(sys.argv) == 1:
        main_runner("seedTester/config/seeds-20180222_101948.txt", "30")

