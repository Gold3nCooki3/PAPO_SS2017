#!/bin/bash

. load_mpi_dep.sh
cd src
make
cd ..
rm err.err out.out output.entout
sbatch job.slurm
