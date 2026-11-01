#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build
flags="-std=c++17 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -DULTIMATE_SIMULATION_TEST"
for effect in snow rain leaves halloween; do
 case "$effect" in
 snow) files="src/effects/snow/snow.cpp src/effects/snow/snow_piles.cpp" ;;
 rain) files="src/effects/rain/rain.cpp src/effects/rain/rain_water.cpp" ;;
 leaves) files="src/effects/leaves/leaves.cpp src/effects/leaves/leaf_piles.cpp" ;;
 halloween) files="src/effects/halloween/scene.cpp src/effects/halloween/piles.cpp src/effects/halloween/spiders.cpp" ;;
 esac
 ${CXX:-g++} $flags tests/${effect}_tests.cpp $files src/effects/${effect}/presets.cpp -o build/${effect}_tests
 ASAN_OPTIONS=detect_leaks=0 ./build/${effect}_tests > build/${effect}-results.txt
 cat build/${effect}-results.txt
done

sh run-suite-tests.sh
