#!/usr/bin/env bash
set -e

SOURCEDIR=$(dirname "$0")/..
BUILDDIR=${SOURCEDIR}/build

cd ${BUILDDIR}
cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ..

ninja
