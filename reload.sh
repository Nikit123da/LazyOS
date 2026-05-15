#!/bin/bash

cd ~/LazyOS/
make clean
./build.sh
cd ~/LazyOS/bin/
qemu-system-i386 -hda ./os.bin
