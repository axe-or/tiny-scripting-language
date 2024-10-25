#!/usr/bin/env sh

set -e

tcc -std=c11 -c kite.c -o build/kite.o
tcc -std=c11 -c main.c -o build/main.o
tcc build/kite.o build/main.o -o build/kite
