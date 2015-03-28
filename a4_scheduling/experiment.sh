#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: bash experiment.sh <exp-id>"
    exit 1
fi

make

function simple {
    sudo insmod assignment.ko
}

function high_load {
    sudo insmod assignment.ko
    ./process_creator 20 0
}

function inc_load {
    sudo insmod assignment.ko
    ./process_creator 10 2
}
    
# context switch
if [ $1 -eq 1 ]; then
    simple

# context switch high load
elif [ $1 -eq 2 ]; then
    high_load

# context switch increasing load
elif [ $1 -eq 3 ]; then
    inc_load

# run queue length w/o affinity
elif [ $1 -eq 4 ]; then
    inc_load

# run queue length w/ affinity
elif [ $1 -eq 5 ]; then
    simple
    ./process_creator 10 2 1

# migrations
elif [ $1 -eq 6 ]; then
    simple

# migrations high load
elif [ $1 -eq 7 ]; then
    high_load

# migrations inc load
elif [ $1 -eq 8 ]; then
    inc_load

else
    echo "Invalid experiment id"
fi

