#!/bin/bash

cd Code
make clean
make parser

cd ../Test

for input_file in *.cmm; do
    [ -e "$input_file" ] || continue

    output_file="${input_file%.cmm}.ir"

    echo "Compiling $input_file to $output_file"

    ../Code/parser "$input_file" "$output_file"
done