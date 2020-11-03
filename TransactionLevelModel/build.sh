#!/usr/bin/env bash

SYSTEMC="/opt/systemc"
SOURCE=$(find . -type f -name "*.cpp")
HEADER="-I. -I$SYSTEMC/include"
LIBS="-L$SYSTEMC/lib-linux64 -lsystemc $(pkg-config --libs gsl) $(pkg-config --libs tinyxml2) -ldl -lpthread"
CPP="clang++"
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

