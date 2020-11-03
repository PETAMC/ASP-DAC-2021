#ifndef SHAREDMEMORY_HPP
#define SHAREDMEMORY_HPP

#include <core/slave.hpp>
#include <channel.hpp>


class SharedMemory : public core::Memory
{
    public:
        SharedMemory(const char* name, sc_dt::uint64 address, size_t size, sc_core::sc_time readdelay, sc_core::sc_time writedelay)
            : Memory(name, size, readdelay, writedelay)
            , address(address), size(size), used(0), channels(0) {};

        unsigned int GetSize() const;
        sc_dt::uint64 GetAddress() const;

        SharedMemory& operator<< (Channel& channel); 

    private:
        unsigned long long AllocateMemory(size_t numbytes);
        Channel* GetChannelByOffset(unsigned long long offset);

        size_t size; // actual size in words
        size_t used; // used words
        sc_dt::uint64 address;
        std::vector<Channel*> channels;
};


#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

