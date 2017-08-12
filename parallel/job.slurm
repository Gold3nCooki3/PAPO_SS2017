#!/bin/bash

#SBATCH --time=5
#SBATCH -N 4
#SBATCH -n 4
#SBATCH --partition=west
#SBATCH --output=job.out
#SBATCH --error=job.err

. /etc/profile.d/wr-spack.sh
spack load --dependencies mpi

if [ "${SLURM_PARTITION}" = 'abu' ]
then
	export MPICH_NEMESIS_NETMOD=ib
fi

mpirun ./src/prog ./data/testdata4.mall.hex 
