#include <tile.hpp>
#include <iostream>
#include <cstdlib>
#include <fstream>

Tile::Tile(sc_core::sc_module_name name)
    : core::Master(sc_core::sc_module_name(name))
{
}

bool Tile::WriteTokens(sc_core::sc_event& read_event, sc_core::sc_event& write_event, bool BufferAvail, bool sameCore, int numTokens, int delayLoop)
{
    int delayOffset;

    //Polling
    if (BufferAvail != 1 & sameCore != 1)
    {
        pCount = pCount + 1;
        wait(read_event);
        pCount = pCount - 1;
    }

    //Copy data
    wCount = wCount + 1;
//    sc_core::wait(2, sc_core::SC_NS);    
    delayOffset= DelayOffsetWrite(numTokens);
    sc_core::wait(this->Delay(pCount, wCount, rCount, numTokens, delayOffset, delayLoop), sc_core::SC_NS);
    wCount = wCount - 1;

    write_event.notify();
    BufferAvail = 0;

    return BufferAvail;
}

bool Tile::ReadTokens(sc_core::sc_event& read_event, sc_core::sc_event& write_event, bool BufferAvail, bool sameCore, int numTokens, int delayLoop)
{
    int delayOffset;

    //Polling 
    if(BufferAvail != 0 & sameCore != 1)
    {
        pCount = pCount + 1;
        wait(write_event);
        pCount = pCount - 1;
    }

    rCount = rCount + 1;
    delayOffset = DelayOffsetRead(numTokens); 
    sc_core::wait(this->Delay(pCount, wCount, rCount, numTokens, delayOffset, delayLoop), sc_core::SC_NS);
    rCount = rCount - 1;

    read_event.notify();
    BufferAvail = 1;
    return BufferAvail;
}

int Tile::Delay(int pCount, int wCount, int rCount, int numTokens, int delayOffset, int delayLoop)
{
    int delay;
    if (pCount+wCount+rCount<=2) // two cores
    {
        if (pCount*t_p_loop+wCount*t_w_loop+rCount*t_r_loop-(pCount+rCount+wCount)*delayLoop > 0)  
            delay = delayOffset + (pCount*t_p_loop+wCount*t_w_loop+rCount*t_r_loop-(pCount+rCount+wCount)*delayLoop)*(numTokens+2); // 1 last polling + n tokens + 1 token update usage
        else
            delay = delayOffset;
    }

    else 
    {
        if (pCount*t_p+wCount*t_w+rCount*t_r-delayLoop > 0)    // n cores (n>2) 
            delay = delayOffset + (pCount*t_p+wCount*t_w+rCount*t_r-delayLoop)*(numTokens+2); // 1 last polling + n tokens + 1 token update usage
        else
            delay = delayOffset + (t_r_loop-t_w_loop)*(numTokens+2); // 1 polling tokens + n tokens + 1 token update usage;
    }
        
    return delay;
}