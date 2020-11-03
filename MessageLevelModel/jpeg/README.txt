How To Build
============

1. Setup
--------
Open build.sh and check if the path to the SystemC installation is valid.
I have installed SystemC in /usr/local/systemc-2.3.3 - If your path is different, just change the path in the HEADER and LIBS variables accordingly.

I also use the GCC C++ compiler, g++. When you want to use the LLVM C++ Compiler just replace g++ by clang++.

2. Compile
----------
Just execute the build.sh script.
It compiles all cpp-files in this directroy and sub directories.
Then the objectfiles get linked to a binary called model.



Save simulated iteration delays

Optional parameters:
 -i (--iterations): Limit the amount of iterations that get simulated. Default is 1 million.
 -s (--skip):       Skip n iterations.

./model -i 1000 -s 20000 # Simulate iteration 1000 .. 2000

./model > results.txt   # save results in a text file