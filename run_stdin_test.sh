#!/usr/bin/env bash
set -e

testcase=$1
DIRECTORY=graphs

for i in $DIRECTORY/*.in; do
    echo "*** Test file $1 ***"
    cat $i | "./cmake-build-debug/$testcase"
    echo "*** OK ***"
    echo ""
done



