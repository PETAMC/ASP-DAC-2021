    
#define REPORT_DEFINE_GLOBALS

#include <tlm.h>
#include <systemc.h>
#include <cstdlib>

#include <iostream>

#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <sharedmemory.hpp>
#include <bus.hpp>
#include <channel.hpp>
#include <delayvector.hpp>
#include <chrono>
#include <ctime> 


int pCount, wCount, rCount;

//#include "monitor.h"
// Default number of iterations (in case it gets not changed by command line argument)
unsigned int NUM_OF_ITERATIONS = 1000000;
unsigned int SKIP_ITERATIONS   = 0; // Samples to skip from the input files


sc_core::sc_time readdelay(18, sc_core::SC_NS);  // \_ per token
sc_core::sc_time writedelay(14, sc_core::SC_NS); // /

DISTRIBUTION distribution = DISTRIBUTION::INJECTED;

// name, producerate, consumerate, size

Channel ch_gx("ch_gx", 9, 9, 9);
Channel ch_gy("ch_gy", 9, 9, 9);
Channel ch_xa("ch_xa", 1, 1, 1);
Channel ch_ya("ch_ya", 1, 1, 1);
Channel ch_pos("ch_pos", 2, 2, 2);

// Create Delay Vectors
// Sobel Timings
std::string sobeldirectory = "../MeasuredDelays/sobel2/bram/";                                  // measured computation time directory
auto DelayGetPixel_EA          = DelayVector(sobeldirectory + "GetPixel-ea.txt",                distribution);
auto DelayGX_EA                = DelayVector(sobeldirectory + "GX-ea.txt",                      distribution);
auto DelayGY_EA                = DelayVector(sobeldirectory + "GY-ea.txt",                      distribution);
auto DelayABS_EA               = DelayVector(sobeldirectory + "ABS-ea.txt",                     distribution);

#include <experiments/sobel1event.hpp>

void PrintUsage()
{
    cerr << "--iterations  -i    - Define number of iterations to simulate (default: 1000000)\n";
    cerr << "--skip        -s    - Skip number of samples from the input files (default: 0)\n";
}

int sc_main(int argc, char *argv[])
{     

    for(int i=0; i<argc; i++)
    {
        if((strncmp("--help", argv[i], 20) == 0) || (strncmp("-h", argv[i], 20) == 0))
        {
            PrintUsage();
            exit(EXIT_SUCCESS);
        }
        if((strncmp("--iterations", argv[i], 20) == 0) || (strncmp("-i", argv[i], 20) == 0))
        {
            i++;
            if(i >= argc)
            {
                cerr << "Invalid use of --iteration. Argument expected!\n";
                PrintUsage();
                exit(EXIT_SUCCESS);
            }
            NUM_OF_ITERATIONS = stol(std::string(argv[i]));
            cerr << "\e[1;33mLimiting iterations to " << NUM_OF_ITERATIONS << "\e[0m\n";
        }
        if((strncmp("--skip", argv[i], 20) == 0) || (strncmp("-s", argv[i], 20) == 0))
        {
            i++;
            if(i >= argc)
            {
                cerr << "Invalid use of --skip. Argument expected!\n";
                PrintUsage();
                exit(EXIT_SUCCESS);
            }
            SKIP_ITERATIONS = stol(std::string(argv[i]));
            cerr << "\e[1;33mSkipping " << SKIP_ITERATIONS << " samples\e[0m\n";
        }
    }
    NUM_OF_ITERATIONS += SKIP_ITERATIONS;

    cerr << "\e[1;35mInitializing simulation...\e[0m\n";     
    // Create Architecture Components
    MB0 mb0;
    MB1 mb1;
    MB2 mb2;
    MB3 mb3;
    MB4 mb4;
    MB5 mb5;
    MB6 mb6;

    SharedMemory sharedmemory("SharedMemory", 0x1000, 32*1024, 
            readdelay, writedelay);
    Bus bus("AXIBus");

    // Build Architecture
    bus << mb0;
    bus << mb1;
    bus << mb2;
    bus << mb3;
    bus << mb4;
    bus << mb5;
    bus << mb6;
    bus << sharedmemory;

    // Map Channels

    sharedmemory << ch_gx;
    sharedmemory << ch_gy;
    sharedmemory << ch_xa;
    sharedmemory << ch_ya;
    sharedmemory << ch_pos;
    
    // Initialize simulation
    std::srand(0); // 0 is the seed - this is not very random but OK in this case

    // Start simulation
    sc_core::sc_start();

    return 0;
}
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
