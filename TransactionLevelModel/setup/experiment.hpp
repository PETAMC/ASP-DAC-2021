#ifndef EXPERIMENTSETUP_HPP
#define EXPERIMENTSETUP_HPP

#include <unordered_map>
#include <tinyxml2.h>

#include <software/actor.hpp>
#include <software/channel.hpp>
#include <hardware/tile.hpp>
#include <hardware/memory.hpp>

using namespace tinyxml2;

typedef std::unordered_map<std::string, Tile*>  TileMap;
typedef std::unordered_map<std::string, Actor*> ActorMap;
typedef std::unordered_map<std::string, Channel*> ChannelMap;
typedef std::unordered_map<std::string, Memory*> MemoryMap;

class Experiment
{
    public:
        Experiment(std::string &configpath);
        ~Experiment();

        bool LoadActorMapping(TileMap &tilemap, ActorMap &actormap);
        bool LoadChannelMapping(MemoryMap &memorymap, TileMap &tilemap, ChannelMap &channelmap);

    private:

        XMLDocument xmlfile;
        XMLNode *applicationnode;
        XMLNode *actormappingnode;
        XMLNode *channelmappingnode;
};

#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

