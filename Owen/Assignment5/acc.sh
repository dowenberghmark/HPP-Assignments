#!/usr/bin/env bash
NUMBS=$(seq 0 9)
THETA=(0.05 0.1 0.15 0.2 0.25 0.3 0.35 0.4 0.45 0.5)
REFS=./ref_output_data/ellipse_N_02000_after200steps.gal
INP=./input_data/ellipse_N_02000.gal

make clean
make
gcc -std=c11  compare_gal_files/compare_gal_files.c  -o comp -lm
rm -f acc.txt
for var in ${NUMBS[*]}; do
    echo "RUNNING galsim: 2000 ${INP} 200 1e-5 ${THETA[$var]} 0"
    ./galsim 2000 ${INP} 200 1e-5 ${THETA[$var]} 0 >> acc.txt 
    echo ""
    echo "RUNNING comp: ${N[$var]} ./result.gal ${REFS[$var]}"
    ./comp 2000 ./result.gal ${REFS} >> acc.txt
    echo ""
    
done
grep pos_maxdiff tmp.txt | grep 00000 && exit 1
grep pos_maxdiff tmp.txt | grep 000 || exit 1
