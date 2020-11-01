#pragma once

#include "Corridor.h"
#include "RoomGraph.h"
#include "Scene.h"

class GraphTests
{
public:
    static RoomGraph test1();
    static RoomGraph test2();
    static RoomGraph test3();
    static RoomGraph2 test4();
    static RoomGraph test5();
};

class RdwTest2 : public Scene
{
public:
    RdwTest2(Corridor::Shape shape)
        : corridor_shape(shape)
    {
    }

    virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player) override;

private:
    Corridor::Shape corridor_shape;
};

class RdwTest3 : public Scene
{
public:
    virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player) override;
};
