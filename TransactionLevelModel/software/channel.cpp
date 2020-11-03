#include <software/channel.hpp>
#include <hardware/tile.hpp>


Channel::Channel(std::string name, unsigned int prate, unsigned int crate, unsigned int size, Monitor &monitor, COMMUNICATIONMODEL model)
            : sc_core::sc_module(sc_core::sc_module_name(name.c_str()))
            , name(name)
            , usageaddress(0)
            , indexaddress(0)
            , fifoaddress(0)
            , producerate(prate)
            , consumerate(crate)
            , fifosize(size)
            , consumertile(NULL)
            , producertile(NULL)
            , monitor(&monitor)
            , model(model)
{
};



void Channel::ChangeConsumerTile(Tile *tile)
{
    if(this->consumertile != NULL)
    {
        std::cerr << "\e[1;33mWARNING:\e[0m Changing associated consumer tile of channel that got already associated to a different tile!\n";
    }
    this->consumertile = tile;
}

void Channel::ChangeProducerTile(Tile *tile)
{
    if(this->producertile != NULL)
    {
        std::cerr << "\e[1;33mWARNING:\e[0m Changing associated producer tile of channel that got already associated to a different tile!\n";
    }
    this->producertile = tile;
}



void Channel::ReadTokens(token_t tokens[])
{
    switch(this->model)
    {
        case COMMUNICATIONMODEL::CYCLEACCURATE:
            this->ReadTokensCycleAccurate(tokens);
            break;

        case COMMUNICATIONMODEL::SYSTEMCEVENTS:
            this->ReadTokensEventBased(tokens);
            break;

        default:
            std::cerr << "\e[1;31mERROR:\e[0m Selected communication model for reading tokens not yet implemented!\n";
    }
}



void Channel::WriteTokens(token_t tokens[])
{
    switch(this->model)
    {
        case COMMUNICATIONMODEL::CYCLEACCURATE:
            this->WriteTokensCycleAccurate(tokens);
            break;

        case COMMUNICATIONMODEL::SYSTEMCEVENTS:
            this->WriteTokensEventBased(tokens);
            break;

        default:
            std::cerr << "\e[1;31mERROR:\e[0m Selected communication model for writing tokens not yet implemented!\n";
    }
}


// Cycle Accurate

void Channel::ReadTokensCycleAccurate(token_t tokens[])
{
    // Initialization Block
    unsigned int usage;
    sc_core::wait(1, sc_core::SC_NS);

    // Polling Block
    do
    {
        this->consumertile->ReadWord(this->usageaddress, &usage);

        sc_core::wait(1, sc_core::SC_NS);
        if(usage != 0)
            break;
        sc_core::wait(2, sc_core::SC_NS);
    }
    while(true);

    // Preparation Block
    unsigned int index;
    index = 0;
    sc_core::wait(5, sc_core::SC_NS);

    // Copy Block
    for(unsigned int tokenindex = 0; tokenindex < this->consumerate; tokenindex++)
    {
        unsigned long long address;
        address  = this->fifoaddress;
        address += index;
        this->consumertile->ReadWord(address, reinterpret_cast< unsigned int* >(&tokens[tokenindex]));
        index   += 1;

        sc_core::wait(5, sc_core::SC_NS);
        if(tokenindex+1 < this->consumerate)
            sc_core::wait(2, sc_core::SC_NS);
    }
    sc_core::wait(1, sc_core::SC_NS);

    // Update meta data
    usage = 0;
    this->consumertile->WriteWord(this->usageaddress, &usage);
    sc_core::wait(3, sc_core::SC_NS);
}



void Channel::WriteTokensCycleAccurate(token_t tokens[])
{
    // Initialization Block
    unsigned int usage;
    sc_core::wait(1, sc_core::SC_NS);

    // Polling Block
    do
    {
        this->producertile->ReadWord(this->usageaddress, &usage);
        sc_core::wait(1, sc_core::SC_NS);
        if(usage == 0)
            break;
        sc_core::wait(2, sc_core::SC_NS);

    }
    while(true);

    // Preparation Block
    unsigned int index;
    index = 0;
    sc_core::wait(4, sc_core::SC_NS);

    // Copy Block
    for(unsigned int tokenindex = 0; tokenindex < this->producerate; tokenindex++)
    {
        unsigned long long address;
        address  = this->fifoaddress;
        address += index;
        sc_core::wait(2, sc_core::SC_NS);
        this->producertile->WriteWord(address, reinterpret_cast< unsigned int* >(&tokens[tokenindex]));
        index   += 1;
        sc_core::wait(5, sc_core::SC_NS);
    }
    
    // Management Block
    usage = 1;
    this->producertile->WriteWord(this->usageaddress, &usage);
    sc_core::wait(3, sc_core::SC_NS);
}



// Event Based

void Channel::ReadTokensEventBased(token_t tokens[])
{
    // Initialization Block
    unsigned int usage;
    sc_core::wait(1, sc_core::SC_NS);

    // Polling Block
    this->consumertile->ReadWord(this->usageaddress, &usage);
    if(usage == 0)
        sc_core::wait(this->fullevent);

    // Preparation Block
    unsigned int index;
    index = 0;
    sc_core::wait(5, sc_core::SC_NS);

    // Copy Block
    for(unsigned int tokenindex = 0; tokenindex < this->consumerate; tokenindex++)
    {
        unsigned long long address;
        address  = this->fifoaddress;
        address += index;
        this->consumertile->ReadWord(address, reinterpret_cast< unsigned int* >(&tokens[tokenindex]));
        index   += 1;

        sc_core::wait(5, sc_core::SC_NS);
        if(tokenindex+1 < this->consumerate)
            sc_core::wait(2, sc_core::SC_NS);
    }
    sc_core::wait(1, sc_core::SC_NS);

    // Update meta data
    usage = 0;
    this->consumertile->WriteWord(this->usageaddress, &usage);
    this->emptyevent.notify();
    sc_core::wait(3, sc_core::SC_NS);
}



void Channel::WriteTokensEventBased(token_t tokens[])
{
    // Initialization Block
    unsigned int usage;
    sc_core::wait(1, sc_core::SC_NS);

    // Polling Block
    this->producertile->ReadWord(this->usageaddress, &usage);
    if(usage != 0)
        sc_core::wait(this->emptyevent);

    // Preparation Block
    unsigned int index;
    index = 0;
    sc_core::wait(4, sc_core::SC_NS);

    // Copy Block
    for(unsigned int tokenindex = 0; tokenindex < this->producerate; tokenindex++)
    {
        unsigned long long address;
        address  = this->fifoaddress;
        address += index;
        sc_core::wait(2, sc_core::SC_NS);
        this->producertile->WriteWord(address, reinterpret_cast< unsigned int* >(&tokens[tokenindex]));
        index   += 1;
        sc_core::wait(5, sc_core::SC_NS);
    }
    
    // Management Block
    usage = 1;
    this->producertile->WriteWord(this->usageaddress, &usage);
    this->fullevent.notify();
    sc_core::wait(3, sc_core::SC_NS);
}


// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

