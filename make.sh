#!/usr/bin/env bash
cd build
cmake ..
make -j16
make all
make install
cd installed/
./bin/popgen-Episim