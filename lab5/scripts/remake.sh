#!/bin/bash

make clean > /dev/null 2>&1
make > /dev/null 2>&1

for file in ../Test/*.ir; do
    echo "=== Testing: $file ===" >&2
    ./parser "$file" 1>/dev/null
    echo "" >&2
done