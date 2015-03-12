#!/bin/bash

sudo insmod char_device.ko
sudo mknod /dev/my_char_device c $(cat /proc/devices | grep my_char_device | cut -d ' ' -f 1) 0
sudo chmod 777 /dev/my_char_device
