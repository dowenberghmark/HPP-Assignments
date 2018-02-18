#!/usr/bin/env bash
N=5000
NUMBS=(1 2 4 8 16 32)

REFS=(./ref_output_data/ellipse_N_05000_after200steps.gal ./ref_output_data/ellipse_N_00100_after200steps.gal)
INP=(./input_data/ellipse_N_05000.gal ./input_data/ellipse_N_00100.gal ./input_data/ellipse_N_01000.gal)

make clean
make 

for var in ${NUMBS[*]}; do
    echo "RUNNING galsim: 5000 ${INP[0]} 200 1e-5 0.25 0 ${var}"
    /usr/bin/time -p ./galsim 5000 ${INP[0]} 200 1e-5 0.25 0 ${var}
    echo "RUNNING galsim: 1000 ${INP[2]} 200 1e-5 0.25 0 ${var}"
    /usr/bin/time -p ./galsim 1000 ${INP[2]} 200 1e-5 0.25 0 ${var}
    echo "RUNNING galsim: 100 ${INP[1]} 200 1e-5 0.25 0 ${var}"
    /usr/bin/time -p ./galsim 100 ${INP[1]} 200 1e-5 0.25 0 ${var}
    echo ""
done

