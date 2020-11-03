#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <map>
#include <string>
#include <systemc>

class Monitor
{
    public:
        Monitor();
        ~Monitor();

        void IterationBegin();
        void IterationEnd();

        void EnableDurationOutput(bool enable=true);
        

    private:
        bool enabledurationoutput;
        unsigned int iterationstarted;
        unsigned int iterationended;
        std::map<unsigned int, sc_core::sc_time> iterationstarts;
};


#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

