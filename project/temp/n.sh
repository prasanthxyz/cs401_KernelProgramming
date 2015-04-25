#!/bin/bash
if [ $# -ne 1 ]; then
    exit 0;
fi
echo "$1" > /sys/ll/nproc
