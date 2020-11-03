#include <iostream>

#include <software/actor.hpp>

Actor::Actor(std::string name, DelayVectorMap &delaymap, Monitor &monitor)
    : name(name)
    , delayvectormap(&delaymap)
    , monitor(&monitor)
    , isstartactor(false)
    , isfinishactor(false)
{
    try
    {
        this->SelectFeature("none");
    }
    catch(const std::exception &e)
    {
        std::cerr << "\e[1;33mWARNING:\e[0m No default architecture feature set for Actor "
                  << name
                  << ".\n"
                  << "\e[1;30m(Better provide characteristics for architectures without special features)"
                  << "\e[0m\n";
    }
}



void Actor::DefineAsStartActor()
{
    this->isstartactor = true;
}



void Actor::DefineAsFinishActor()
{
    this->isfinishactor = true;
}




void Actor::ChangeTile(Tile *tile)
{
    if(this->tile != NULL)
    {
        std::cerr << "\e[1;33mWARNING:\e[0m Changing associated tile of an actor that got already mapped onto a different tile!\n";
    }

    this->tile = tile;

    for(auto channel : this->channels_out)
        channel->ChangeProducerTile(this->tile);
    for(auto channel : this->channels_in)
        channel->ChangeConsumerTile(this->tile);
}



void Actor::SelectFeature(std::string feature)
{
    if(this->delayvectormap == nullptr)
    {
        std::cerr << "\e[1;31mERROR:\e[0m Actor "
                  << this->name
                  << " does not support selecting different feature! "
                  << "No DelayVectorMap available.\n";
        throw std::runtime_error("No DelayVectorMap available");
    }

    try
    {
        this->delayvector = this->delayvectormap->at(feature);
    }
    catch(const std::out_of_range &e)
    {
        std::cerr << "\e[1;31mERROR:\e[0m Selected feature "
                  << feature
                  << " does not exist for actor "
                  << this->name
                  << "!\n";
        throw;
    }
}



Actor& Actor::operator<< (Channel &incoming)
{
    this->channels_in.push_back(&incoming);
    incoming.ChangeConsumerTile(this->tile);
    return *this;
}

Actor& Actor::operator>> (Channel &outgoing)
{
    this->channels_out.push_back(&outgoing);
    outgoing.ChangeProducerTile(this->tile);
    return *this;
}


void Actor::Execute()
{
    if(this->monitor != NULL and this->isstartactor == true)
        this->monitor->IterationBegin();

    this->ReadPhase();
    this->ComputePhase();
    this->WritePhase();

    if(this->monitor != NULL and this->isfinishactor == true)
        this->monitor->IterationEnd();
}



void Actor::Initialize()
{
}



void Actor::ReadPhase()
{
}



void Actor::ComputePhase()
{
}



void Actor::WritePhase()
{
}

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

