#ifndef BUS_HPP
#define BUS_HPP

#include <core/bus.hpp>
#include <tile.hpp>
#include <sharedmemory.hpp>

class Bus
    : public core::Bus
{
    public:
        Bus(const char* name)
            : core::Bus(name) {};

        Bus& operator<< (Tile& tile); 
        Bus& operator<< (SharedMemory& sharedmemory); 
};


#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

