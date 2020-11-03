#pragma once

#include "Portal.h"

constexpr static const int MaxPortals = 4;
constexpr static const int MaxRooms = 16;

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

struct RoomInfo2
{
    int   type;
    float w;
    float h;
    float adjacency[MaxRooms];
};

struct RoomGraph2
{
    RoomInfo2 rooms[MaxRooms];
};

void create_ve_from_graph(const RoomGraph& graph, PObjectVec& objs, PPortalVec& portals);
void create_ve_from_graph2(const RoomGraph2& graph, PObjectVec& objs, PPortalVec& portals);
void write_to_csv(const char* filename, const RoomGraph2& graph);
