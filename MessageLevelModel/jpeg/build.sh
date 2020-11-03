#!/usr/bin/env bash

SOURCE=$(find . -type f -name "*.cpp")
SYSTEMC="/opt/systemc"
APPS="./apps"
HEADER="-I. -I$SYSTEMC/include -I$APPS"
LIBS="-L$SYSTEMC/lib-linux64 -lsystemc $(pkg-config --libs gsl)"
CPP="g++"
CPPFLAGS="-g3 -O0 -std=c++14"

for c in $SOURCE ;
do
    echo -e "\e[1;34mCompiling $c …\e[0m"
    $CPP $HEADER $CPPFLAGS -c -o "${c%.*}.o" $c
done

OBJECTS=$(find . -type f -name "*.o")

$CPP -o model $OBJECTS $LIBS
rm $OBJECTS

# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

