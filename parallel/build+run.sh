#!/bin/bash

cd src
make
cd ..
sbatch job.slurm
