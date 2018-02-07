#!/usr/bin/env bash
N=(1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
NUMBS=$(seq 0 9)

INP=(./input_data/ellipse_N_01000.gal ./input_data/ellipse_N_02000.gal ./input_data/ellipse_N_03000.gal ./input_data/ellipse_N_04000.gal ./input_data/ellipse_N_05000.gal ./input_data/ellipse_N_06000.gal ./input_data/ellipse_N_07000.gal ./input_data/ellipse_N_08000.gal ./input_data/ellipse_N_09000.gal ./input_data/ellipse_N_10000.gal)


make clean
make all

for var in ${NUMBS[*]}; do
    echo "RUNNING galsim: ${N[$var]} ${INP[$var]} 200 1e-5 0"
    ./galsim ${N[$var]} ${INP[$var]} 200 1e-5 0 >> benchmark.csv
    echo ""
    echo "RUNNING comp: ${N[$var]} ./result.gal ${REFS[$var]}"
    echo ""

done
