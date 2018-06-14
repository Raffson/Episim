#!/usr/bin/env bash
cd build
cmake ..
make -j8 -Wextra -Wall -Weffc++
make all
make install
cd installed/
./bin/stride -e geopop -m