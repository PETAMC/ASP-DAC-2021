#include <monitor.hpp>

Monitor::Monitor()
    : iterationstarted(0)
    , iterationended(0)
    , enabledurationoutput(true)
{
}

Monitor::~Monitor()
{
}



void Monitor::EnableDurationOutput(bool enable)
{
    this->enabledurationoutput = enable;
}



void Monitor::IterationBegin()
{
    this->iterationstarted++;
    this->iterationstarts.emplace(iterationstarted, sc_core::sc_time_stamp());
}



void Monitor::IterationEnd()
{
    // Get information about start time of the currently ended iteration
    this->iterationended++;

    auto startinfos = this->iterationstarts.find(this->iterationended);
    if(startinfos == this->iterationstarts.end())
    {
        std::cerr << "\e[1;31mERROR: \e[0mCannot find start time stamp for a finished iteration.\n";
        return;
    }

    // Calculate time difference (= iteration duration)
    sc_core::sc_time starttime;
    sc_core::sc_time stoptime;
    sc_core::sc_time duration;

    starttime = startinfos->second;
    stoptime  = sc_core::sc_time_stamp();
    duration  = stoptime - starttime;

    if(this->enabledurationoutput == true)
        std::cout << std::dec << duration.value() / 1000 << "\n";
}



// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

