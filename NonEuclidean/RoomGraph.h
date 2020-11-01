#pragma once

#include "Portal.h"

constexpr static const int MaxPortals = 4;

struct RoomInfo
{
    int   type;
    float w;
    float h;
    float portals[MaxPortals];
};

struct RoomGraph
{
    std::vector<RoomInfo>          rooms;
    std::vector<std::vector<bool>> adjacency;
};

void create_ve_from_graph(const RoomGraph& graph, PObjectVec& objs, PPortalVec& portals);
