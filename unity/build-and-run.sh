#!/bin/sh

# This script builds the runtime and class libraries and runs all tests, in one command.

set -e

# Build the runtime and class libraries
cd unity/unitygc
echo "Here 1"
mkdir -p release
echo "Here 2"
cd release
echo "Here 3"
cmake -DCMAKE_BUILD_TYPE=Release ..
echo "Here 4"
cmake --build .
echo "Here 5"
cd ../../..
echo "Here 6"
pwd
./build.sh -subset clr+libs+libs -a x64 -c release -ci -ninja
echo "Here 7"

# Build and run the tests
./build.sh -subset libs.tests -test -a x64 -c release -ci -ninja
./src/tests/build.sh x64 release ci
./src/tests/run.sh x64 release
./build.sh clr.paltests
./artifacts/bin/coreclr/$(uname).x64.Debug/paltests/runpaltests.sh $(pwd)/artifacts/bin/coreclr/$(uname).x64.Debug/paltests
