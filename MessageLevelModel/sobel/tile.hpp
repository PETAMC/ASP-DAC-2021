#ifndef MB0_HPP
#define MB0_HPP

#include <core/master.hpp>
#include <delayvector.hpp>
#include <elementarydelay.hpp>

extern int pCount, wCount, rCount;

class Tile : public core::Master
{
    public:
        Tile(sc_core::sc_module_name name);
        ~Tile(){};

        virtual void Execute() = 0;

    protected:
        bool WriteTokens(sc_core::sc_event& read_event, sc_core::sc_event& write_event, bool BufferAvail, bool sameCore, int numTokens, int delayLoop);
        bool ReadTokens(sc_core::sc_event& read_event, sc_core::sc_event& write_event, bool BufferAvail, bool sameCore, int numTokens, int delayLoop);
        int Delay(int pCount, int wCount, int rCount, int numTokens, int delayOffset, int delayLoop);
    private:

};

#endif