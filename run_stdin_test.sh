#!/usr/bin/env bash
set -e

testcase=$1
DIRECTORY=$2

for i in $DIRECTORY/*.in; do
    echo "*** Test file $i ***"
    cat $i | "./cmake-build-debug/$testcase"
    echo "*** OK ***"
    echo ""
done



