#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <sdf.h>
#define BYTES_PER_TOKEN 4

struct Channel
{
    Channel(const char* name, unsigned int prate, unsigned int crate, unsigned int size)
        : name(name)
        , usageaddress(0)
        , indexaddress(0)
        , fifoaddress(0)
        , producerate(prate)
        , consumerate(crate)
        , fifosize(size)
    {
    };
    const char* name;
    unsigned long long usageaddress; // Virtual address
    unsigned long long indexaddress; // Virtual address - index of the first token in the FIFO
    unsigned long long fifoaddress;  // Virtual address
    unsigned int producerate;
    unsigned int consumerate;

    unsigned int fifosize; // in tokens
};


#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

