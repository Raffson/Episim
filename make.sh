#!/usr/bin/env bash
cd build
cmake ..
make -j8 -Wextra -Wall -Weffc++ -c++11
make all
make install
cd installed/
./bin/stride -e geopop -m