#!/bin/bash

cd ~/LazyOS/
make clean
./build.sh
cd ~/LazyOS/bin/
qemu-system-x86_64 -hda ./os.bin
