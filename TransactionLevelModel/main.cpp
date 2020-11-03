
#define REPORT_DEFINE_GLOBALS

#include <tlm.h>
#include <systemc.h>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>

#include <vector>
#include <string>
#include <fstream>
#include <random>

#include <hardware/tile.hpp>
#include <hardware/memory.hpp>
#include <hardware/axi.hpp>
#include <software/channel.hpp>
#include <delayvector.hpp>

#include <setup/experiment.hpp>

// Based on low level measurements.
sc_core::sc_time readdelay(12, sc_core::SC_NS); // \_ per token
sc_core::sc_time writedelay(9, sc_core::SC_NS); // /
// These are the delays for read (lwi) and write (swi) instructions
// including execution and memory access phases.
//
// The cycle accurate model expects exact these values and extrapolates
// for multiple contender C by assuming 
//      readdelay  · (C + 1)    and
//      writedelay · (C + 1)
// The +1 is the initiator of the communication.
// This is a very pessimistic approach and cannot be confirmed by experiments
// For example write access with 6 writing contender was measured
// with 26 cycles, instead of calculated 9·(6+1)=63 cycles
//
// The SystemC event based model takes less care about contention.
// Processing elements in polling mode are not considered as a contender.
// This leads to over optimistic results for many processing elements.

#include <monitor.hpp>

void PrintUsage()
{
    cerr << "--iterations   -i    - Define number of iterations to simulate (default: 1000000)\n";
    cerr << "--experiment   -e    - Select the experiment that shall be simulated (mandatory parameter)\n";
    cerr << "                           sobel1, sobel4, jpeg1, jpeg3, jpeg7\n";
    cerr << "--distribution -d    - Select the distribution used for the simulated (default: injected)\n";
    cerr << "                           injected, gaussian, uniform, wcet\e[0m\n";
    cerr << "--communicationmodel - Select the model for the communication via channels (default: cycleaccurate)\n";
    cerr << "               -c          cycleaccurate, systemcevents\n";
}


void CancelSimulation(sig_atomic_t s)
{
    exit(1); 
}


