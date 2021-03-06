#!/usr/bin/env bash
N=(10 100 500 1000 2000 3000)
NUMBS=$(seq 0 4)
REFS=(./ref_output_data/ellipse_N_00010_after200steps.gal ./ref_output_data/ellipse_N_00100_after200steps.gal ./ref_output_data/ellipse_N_00500_after200steps.gal ./ref_output_data/ellipse_N_01000_after200steps.gal ./ref_output_data/ellipse_N_02000_after200steps.gal ./ref_output_data/ellipse_N_03000_after100steps.gal)
INP=(./input_data/ellipse_N_00010.gal ./input_data/ellipse_N_00100.gal ./input_data/ellipse_N_00500.gal ./input_data/ellipse_N_01000.gal ./input_data/ellipse_N_02000.gal ./input_data/ellipse_N_03000.gal)



make clean
make 
gcc -std=c11  compare_gal_files/compare_gal_files.c  -o comp -lm


for var in ${NUMBS[*]}; do
    echo "RUNNING galsim: ${N[$var]} ${INP[$var]} 200 1e-5 0"
    ./galsim ${N[$var]} ${INP[$var]} 200 1e-5 0.15 0 2
    echo ""
    echo "RUNNING comp: ${N[$var]} ./result.gal ${REFS[$var]}"
    ./comp ${N[$var]} ./result.gal ${REFS[$var]} > tmp.txt
    echo ""
    grep pos_maxdiff tmp.txt | grep 00000 && exit 1
    grep pos_maxdiff tmp.txt | grep 000 || exit 1

done

echo "RUNNING galsim"
./galsim ${N[5]} ${INP[5]} 100 1e-5 0.25 0 9
echo ""
echo "RUNNING comp"
./comp ${N[5]} result.gal ${REFS[5]} 
#rm -f tmp.txt
