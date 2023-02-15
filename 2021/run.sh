#!/bin/bash
set -e

ml_file="day$1.ml"
test_file="inputs/day$1_test.txt"
input_file="inputs/day$1_input.txt"

# Build
ocamlc util.mli

# Test
if [ -f $test_file ]; then
        ocamlc util.ml -g $ml_file
        echo "# test"
         OCAMLRUNPARAM=b ./a.out $test_file
fi

# Input
if [ -f $input_file ]; then
        ocamlc util.ml $ml_file
        echo "# input"
        ./a.out $input_file
fi

# Clean up
rm day$1.cmo day$1.cmi a.out util.cmo
