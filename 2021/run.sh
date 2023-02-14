#!/bin/bash
set -e

ml_file="day$1.ml"
test_file="day$1_test.txt"
input_file="day$1_input.txt"

# Build
ocamlc util.mli
ocamlc util.ml $ml_file

# Test
if [ -f $test_file ]; then
        echo "# test"
        ./a.out $test_file
fi

# Input
if [ -f $input_file ]; then
        echo "# input"
        ./a.out $input_file
fi

# Clean up
rm day$1.cmo day$1.cmi a.out util.cmo
