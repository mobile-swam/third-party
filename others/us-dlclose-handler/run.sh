#!/usr/bin/env bash

rm -f ./main
gcc -fPIC -c count.c
gcc -shared count.o -o libcount.so
gcc -o main main.c -ldl -lpthread
./main