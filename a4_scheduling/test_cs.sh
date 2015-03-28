#!/bin/bash

# sudo rmmod assignment
# make
# sudo insmod assignment.ko
# ./process_creator 10
# sudo rmmod assignment

sudo rmmod assignment
sudo insmod assignment.ko
./process_creator 10 2 1
