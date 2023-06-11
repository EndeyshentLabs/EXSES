#!/bin/env bash

set -eo pipefail

if [[ ! -d release ]]
then
    echo Initializing CMake Build Tree...
    cmake -G Ninja -B release -DCMAKE_BUILD_TYPE=Release
fi

echo Building EXSI release...
cmake --build release -j --config Release

has_failed=0

function test_folder () {
    local folder=$1
    echo -e "\nTesting $folder @ `date`"
    local failed=0
    local num=0
    local skipped=0
    for f in `find $folder -type f -name \*.xes | sort -n`
    do
        num=$((num+1))
        echo $f
        if [[ ! `cat $f | grep '^# skip'` == "" ]]
        then
            skipped=$((skipped+1))
            printf "SKIPPED\n"
            echo
            continue
        fi
        local output=`cat $f | grep '^# output: ' | sed 's/^# output: //g'`
        if [ ! "$(./release/exsi $f)" = "$output" ]
        then
            printf "\e[31mFAILED!\e[0m\n"
            printf "Expected:\n"
            printf "$output\n"
            printf "Got:\n"
            printf "$(./release/exsi $f)\n"
            failed=$((failed+1))
            has_failed=1
        else
            printf "\e[32mOk!\e[0m\n"
        fi
        echo
    done

    local passed=$((num-failed-skipped))
    printf  "$folder \e[32mTOTAL\e[0m:   $((num+skipped))\n"
    printf  "$folder \e[32mPASSED\e[0m:  $passed ($((passed*100/num))%%)\n"
    printf  "$folder SKIPPED: $skipped ($((skipped*100/num))%%)\n"
    printf  "$folder \e[31mFAILED\e[0m:  $failed ($((failed*100/num))%%)\n"
}

test_folder tests/
echo
test_folder examples/

if [[ $has_failed -gt 0 ]]
then
    exit 1
fi
