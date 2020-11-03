    
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
#include <tile.hpp> 
int pCount, wCount, rCount;

// Default number of iterations (in case it gets not changed by command line argument)
unsigned int NUM_OF_ITERATIONS = 1000000;
unsigned int SKIP_ITERATIONS   = 0; // Samples to skip from the input files

sc_core::sc_time readdelay(18, sc_core::SC_NS);  // \_ per token
sc_core::sc_time writedelay(14, sc_core::SC_NS); // /

DISTRIBUTION distribution = DISTRIBUTION::INJECTED;

// name, producerate, consumerate, size
Channel ch_dcoffset("dcoffset",  3,  3,  3);
Channel ch_ency    ("ency",     64, 64, 64);
Channel ch_enccr   ("enccr",    64, 64, 64);
Channel ch_enccb   ("enccb",    64, 64, 64);
Channel ch_prepy   ("prepy",    64, 64, 64);
Channel ch_prepcr  ("prepcr",   64, 64, 64);
Channel ch_prepcb  ("prepcb",   64, 64, 64);
Channel ch_y       ("y",        64, 64, 64);
Channel ch_cr      ("cr",       64, 64, 64);
Channel ch_cb      ("cb",       64, 64, 64);


// Create Delay Vectors
// Sobel Timings

// JPEG Timings
std::string jpegdirectory = "../MeasuredDelays/jpeg/bram/";                                     //measured computation time directory

auto DelayGetEncodedImageBlock = DelayVector(jpegdirectory + "GetEncodedImageBlock.txt",        distribution);
auto DelayIQ_Y                 = DelayVector(jpegdirectory + "InverseQuantization_Y.txt",       distribution);
auto DelayIQ_Cr                = DelayVector(jpegdirectory + "InverseQuantization_Cr.txt",      distribution);
auto DelayIQ_Cb                = DelayVector(jpegdirectory + "InverseQuantization_Cb.txt",      distribution);
auto DelayIQ_Y_EA              = DelayVector(jpegdirectory + "InverseQuantization_Y-ea.txt",    distribution);
auto DelayIQ_Cr_EA             = DelayVector(jpegdirectory + "InverseQuantization_Cr-ea.txt",   distribution);
auto DelayIQ_Cb_EA             = DelayVector(jpegdirectory + "InverseQuantization_Cb-ea.txt",   distribution);
auto DelayIDCT_Y               = DelayVector(jpegdirectory + "IDCT_Y.txt",                      distribution);
auto DelayIDCT_Cr              = DelayVector(jpegdirectory + "IDCT_Cr.txt",                     distribution);
auto DelayIDCT_Cb              = DelayVector(jpegdirectory + "IDCT_Cb.txt",                     distribution);
auto DelayIDCT_Y_EF            = DelayVector(jpegdirectory + "IDCT_Y-ef.txt",                   distribution);
auto DelayIDCT_Cr_EF           = DelayVector(jpegdirectory + "IDCT_Cr-ef.txt",                  distribution);
auto DelayIDCT_Cb_EF           = DelayVector(jpegdirectory + "IDCT_Cb-ef.txt",                  distribution);
auto DelayCreateRGBPixels      = DelayVector(jpegdirectory + "CreateRGBPixels.txt",             distribution);

//JPEG Communication Timings
std::string jpegdirectorycom = "../MeasuredDelays/jpeg/bram/com/";                                 // Measured Read/Write Offset time directory

auto DelayReadDCOffsetChannel  = DelayVector(jpegdirectorycom + "ReadDCOffsetChannel.txt",         distribution);
auto DelayWriteDCOffsetChannel = DelayVector(jpegdirectorycom + "WriteDCOffsetChannel.txt",        distribution);

#include <experiments/jpeg1event.hpp>

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

    sharedmemory << ch_dcoffset;
    sharedmemory << ch_ency    ;
    sharedmemory << ch_enccr   ;
    sharedmemory << ch_enccb   ;
    sharedmemory << ch_prepy   ;
    sharedmemory << ch_prepcr  ;
    sharedmemory << ch_prepcb  ;
    sharedmemory << ch_y       ;
    sharedmemory << ch_cr      ;
    sharedmemory << ch_cb      ;
    
    // Initialize simulation
    std::srand(0); // 0 is the seed - this is not very random but OK in this case

    // Start simulation
    sc_core::sc_start();
    return 0;
}
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

