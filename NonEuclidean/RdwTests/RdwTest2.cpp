#include "Corridor.h"
#include "Ground.h"
#include "RdwTests.h"
#include "Room.h"

void RdwTest2::Load(PObjectVec& objs, PPortalVec& portals, Player& player)
{
    auto p1 = std::make_shared<Portal>();
    auto p2 = std::make_shared<Portal>();
    auto p3 = std::make_shared<Portal>();
    auto p4 = std::make_shared<Portal>();

    auto room1 = std::make_shared<Room>(6.0f, 6.0f);
    room1->AddPortal(*p1, Vector3(-2.0f, 1.0f, 2.999f));
    objs.push_back(room1);

    auto room2 = std::make_shared<Room>(6.0f, 6.0f);
    room2->pos = Vector3(10.0f, 0.0f, 0.0f);
    room2->AddPortal(*p3, Vector3(2.0f, 1.0f, 2.999f));
    objs.push_back(room2);

    auto corridor = std::make_shared<Corridor>(corridor_shape);
    corridor->pos = Vector3(20.0f, 0.0f, 0.0f);
    objs.push_back(corridor);
    corridor->SetEntrancePortal(*p2);
    corridor->SetExitPortal(*p4);

    auto ground = std::make_shared<Ground>();
    ground->pos = Vector3(0.0f, -0.001f, 0.0f);
    ground->scale = Vector3(100.0f, 1.0f, 100.0f);
    objs.push_back(ground);

    Portal::Connect(p1, p2);
    Portal::Connect(p3, p4);

    portals.push_back(p1);
    portals.push_back(p2);
    portals.push_back(p3);
    portals.push_back(p4);

    player.SetPosition(Vector3(0.0f, GH_PLAYER_HEIGHT, 0.0f));
}
