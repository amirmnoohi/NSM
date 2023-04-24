#!/bin/bash

rmmod nsm

make clean

make

insmod nsm.ko