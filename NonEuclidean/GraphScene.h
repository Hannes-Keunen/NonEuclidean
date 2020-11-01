#pragma once

#include "RoomGraph.h"
#include "Scene.h"

class GraphScene : public Scene
{
public:
    GraphScene(const RoomGraph& _graph)
        : graph(_graph)
    {
    }

    virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player)
    {
        create_ve_from_graph(graph, objs, portals);
    }

private:
    RoomGraph graph;
};

class GraphScene2 : public Scene
{
public:
    GraphScene2(const RoomGraph2& _graph)
        : graph(_graph)
    {
    }

    virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player)
    {
        create_ve_from_graph2(graph, objs, portals);
    }

private:
    RoomGraph2 graph;
};
