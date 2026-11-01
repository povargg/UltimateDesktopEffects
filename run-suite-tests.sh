#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build
${CXX:-g++} -std=c++17 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -DULTIMATE_SIMULATION_TEST \
 tests/suite_tests.cpp src/config.cpp src/effects.cpp src/effects/*/settings.cpp src/effects/*/presets.cpp \
 src/effects/snow/snow.cpp src/effects/snow/snow_piles.cpp src/effects/rain/rain.cpp src/effects/rain/rain_water.cpp \
 src/effects/leaves/leaves.cpp src/effects/leaves/leaf_piles.cpp src/effects/halloween/scene.cpp src/effects/halloween/piles.cpp src/effects/halloween/spiders.cpp \
 -o build/suite_tests
ASAN_OPTIONS=detect_leaks=0 ./build/suite_tests
