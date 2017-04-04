#!/bin/sh

dd if=/dev/urandom of=testfile bs=32M count=1 iflag=fullblock
