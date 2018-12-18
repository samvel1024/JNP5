#!/usr/bin/env bash

set -e


for test in cmake-build-debug/test_*
do
    echo "Running executable $test";
    ./"$test"
    echo "OK"
    echo ""
done

#TODO
