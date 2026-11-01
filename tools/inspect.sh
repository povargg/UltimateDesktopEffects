#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
for target in preview benchmark; do
 ${CXX:-g++} -std=c++17 -O2 -DNDEBUG -DULTIMATE_SIMULATION_TEST tests/${target}.cpp src/config.cpp src/effects.cpp src/effects/*/settings.cpp src/effects/*/presets.cpp \
  src/effects/snow/snow.cpp src/effects/snow/snow_piles.cpp src/effects/rain/rain.cpp src/effects/rain/rain_water.cpp src/effects/leaves/leaves.cpp src/effects/leaves/leaf_piles.cpp \
  src/effects/halloween/scene.cpp src/effects/halloween/piles.cpp src/effects/halloween/spiders.cpp -o build/${target}
 if [ "$target" = "preview" ]; then ./build/preview; else ./build/benchmark; fi
done
python3 tools/preview.py
