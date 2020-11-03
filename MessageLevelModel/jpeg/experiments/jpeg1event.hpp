#ifndef EXPERIMENT_HPP
#define EXPERIMENT_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <tile.hpp>
#include <channel.hpp>

sc_core::sc_time start[1000000], stop[1000000], latency[1000000];
double t_latency = 0;
// Event definition
sc_core::sc_event write_EncY, write_EncCr, write_EncCb, write_PrepY, write_PrepCr, write_PrepCb, write_Y, write_Cr, write_Cb;
sc_core::sc_event read_EncY, read_EncCr, read_EncCb, read_PrepY, read_PrepCr, read_PrepCb, read_Y, read_Cr, read_Cb;
// Buffer Availability
bool buff_EncY = 1, buff_EncCr = 1, buff_EncCb = 1, buff_PrepY = 1, buff_PrepCr = 1, buff_PrepCb = 1, buff_Y = 1, buff_Cr = 1, buff_Cb = 1;

class MB0 : public Tile
{
    public:
        MB0() : Tile(sc_core::sc_module_name("MB0")) {};
    protected:
        void Execute();
};

void MB0::Execute()
{
    double t_latency = 0;

    for(int i=SKIP_ITERATIONS; i<NUM_OF_ITERATIONS; i++)
    {    
        start[i] =  sc_core::sc_time_stamp();
        
        // GetEncodedImageBlock
        sc_core::wait(DelayReadDCOffsetChannel.GetDelay());
        sc_core::wait(DelayGetEncodedImageBlock.GetDelay());
        buff_EncY = this->WriteTokens(read_EncY, write_EncY, buff_EncY, 1, 64, t_w_loop);

        buff_EncCr = this->WriteTokens(read_EncCr, write_EncCr, buff_EncCr, 1, 64, t_w_loop);
        buff_EncCb = this->WriteTokens(read_EncCb, write_EncCb, buff_EncCb, 1, 64, t_w_loop);

        sc_core::wait(DelayWriteDCOffsetChannel.GetDelay());

        // InverseQuantization_Y
        buff_EncY = this->ReadTokens(read_EncY, write_EncY, buff_EncY, 1, 64, t_r_loop);
        sc_core::wait(DelayIQ_Y.GetDelay()); // InverseQuantization_Y
        buff_PrepY = this->WriteTokens(read_PrepY, write_PrepY, buff_PrepY, 1, 64, t_w_loop);

        // InverseQuantization_Cr 
        buff_EncCr = this->ReadTokens(read_EncCr, write_EncCr, buff_EncCr, 1, 64, t_r_loop);
        sc_core::wait(DelayIQ_Cr.GetDelay()); // InverseQuantization_Cr
        buff_PrepCr = this->WriteTokens(read_PrepCr, write_PrepCr, buff_PrepCr, 1, 64, t_w_loop);

        // InverseQuantization_Cb 
        buff_EncCb = this->ReadTokens(read_EncCb, write_EncCb, buff_EncCb, 1, 64, t_r_loop);
        sc_core::wait(DelayIQ_Cb.GetDelay()); // InverseQuantization_Cb
        buff_PrepCb = this->WriteTokens(read_PrepCb, write_PrepCb, buff_PrepCb, 1, 64, t_w_loop);



        // IDCT_Y 
        buff_PrepY = this->ReadTokens(read_PrepY, write_PrepY, buff_PrepY, 1, 64, t_r_loop);
        sc_core::wait(DelayIDCT_Y.GetDelay()); // IDCT_Y
        buff_Y = this->WriteTokens(read_Y, write_Y, buff_Y, 1, 64, t_w_loop); 

        // IDCT_Cr
        buff_PrepCr = this->ReadTokens(read_PrepCr, write_PrepCr, buff_PrepCr, 1, 64, t_r_loop);
        sc_core::wait(DelayIDCT_Cr.GetDelay()); // IDCT_Cr
        buff_Cr = this->WriteTokens(read_Cr, write_Cr, buff_Cr, 1, 64, t_w_loop);  

        // IDCT_Cb 
        buff_PrepCb = this->ReadTokens(read_PrepCb, write_PrepCb, buff_PrepCb, 1, 64, t_r_loop);
        sc_core::wait(DelayIDCT_Cb.GetDelay()); // IDCT_Y
        buff_Cb = this->WriteTokens(read_Cb, write_Cb, buff_Cb, 1, 64, t_w_loop);

        // CreateRGBPixels

        buff_Y = this->ReadTokens(read_Y, write_Y, buff_Y, 1, 64, t_r_loop);
        buff_Cr = this->ReadTokens(read_Cr, write_Cr, buff_Cr, 1, 64, t_r_loop);
        buff_Cb = this->ReadTokens(read_Cb, write_Cb, buff_Cb, 1, 64, t_r_loop);

        sc_core::wait(DelayCreateRGBPixels.GetDelay()); // CreateRGBPixels     

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