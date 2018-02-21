import pystride
from pystride.Simulation import Simulation
from subprocess import Popen, PIPE
import sys



def run_simulator(seed, num_days):
    """
    Runs the simulator once.
    Configures the num_days and seed parameter of the simulator.

    :param seed for the random number generator, used by the simulator
    :param num_days the number of days (steps) the simulator needs to run.
    """

    simulator = Simulation()
    simulator.loadRunConfig("config/run_default.xml")
    simulator.runConfig.setParameter("num_days", num_days)

    simulator.runConfig.setParameter("rng_seed", seed)

    simulator.run()


def main_runner(file, days):
    """
    reads the config file and test the seeds.
    :param file: config file we read from.
    """
    writer = ""
    file = open(file, 'r')
    for line in file:
        line = line.rstrip()
        if line.isdigit():

            p1 = Popen(["python3", "seedTester.py", "--p", line, days], stdout=PIPE)
            output = p1.communicate()[0]
            p1.stdout.close()
            output = str(output)
            output = output.split("\\n")

            print(output[len(output) - 8])
            output = "Seednumber:" + line + "\n" + output[len(output) - 8] + "\n\n"
            writer += output

    with open("result.txt",'w') as f:
        f.write(writer)


if __name__ == '__main__':

    if sys.argv[1] == "--p":
        run_simulator(int(sys.argv[2]), int(sys.argv[3]))

    elif len(sys.argv) == 3:
        main_runner(sys.argv[1], sys.argv[2])

