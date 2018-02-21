"""
Script to generate a config file for seedTester.py, will generate an amount of given seeds.

Script takes 2 arguments:

    - an integer: amount of seeds to generate.
    - optional filename: file to write seeds to.
"""
import sys
import random
import os

if __name__ == "__main__":

    try:
        seed_amount = int(sys.argv[1])

        if not os.path.exists("config"):  # testing if write dirs exist, else create
            os.makedirs("config")

        if not os.path.exists("config/seedTester"):
            os.makedirs("config/seedTester")

        filename = "generated_seeds.txt"
        if len(sys.argv) >= 3:
            filename = sys.argv[2]

        filename = "config/seedTester/{0}".format(filename)

        with open(filename, "w+") as file:
            for i in range(seed_amount):
                rand_int = random.randint(0, 100000000000)
                file.write("{0}\n".format(rand_int))

    except IndexError:
        sys.exit("No seed amount given, exiting...")

    except ValueError:
        sys.exit("Please insert a integer value, exiting...")

    except IOError:
        sys.exit("Given filepath does not exist, exiting...")
