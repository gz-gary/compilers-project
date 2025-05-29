#!/bin/bash

# 遍历 ./Test 下的所有 .ir 文件
for file in ./Test/*.ir; do
    echo "=== Testing: $file ===" >&2
    ./Code/parser "$file" 1>/dev/null
    echo "" >&2
done
