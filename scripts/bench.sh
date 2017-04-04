#!/bin/bash

ionice -c 1 -n 0 nice -n -20 /usr/bin/time -v ./build/$1 ./testfile ./testfile2 2>&1 | tee out/$1
