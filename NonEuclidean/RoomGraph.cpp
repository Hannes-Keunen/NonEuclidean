#include "RoomGraph.h"
#include "RdwTests/Room.h"

static void place_portal(std::shared_ptr<Room>& room, std::shared_ptr<Portal> portal, float p)
{
    float   w = room->scale.x;
    float   h = room->scale.z;
    Vector3 position;
    if (p < w)
    {
        // back (+z) wall
        position = Vector3(-w / 2.0f + p, 1.0f, h / 2.0f - 0.001f);
    }
    else if (p < w + h)
    {
        // right wall
        p -= (w);
        position = Vector3(w / 2.0f - 0.001f, 1.0f, h / 2.0f - p);
        portal->euler = Vector3(0.0f, M_PI_2, 0.0f);
    }
    else if (p < w * 2 + h)
    {
        // front (-z) wall
        p -= (w + h);
        position = Vector3(w / 2.0f - p, 1.0f, -h / 2.0f + 0.001f);
    }
    else if (p < w * 2 + h * 2)
    {
        // left wall
        p -= (w * 2 + h);
        position = Vector3(-w / 2.0f + 0.001f, 1.0f, -h / 2.0f + p);
        portal->euler = Vector3(0.0f, M_PI_2, 0.0f);
    }

    room->AddPortal(*portal, position);
}

void create_ve_from_graph(const RoomGraph& graph, PObjectVec& objs, PPortalVec& portals)
{
    printf("==new ve==\n");
    printf("Rooms:\n");
    std::vector<std::shared_ptr<Room>> rooms;
    rooms.reserve(graph.rooms.size());
    float room_x = 0.0f;
    for (const auto& room_info : graph.rooms)
    {
        if (room_x != 0.0f)
        {
            room_x += room_info.w;
        }

        printf(" #%d: (%f, %f)\n", rooms.size(), room_info.w, room_info.h);
        auto room = std::make_shared<Room>(room_info.w, room_info.h);
        room->pos = Vector3(room_x, 0.0f, 0.0f);
        rooms.push_back(room);
        objs.push_back(room);
        room_x += room_info.w;
    }

    printf("Portals:\n");
    std::vector<int> placed_portals(rooms.size(), 0);
    for (int y = 0; y < graph.adjacency.size(); y++)
    {
        auto& row = graph.adjacency[y];
        // only go through upper right triangle
        for (int x = y + 1; x < row.size(); x++)
        {
            if (row[x])
            {
                // add portal
                auto& room1 = rooms[y];
                auto& room2 = rooms[x];
                auto& info1 = graph.rooms[y];
                auto& info2 = graph.rooms[x];

                if (placed_portals[x] >= MaxPortals || placed_portals[y] >= MaxPortals)
                {
                    // skip this portal because one of the rooms already has the maximum number of portals
                    continue;
                }

                // clang-format off
                printf(" #%d: room %d(#%d: p=%f) -> room %d(#%d: p=%f)\n",
                        portals.size() / 2,
                        y, placed_portals[y], info1.portals[placed_portals[y]],
                        x, placed_portals[x], info2.portals[placed_portals[x]]);
                // clang-format on

                auto p1 = std::make_shared<Portal>();
                auto p2 = std::make_shared<Portal>();

                place_portal(room1, p1, info1.portals[placed_portals[y]]);
                place_portal(room2, p2, info2.portals[placed_portals[x]]);

                Portal::Connect(p1, p2);

                portals.push_back(p1);
                portals.push_back(p2);

                placed_portals[y]++;
                placed_portals[x]++;
            }
        }
    }
}
