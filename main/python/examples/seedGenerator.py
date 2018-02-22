"""
Script to generate a config file for seedTester.py, will generate an amount of given seeds.

Script has optional arguments (order of occurrence does not matter):

    - --help which explains the usage and quits, can occur at any position.
    - optional integer (e.g. --seed=5000): amount of seeds to generate per file, default = 1000.
    - optional integer (e.g. --files=2): amount of files to generate, default = 1.
    - optional filename (e.g. --name=example): file to write seeds to, default "seed-TIME.txt".

    TODO:
    - optional seed generation profile, can occur at any position:
        only (unique) randomly generated seeds are available at this point.
        considering "--linear", i.e. seed(i) = floor((4294967295/(nr-of-seeds))*i) for i = 0..((nr-of-seeds)-1)
        perhaps something more advanced like, floor((4294967295/(nr-of-seeds))*x)+floor(75000*(cos(x)*sin(x)))
        and floor((4294967295/(nr-of-seeds))*x)+floor(75000*(cos(-x)*sin(-x)))
        this introduces a wave into the linear curve, kind of like noise on data...
        the 75000 can be adjusted to more values to noise on a smaller/larger scale
"""
import sys
import random
import os
from datetime import datetime
import string

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
            sys.exit("Usage: python3 seedGenerator.py --seeds=INT --files=INT --name=CHARSEQUENCE\n" +\
                    "Where --seeds=INT is optional (default=1000), representing the number of seeds per file.\n" +\
                    "Where --files=INT is optional (default=1), representing the number of files to be generated.\n" +\
                    "Where --name=CHARSEQUENCE is optional, representing the filename used to write in \"config/seedTester\".\n" +\
                    "The order in which the options are given does not matter.\n")

        seed_amount = 1000
        present, value = argsContains(sys.argv, "--seeds")
        if( len(sys.argv) >= 2 and present and is_int(value, seed_amount) ):
            seed_amount = abs(int(value)) #in case someone tries to mess with negative numbers...

        nrfiles = 1
        present, value = argsContains(sys.argv, "--files")
        if( len(sys.argv) >= 2 and present and is_int(value, nrfiles) ):
            nrfiles = abs(int(value))

        if not os.path.exists("config"):  # testing if write dirs exist, else create
            os.makedirs("config")

        if not os.path.exists("config/seedTester"):
            os.makedirs("config/seedTester")

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

        for i in range(nrfiles):
            time = str(datetime.now())
            time = time.split(".")[0].replace("-", "").replace(" ", "_").replace(":", "")
            filename = "seeds-"+time+".txt"
            if( present ):
                filename = prefix + "-" + str(i) + ".txt"

            filename = "config/seedTester/{0}".format(filename)

            seeds = []
            #make sure seeds are unique within their respective file
            while( len(seeds) < seed_amount):
                #maybe more types of seeds besides random?
                #as in, with an option? but then we only need 1 file... so that forces some stuff...
                rand_int = random.randint(0, 4294967295) #because seed is an unsigned long
                if( rand_int not in seeds ):
                    seeds.append(rand_int)

            with open(filename, "w+") as file:
                for i in range(seed_amount):
                    file.write("{0}\n".format(seeds[i]))

    except IndexError:
        sys.exit("\033[91mError:\033[0m No seed amount given, try using --help...\n")

    except IOError:
        sys.exit("\033[91mError:\033[0m Given filepath does not exist, try using --help...\n")
