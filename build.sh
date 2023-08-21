#！/bin/bash

set -e

rm -rf build/*
cd build
cmake ..
make