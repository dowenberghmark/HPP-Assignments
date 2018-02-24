#!/usr/bin/env bash
N=5000
NUMBS=(1 2 4 8 16)

REFS=./ref_output_data/ellipse_N_05000_after200steps.gal
INP=./input_data/ellipse_N_05000.gal



make clean
make 
gcc -std=c11  compare_gal_files/compare_gal_files.c  -o comp -lm


for var in ${NUMBS[*]}; do
    echo "RUNNING galsim: 5000 ${INP} 200 1e-5 0.25 0 ${var}"
    /usr/bin/time -p ./galsim 5000 ${INP} 200 1e-5 0.25 0 ${var}
    echo ""
    echo "RUNNING comp: $5000 ./result.gal ${REFS}"
    ./comp 5000 ./result.gal ${REFS} > tmp.txt
    echo ""
    grep pos_maxdiff tmp.txt | grep 00000 && exit 1
    grep pos_maxdiff tmp.txt | grep 000 || exit 1

done

