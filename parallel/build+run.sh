#!/bin/bash

. load_mpi_dep.sh
cd src
make
cd ..
sbatch job.slurm
