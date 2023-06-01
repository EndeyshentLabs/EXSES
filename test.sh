#!/bin/env bash

set -eo pipefail

if [[ ! -d release ]]
then
    echo Initializing CMake Build Tree...
    cmake -G Ninja -B release -DCMAKE_BUILD_TYPE=Release
fi

echo Building EXSI release...
cmake --build release -j --config Release

echo -e "\nTesting tests/ @ `date`"
for f in `find tests -type f -name \*.xes | sort -n`
do
    echo $f
    output=`cat $f | grep '^# output: ' | awk '{ print $3 }'`
    if [ ! "$(./release/exsi $f)" = "$output" ]
    then
        printf "\e[31mFAILED!\e[0m\n"
        printf "Expected:\n"
        printf "$output\n"
        printf "Got:\n"
        printf "$(./release/exsi $f)\n"
    else
        printf "\e[32mOk!\e[0m\n"
    fi
    echo
done
