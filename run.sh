#!/bin/bash

# mkdir -p build
#cd build
# rm -rf *
cmake .. -DCMAKE_INSTALL_PREFIX=~
cmake --build .
# sudo apt-get install libssl-dev
