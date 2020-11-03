#ifndef EXPERIMENT_HPP
#define EXPERIMENT_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <tile.hpp>
#include <channel.hpp>
// Event definition
sc_core::sc_event write_gx, write_gy, write_xa, write_ya, write_pos ,read_gx, read_gy, read_xa, read_ya, read_pos;
// Buffer Availability
bool buff_gx = 1;
bool buff_gy = 1;
bool buff_xa = 1;
bool buff_ya = 1;
bool buff_pos = 1;
double t_latency = 0;

sc_core::sc_time start[1000000];
sc_core::sc_time stop[1000000];
sc_core::sc_time latency[1000000];

class MB0 : public Tile
{
    public:
        MB0() : Tile(sc_core::sc_module_name("MB0")) {};
    protected:
        void Execute();
};

void MB0::Execute()
{
    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    {   

        start[i] =  sc_core::sc_time_stamp();

        //Read position
        buff_pos = this->ReadTokens(read_pos, write_pos, buff_pos, 1, 2, t_r_loop);

        sc_core::wait(DelayGetPixel_EA.GetDelay());
        buff_gx = this->WriteTokens(read_gx, write_gx, buff_gx, 1, 9, t_w_loop);
        buff_gy = this->WriteTokens(read_gy, write_gy, buff_gy, 1, 9, t_w_loop);
        buff_pos = this->WriteTokens(read_pos, write_pos, buff_pos, 1, 2, t_w_loop);

        // GX
        buff_gx = this->ReadTokens(read_gx, write_gx, buff_gx, 1, 9, t_r_loop);
        sc_core::wait(DelayGX_EA.GetDelay());
        buff_xa = this->WriteTokens(read_xa, write_xa, buff_xa, 1, 1, t_w_loop);

        // GY
        buff_gy = this->ReadTokens(read_gy, write_gy, buff_gy, 1, 9, t_r_loop);
        sc_core::wait(DelayGY_EA.GetDelay());
        buff_ya = this->WriteTokens(read_ya, write_ya, buff_ya, 1, 1, t_w_loop);

        //ABS
        buff_xa = this->ReadTokens(read_xa, write_xa, buff_xa, 1, 1, t_r_loop);
        buff_ya = this->ReadTokens(read_ya, write_ya, buff_ya, 1, 1, t_r_loop);


        sc_core::wait(DelayABS_EA.GetDelay()); 

        stop[i] =  sc_core::sc_time_stamp();
        latency[i] = stop[i] - start[i]; // In cycles
        t_latency =  (double) latency[i].value()/1000;
        std::cout <<  std::dec  << t_latency << std::endl;   

    }
        
}

class MB1 : public Tile
{
    public:
        MB1() : Tile(sc_core::sc_module_name("MB1")) {};
    protected:
         void Execute();
};

void MB1::Execute()
{

    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    {   

    }   
}


class MB2 : public Tile
{
    public:
        MB2() : Tile(sc_core::sc_module_name("MB2")) {};
    protected:
         void Execute();
};

void MB2::Execute()
{
    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    {  
        
        
    }   
}

class MB3 : public Tile
{
    public:
        MB3() : Tile(sc_core::sc_module_name("MB3")) {};
    protected:
         void Execute();
};

void MB3::Execute()
{
    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    {

    }   
}

class MB4 : public Tile
{
    public:
        MB4() : Tile(sc_core::sc_module_name("MB4")) {};
    protected:
         void Execute();
};

void MB4::Execute()
{

    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    {   

    }    
        
}


class MB5 : public Tile
{
    public:
        MB5() : Tile(sc_core::sc_module_name("MB5")) {};
    protected:
         void Execute();
};

void MB5::Execute()
{
    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    { 
    }   
}


class MB6 : public Tile
{
    public:
        MB6() : Tile(sc_core::sc_module_name("MB6")) {};
    protected:
         void Execute();
};

void MB6::Execute()
{
    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    {   
    }   
}


#endif