int sc_main(int argc, char *argv[])
{
    std::srand(0); // 0 is the seed - this is not very random but OK in this case

    // Read command line parameters
    unsigned int maxiterations = 1000000;
    std::string  experimentname= "null";
    DISTRIBUTION distribution  = DISTRIBUTION::INJECTED;
    COMMUNICATIONMODEL communicationmodel = COMMUNICATIONMODEL::CYCLEACCURATE;
    bool datadependentdelay    = false;

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
            maxiterations = stol(std::string(argv[i]));
            cerr << "\e[1;33mLimiting iterations to " << maxiterations << "\e[0m\n";
        }
        if((strncmp("--experiment", argv[i], 20) == 0) || (strncmp("-e", argv[i], 20) == 0))
        {
            i++;
            if(i >= argc)
            {
                cerr << "Invalid use of --experiment. Argument expected!\n";
                PrintUsage();
                exit(EXIT_FAILURE);
            }
            experimentname = std::string(argv[i]);
            cerr << "\e[1;37mRunning experiment " << experimentname << "\e[0m\n";
        }
        if((strncmp("--communicationmodel", argv[i], 20) == 0) || (strncmp("-c", argv[i], 20) == 0))
        {
            i++;
            if(i >= argc)
            {
                cerr << "Invalid use of --communicationmodel. Argument expected!\n";
                PrintUsage();
                exit(EXIT_FAILURE);
            }

            auto modelname = std::string(argv[i]);
            
            if(modelname == "cycleaccurate")
                communicationmodel = COMMUNICATIONMODEL::CYCLEACCURATE;
            else if(modelname == "systemcevents")
                communicationmodel = COMMUNICATIONMODEL::SYSTEMCEVENTS;
            else
            {
                cerr << "\e[1;31mERROR\e[0m Communication Model " << modelname << " not known.\e[0m\n";
                PrintUsage();
                exit(EXIT_FAILURE);
            }

            cerr << "\e[1;34mUsing communication model " << modelname << "\e[0m\n";
        }
        if((strncmp("--distribution", argv[i], 20) == 0) || (strncmp("-d", argv[i], 20) == 0))
        {
            i++;
            if(i >= argc)
            {
                cerr << "Invalid use of --distribution. Argument expected!\n";
                PrintUsage();
                exit(EXIT_FAILURE);
            }

            auto distributionname = std::string(argv[i]);
            
            if(distributionname == "injected")
                distribution  = DISTRIBUTION::INJECTED;
            else if(distributionname == "gaussian")
                distribution  = DISTRIBUTION::GAUSSIAN;
            else if(distributionname == "uniform")
                distribution  = DISTRIBUTION::UNIFORM;
            else if(distributionname == "wcet")
                distribution  = DISTRIBUTION::WCET;
            else if(distributionname == "explicit")
                datadependentdelay = true;
            else
            {
                cerr << "\e[1;31mERROR\e[0m Distribution " << distributionname << " not known.\e[0m\n";
                PrintUsage();
                exit(EXIT_FAILURE);
            }

            cerr << "\e[1;34mUsing distribution " << distributionname << "\e[0m\n";
        }
    }

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = CancelSimulation;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    cerr << "\e[1;34mPreparing experiment...\e[0m\n";

    // Create Actors
    Monitor monitor;

    // Create Channels
    // name, producerate, consumerate, size
    Channel ch_gx      ("ch_gx",    81, 81, 81, monitor, communicationmodel);
    Channel ch_gy      ("ch_gy",    81, 81, 81, monitor, communicationmodel);
    Channel ch_xa      ("ch_xa",     1 , 1,  1, monitor, communicationmodel);
    Channel ch_ya      ("ch_ya",     1 , 1,  1, monitor, communicationmodel);
    Channel ch_pos     ("ch_pos",    2,  2,  2, monitor, communicationmodel);
    Channel ch_gx2     ("ch_gx2",    9,  9,  9, monitor, communicationmodel);
    Channel ch_gy2     ("ch_gy2",    9,  9,  9, monitor, communicationmodel);
    Channel ch_xa2     ("ch_xa2",    1 , 1,  1, monitor, communicationmodel);
    Channel ch_ya2     ("ch_ya2",    1 , 1,  1, monitor, communicationmodel);
    Channel ch_dcoffset("dcoffset",  3,  3,  3, monitor, communicationmodel);
    Channel ch_ency    ("ency",     64, 64, 64, monitor, communicationmodel);
    Channel ch_enccr   ("enccr",    64, 64, 64, monitor, communicationmodel);
    Channel ch_enccb   ("enccb",    64, 64, 64, monitor, communicationmodel);
    Channel ch_prepy   ("prepy",    64, 64, 64, monitor, communicationmodel);
    Channel ch_prepcr  ("prepcr",   64, 64, 64, monitor, communicationmodel);
    Channel ch_prepcb  ("prepcb",   64, 64, 64, monitor, communicationmodel);
    Channel ch_y       ("y",        64, 64, 64, monitor, communicationmodel);
    Channel ch_cr      ("cr",       64, 64, 64, monitor, communicationmodel);
    Channel ch_cb      ("cb",       64, 64, 64, monitor, communicationmodel);

    // Create Delay Vectors
    // Sobel Timings
    std::string sobeldirectory = "../MeasuredDelays/sobel/bram/";
    DelayVectorMap getpixel_delay;
    getpixel_delay["none"] = new DelayVector(sobeldirectory + "GetPixel.txt", distribution);
    getpixel_delay["ea"]   = new DelayVector(sobeldirectory + "GetPixel-ea.txt", distribution);
    DelayVectorMap gx_delay;
    gx_delay["none"] = new DelayVector(sobeldirectory + "GX.txt", distribution);
    gx_delay["ea"]   = new DelayVector(sobeldirectory + "GX-ea.txt", distribution);
    DelayVectorMap gy_delay;
    gy_delay["none"] = new DelayVector(sobeldirectory + "GY.txt", distribution);
    gy_delay["ea"]   = new DelayVector(sobeldirectory + "GY-ea.txt", distribution);
    DelayVectorMap abs_delay;
    abs_delay["none"] = new DelayVector(sobeldirectory + "ABS.txt", distribution);
    abs_delay["ea"]   = new DelayVector(sobeldirectory + "ABS-ea.txt", distribution);

    // Sobel2 Timings
    std::string sobel2directory = "../MeasuredDelays/sobel2/bram/";
    DelayVectorMap getpixel2_delay;
    getpixel2_delay["ea"]   = new DelayVector(sobel2directory + "GetPixel-ea.txt", distribution);
    DelayVectorMap gx2_delay;
    gx2_delay["ea"]   = new DelayVector(sobel2directory + "GX-ea.txt", distribution);
    DelayVectorMap gy2_delay;
    gy2_delay["ea"]   = new DelayVector(sobel2directory + "GY-ea.txt", distribution);
    DelayVectorMap abs2_delay;
    abs2_delay["ea"]   = new DelayVector(sobel2directory + "ABS-ea.txt", distribution);

    // JPEG Timings
    std::string jpegdirectory = "../MeasuredDelays/jpeg/bram/";
    DelayVectorMap getencodedimageblock_delay;
    DelayVectorMap iq_y_delay;
    DelayVectorMap iq_cr_delay;
    DelayVectorMap iq_cb_delay;
    DelayVectorMap idct_y_delay;
    DelayVectorMap idct_cr_delay;
    DelayVectorMap idct_cb_delay;
    DelayVectorMap creatergbpixels_delay;

    getencodedimageblock_delay["none"] = new DelayVector(jpegdirectory + "GetEncodedImageBlock.txt",    distribution);
    getencodedimageblock_delay["ea"]   = new DelayVector(jpegdirectory + "GetEncodedImageBlock-ea.txt", distribution);
    getencodedimageblock_delay["ef"]   = new DelayVector(jpegdirectory + "GetEncodedImageBlock-ef.txt", distribution);

    iq_y_delay["none"]  = new DelayVector(jpegdirectory + "InverseQuantization_Y.txt",     distribution);
    iq_y_delay["ea"]    = new DelayVector(jpegdirectory + "InverseQuantization_Y-ea.txt",  distribution);
    iq_y_delay["ef"]    = new DelayVector(jpegdirectory + "InverseQuantization_Y-ef.txt",  distribution);
    iq_cr_delay["none"] = new DelayVector(jpegdirectory + "InverseQuantization_Cr.txt",    distribution);
    iq_cr_delay["ea"]   = new DelayVector(jpegdirectory + "InverseQuantization_Cr-ea.txt", distribution);
    iq_cr_delay["ef"]   = new DelayVector(jpegdirectory + "InverseQuantization_Cr-ef.txt", distribution);
    iq_cb_delay["none"] = new DelayVector(jpegdirectory + "InverseQuantization_Cb.txt",    distribution);
    iq_cb_delay["ea"]   = new DelayVector(jpegdirectory + "InverseQuantization_Cb-ea.txt", distribution);
    iq_cb_delay["ef"]   = new DelayVector(jpegdirectory + "InverseQuantization_Cb-ef.txt", distribution);

    idct_y_delay["none"]  = new DelayVector(jpegdirectory + "IDCT_Y.txt",     distribution);
    idct_y_delay["ea"]    = new DelayVector(jpegdirectory + "IDCT_Y-ea.txt",  distribution);
    idct_y_delay["ef"]    = new DelayVector(jpegdirectory + "IDCT_Y-ef.txt",  distribution);
    idct_cr_delay["none"] = new DelayVector(jpegdirectory + "IDCT_Cr.txt",    distribution);
    idct_cr_delay["ea"]   = new DelayVector(jpegdirectory + "IDCT_Cr-ea.txt", distribution);
    idct_cr_delay["ef"]   = new DelayVector(jpegdirectory + "IDCT_Cr-ef.txt", distribution);
    idct_cb_delay["none"] = new DelayVector(jpegdirectory + "IDCT_Cb.txt",    distribution);
    idct_cb_delay["ea"]   = new DelayVector(jpegdirectory + "IDCT_Cb-ea.txt", distribution);
    idct_cb_delay["ef"]   = new DelayVector(jpegdirectory + "IDCT_Cb-ef.txt", distribution);

    creatergbpixels_delay["none"] = new DelayVector(jpegdirectory + "CreateRGBPixels.txt",    distribution);
    creatergbpixels_delay["ea"]   = new DelayVector(jpegdirectory + "CreateRGBPixels-ea.txt", distribution);
    creatergbpixels_delay["ef"]   = new DelayVector(jpegdirectory + "CreateRGBPixels-ef.txt", distribution);




    // Create Architecture Components
    Tile mb0("MB0", maxiterations, monitor);
    Tile mb1("MB1", maxiterations, monitor);
    Tile mb2("MB2", maxiterations, monitor);
    Tile mb3("MB3", maxiterations, monitor);
    Tile mb4("MB4", maxiterations, monitor);
    Tile mb5("MB5", maxiterations, monitor);
    Tile mb6("MB6", maxiterations, monitor);
    SharedMemory sharedmemory("SharedMemory", 0x00010000, 32*1024,
            readdelay, writedelay, monitor);
    AXI bus("AXIBus");


    // Create & Load Actors
    std::string experimentpath;
    experimentpath  = "./experiments/";
    experimentpath += experimentname;
    experimentpath += ".xml";
    Experiment experiment(experimentpath);

    Actor getpixel("GetPixel", getpixel_delay, monitor);
    Actor gx      ("GX"      , gx_delay      , monitor);
    Actor gy      ("GY"      , gy_delay      , monitor);
    Actor abs     ("ABS"     , abs_delay     , monitor);

    Actor getpixel2("GetPixel2", getpixel2_delay, monitor);
    Actor gx2      ("GX2"      , gx2_delay      , monitor);
    Actor gy2      ("GY2"      , gy2_delay      , monitor);
    Actor abs2     ("ABS2"     , abs2_delay     , monitor);

    Actor getencodedimageblock("GetEncodedImageBlock", getencodedimageblock_delay, monitor);
    Actor iq_y                ("IQ_Y"                , iq_y_delay                , monitor);
    Actor iq_cr               ("IQ_Cr"               , iq_cr_delay               , monitor);
    Actor iq_cb               ("IQ_Cb"               , iq_cb_delay               , monitor);
    Actor idct_y              ("IDCT_Y"              , idct_y_delay              , monitor);
    Actor idct_cr             ("IDCT_Cr"             , idct_cr_delay             , monitor);
    Actor idct_cb             ("IDCT_Cb"             , idct_cb_delay             , monitor);
    Actor creatergbpixels     ("CreatetRGBPixels"    , creatergbpixels_delay     , monitor);

    // Define start and finish actors
    getpixel.DefineAsStartActor();
    abs.DefineAsFinishActor();

    getpixel2.DefineAsStartActor();
    abs2.DefineAsFinishActor();

    getencodedimageblock.DefineAsStartActor();
    creatergbpixels.DefineAsFinishActor();

    // TODO: (work in progress) Load Dynamic Experiment
    TileMap tilemap;
    tilemap["MB0"] = &mb0;
    tilemap["MB1"] = &mb1;
    tilemap["MB2"] = &mb2;
    tilemap["MB3"] = &mb3;
    tilemap["MB4"] = &mb4;
    tilemap["MB5"] = &mb5;
    tilemap["MB6"] = &mb6;

    ActorMap actormap;
    actormap["GetPixel"            ] = &getpixel            ;
    actormap["GX"                  ] = &gx                  ;
    actormap["GY"                  ] = &gy                  ;
    actormap["ABS"                 ] = &abs                 ;
    actormap["GetPixel2"           ] = &getpixel2           ;
    actormap["GX2"                 ] = &gx2                 ;
    actormap["GY2"                 ] = &gy2                 ;
    actormap["ABS2"                ] = &abs2                ;
    actormap["GetEncodedImageBlock"] = &getencodedimageblock;
    actormap["IQ_Y"                ] = &iq_y                ;
    actormap["IQ_Cr"               ] = &iq_cr               ;
    actormap["IQ_Cb"               ] = &iq_cb               ;
    actormap["IDCT_Y"              ] = &idct_y              ;
    actormap["IDCT_Cr"             ] = &idct_cr             ;
    actormap["IDCT_Cb"             ] = &idct_cb             ;
    actormap["CreateRGBPixels"     ] = &creatergbpixels     ;
    
    ChannelMap channelmap;
    channelmap["ch_gx"      ] = &ch_gx      ;
    channelmap["ch_gy"      ] = &ch_gy      ;
    channelmap["ch_xa"      ] = &ch_xa      ;
    channelmap["ch_ya"      ] = &ch_ya      ;
    channelmap["ch_pos"     ] = &ch_pos     ;
    channelmap["ch_gx2"     ] = &ch_gx2     ;
    channelmap["ch_gy2"     ] = &ch_gy2     ;
    channelmap["ch_xa2"     ] = &ch_xa2     ;
    channelmap["ch_ya2"     ] = &ch_ya2     ;
    channelmap["ch_dcoffset"] = &ch_dcoffset;
    channelmap["ch_ency"    ] = &ch_ency    ;
    channelmap["ch_enccr"   ] = &ch_enccr   ;
    channelmap["ch_enccb"   ] = &ch_enccb   ;
    channelmap["ch_prepy"   ] = &ch_prepy   ;
    channelmap["ch_prepcr"  ] = &ch_prepcr  ;
    channelmap["ch_prepcb"  ] = &ch_prepcb  ;
    channelmap["ch_y"       ] = &ch_y       ;
    channelmap["ch_cr"      ] = &ch_cr      ;
    channelmap["ch_cb"      ] = &ch_cb      ;
    
    MemoryMap memorymap;
    memorymap["SharedMemory"] = &sharedmemory;


    // Load mappings
    bool success;

    success = experiment.LoadActorMapping(tilemap, actormap);
    if(not success)
    {
        std::cerr << "\e[1;31mERROR:\e[0m Loading actor mapping for experiment "
                  << experimentname
                  << " failed!\n";
        exit(EXIT_FAILURE);
    }

    success = experiment.LoadChannelMapping(memorymap, tilemap, channelmap);
    if(not success)
    {
        std::cerr << "\e[1;31mERROR:\e[0m Loading channel mapping for  experiment "
                  << experimentname
                  << " failed!\n";
        exit(EXIT_FAILURE);
    }


    // Connection of actors
    getpixel >> ch_gx;
    getpixel >> ch_gy;
    gx       << ch_gx;
    gx       >> ch_xa;
    gy       << ch_gy;
    gy       >> ch_ya;
    abs      << ch_xa;
    abs      << ch_ya;

    getpixel2 << ch_pos;
    getpixel2 >> ch_gx2;
    getpixel2 >> ch_gy2;
    getpixel2 >> ch_pos;
    gx2       << ch_gx2;
    gx2       >> ch_xa2;
    gy2       << ch_gy2;
    gy2       >> ch_ya2;
    abs2      << ch_xa2;
    abs2      << ch_ya2;

    getencodedimageblock << ch_dcoffset;
    getencodedimageblock >> ch_ency;
    getencodedimageblock >> ch_enccr;
    getencodedimageblock >> ch_enccb;
    getencodedimageblock >> ch_dcoffset;

    iq_y    << ch_ency;
    iq_y    >> ch_prepy;
    iq_cr   << ch_enccr;
    iq_cr   >> ch_prepcr;
    iq_cb   << ch_enccb;
    iq_cb   >> ch_prepcb;

    idct_y  << ch_prepy;
    idct_y  >> ch_y;
    idct_cr << ch_prepcr;
    idct_cr >> ch_cr;
    idct_cb << ch_prepcb;
    idct_cb >> ch_cb;

    creatergbpixels << ch_y;
    creatergbpixels << ch_cr;
    creatergbpixels << ch_cb;

    // Build Architecture
    bus << mb0;
    bus << mb1;
    bus << mb2;
    bus << mb3;
    bus << mb4;
    bus << mb5;
    bus << mb6;
    bus << sharedmemory;


    // Start simulation
    std::cerr << "\e[1;37mSimulation started\n\e[0m";
    sc_core::sc_start();

    return 0;
}
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

