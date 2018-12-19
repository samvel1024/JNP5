#!/usr/bin/env bash
set -e

mkdir -p graphs
for i in $(seq 1 20); do
    file="./graphs/g$i.dot"
   ./cmake-build-debug/generate > ${file}
   cat $file | grep "\->" | awk '{split($0,a," "); print a[1],a[3]}' > "./graphs/g$i.in"
done;


