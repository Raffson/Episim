"""
Script to generate a config file for seedTester.py, will generate an amount of given (unique) seeds.

Script has optional arguments (order of occurrence does not matter):

    - --help which explains the usage and quits, can occur at any position.
    - optional integer (e.g. --seed=5000): amount of seeds to generate per file, default = 1000.
    - optional integer (e.g. --files=2): amount of files to generate, default = 1.
    - optional filename (e.g. --name=example): file to write seeds to, default = "seed-TIME.txt".

    TODO:
    - optional seed generation profile (e.g. --type=linear): choice between 'linear', 'noise1', 'noise2' & 'random'
        default = 'random', any other input will result back into 'random'
        generates only 1 file with specific name, overriding the other options...
        Types:
            linear ->  seed(i) = floor((4294967295/(nr-of-seeds))*i) for i = 0..((nr-of-seeds)-1)
            noise1 ->  seed(i) = floor((4294967295/(nr-of-seeds))*i)+floor(75000*(cos(i)*sin(i)))
            noise2 ->  seed(i) = floor((4294967295/(nr-of-seeds))*i)+floor(75000*(cos(-i)*sin(-i)))
            random ->  seed(i) = random number between 0 and 4294967295

        noise1 and noise2 introduce a wave into the linear curve, kind of like noise on data, hence the name...
        the 75000 can be adjusted to more values to noise on a smaller/larger scale
"""
import sys
import random
import os
from datetime import datetime
import string
from math import cos, sin, floor

def is_int(i, default):
    try:
        int(i)
        return True
    except ValueError:
        print("\033[91mError:\033[0m Bad parameter \"%s\" given, using default value of \"%s\".\n" % (i, default))
        return False

def argsContains(args, argtype):
    for a in args:
        if( argtype in a ):
            value = None
            try:
                value = a.split("=")[1]
                return (True, value)
            except IndexError:
                return (False, 0)
    return (False, 0)

if __name__ == "__main__":

    try:
        if( len(sys.argv) >= 2 and "--help" in sys.argv ):
            sys.exit("Usage: python3 seedGenerator.py --seeds=INT --files=INT --name=CHARSEQUENCE --type=TYPE\n" +\
                    "Where --seeds=INT is optional (default=1000), representing the number of seeds per file.\n" +\
                    "Where --files=INT is optional (default=1), representing the number of files to be generated.\n" +\
                    "Where --name=CHARSEQUENCE is optional, representing the filename used to write in \"seedTester/config\".\n" +\
                    "Where --type=TYPE is optional, representing the type of seed generation.\n" +\
                    "TYPE can be 'linear', 'noise1, 'noise2' or 'random'. Default value for TYPE is 'random'." +\
                    "If TYPE is different from 'random', only one file is generated.\n" +\
                    "The order in which the options are given does not matter.\n")

        gentype = "random"
        present, value = argsContains(sys.argv, "--type")
        if( len(sys.argv) >= 2 and present ):
            if( value in ["linear", "noise1", "noise2"] ):
                gentype = value

        seed_amount = 1000
        present, value = argsContains(sys.argv, "--seeds")
        if( len(sys.argv) >= 2 and present and is_int(value, seed_amount) ):
            seed_amount = abs(int(value)) #in case someone tries to mess with negative numbers...

        nrfiles = 1
        present, value = argsContains(sys.argv, "--files")
        if( len(sys.argv) >= 2 and present and is_int(value, nrfiles) ):
            nrfiles = abs(int(value))

        if not os.path.exists("seedTester"):  # testing if write dirs exist, else create
            os.makedirs("seedTester")

        if not os.path.exists("seedTester/config"):
            os.makedirs("seedTester/config")

        prefix = ""
        present, value = argsContains(sys.argv, "--name")
        if len(sys.argv) >= 2 and present:
            prefix = value
            #filter illegal characters...
            #thus this allows for custom filenames to be written to "config/seedTester"
            valid_chars = "-_.()%s%s" % (string.ascii_letters, string.digits)
            prefix = ''.join(c for c in prefix if c in valid_chars)
            if( prefix != value ):
                print("Filename was adjusted from \"%s\" to \"%s\"."%(value, prefix))

        if( gentype != "random" ):
            f = lambda x: ((4294967295/seed_amount)*x) #since we're dealing with integer division, no need for floor function...
            if( gentype == "noise1" ):
                f = lambda x: (((4294967295/seed_amount)*x)+floor(75000*cos(x)*sin(x)))
            elif( gentype == "noise2" ):
                f = lambda x: (((4294967295/seed_amount)*x)+floor(75000*cos(-x)*sin(-x)))

            filename = "seedTester/config/{0}.txt".format(gentype)
            with open(filename, "w+") as file:
                file.write("{0} seed generation\n".format(gentype))
                for i in range(seed_amount):
                    nextseed = int(f(i)) #just to be sure...
                    file.write("{0}\n".format(nextseed))
        else:
            #random generation of seeds
            for i in range(nrfiles):
                time = str(datetime.now())
                time = time.split(".")[0].replace("-", "").replace(" ", "_").replace(":", "")
                filename = "seeds-"+time+".txt"
                if( present ):
                    filename = prefix + "-" + str(i) + ".txt"

                filename = "seedTester/config/{0}".format(filename)

                seeds = []
                #make sure seeds are unique within their respective file
                while( len(seeds) < seed_amount):
                    rand_int = random.randint(0, 4294967295) #because seed is an unsigned long
                    if( rand_int not in seeds ):
                        seeds.append(rand_int)

                with open(filename, "w+") as file:
                    file.write("random seed generation\n".format(gentype))
                    for i in range(seed_amount):
                        file.write("{0}\n".format(seeds[i]))

    except IndexError:
        sys.exit("\033[91mError:\033[0m No seed amount given, try using --help...\n")

    except IOError:
        sys.exit("\033[91mError:\033[0m Given filepath does not exist, try using --help...\n")
