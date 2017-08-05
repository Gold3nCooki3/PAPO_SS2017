#!/bin/bash

#SBATCH --time=1
# Run ten tasks on two nodes.
#SBATCH -N 4
#SBATCH -n 4
# Use "west" partition.
#SBATCH --partition=west
# Output goes to "output.out", error messages to "error.err".
#SBATCH --output=job.out
#SBATCH --error=job.err

. /etc/profile.d/wr-spack.sh
spack load --dependencies mpi

if [ "${SLURM_PARTITION}" = 'abu' ]
then
	export MPICH_NEMESIS_NETMOD=ib
fi

#srun hostname
mpirun ./src/prog ./testdata2.mall 
