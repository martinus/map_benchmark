#!/usr/bin/env bash
SOURCEDIR=$(dirname "$0")/..
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="g++-8" -G Ninja ${SOURCEDIR}