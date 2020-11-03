#include <sharedmemory.hpp>

unsigned int SharedMemory::GetSize() const
{
    return this->size;
}

sc_dt::uint64 SharedMemory::GetAddress() const
{
    return this->address;
}

unsigned long long SharedMemory::AllocateMemory(size_t numofwords)
{
    if(this->used + numofwords > this->size)
        return 0;

    unsigned long long newaddress;
    newaddress  = this->address + this->used;
    this->used += numofwords;

    return newaddress;
}



SharedMemory& SharedMemory::operator<< (Channel& channel)
{
    unsigned int memorysize = channel.fifosize + 2; // +2 for usage and firsttoken token
    if(this->used + memorysize > this->size)
        return *this;

    // calculate and assign new address for channel
    channel.usageaddress = this->AllocateMemory(1);
    channel.indexaddress = this->AllocateMemory(1);
    channel.fifoaddress  = this->AllocateMemory(channel.fifosize);
    std::cerr << "\e[0;36mMapping channel to 0x" << std::hex << channel.fifoaddress 
              << " with size "                   << std::dec << channel.fifosize + 2
              << " \e[1;30mName: "               << channel.name
              << std::endl;

    // remember channel
    this->channels.push_back(&channel);
    return *this;
}


Channel* SharedMemory::GetChannelByOffset(unsigned long long offset)
{
    for(size_t i = 0; i < this->channels.size(); i++)
    {
        Channel* channel;
        unsigned long long channeloffset;
        unsigned int channelsize;

        channel       = this->channels[i];
        channeloffset = channel->usageaddress - this->address;  // usage is the first address managed by the SM
        channelsize   = channel->fifosize + 2;  // + usage and findex address
        if(channeloffset <= offset && channeloffset + channelsize > offset)
            return channel;
    }
    return NULL;
}



// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

