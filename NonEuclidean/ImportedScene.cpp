#include "ImportedScene.h"

#include "Ground.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct PortalPos
{
    Vector3 pos;
    float angle;

    static PortalPos from_json(json& j)
    {
        return {
            .pos = Vector3(j[0], 1, j[1]),
            .angle = j[2],
        };
    }
};

struct RoomType
{
    bool is_corridor;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;
    Vector3 scale;
    std::vector<PortalPos> portal_positions;
};

auto create_corridor_mesh(const std::vector<Vector3>& points) -> std::shared_ptr<Mesh>
{
    std::vector<float> verts;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<Collider> colliders;

    auto vertex = [&](const Vector3& vert) {
        verts.push_back(vert.x);
        verts.push_back(vert.y);
        verts.push_back(vert.z);
    };

    auto uv = [&](const Vector3& uv) {
        uvs.push_back(uv.x);
        uvs.push_back(uv.y);
    };

    auto normal = [&](const Vector3& normal) {
        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    };

    auto quad = [&](const Vector3& center, const Vector3& min, const Vector3& max, const Vector3& norm) {
        Vector3 v1 = center + Vector3(min.x, min.y, max.z); // front left
        Vector3 v2 = center + Vector3(min.x, min.y, min.z); // back left
        Vector3 v3 = center + Vector3(max.x, min.y, min.z); // back right
        Vector3 v4 = center + Vector3(max.x, min.y, max.z); // front right

        float w = abs(min.x - max.x);
        float d = abs(min.z - max.z);
        Vector3 vt1 = Vector3(0, d / 8, 0);     // front left
        Vector3 vt2 = Vector3(0, 0, 0);         // back left
        Vector3 vt3 = Vector3(w / 8, 0, 0);     // back right
        Vector3 vt4 = Vector3(w / 8, d / 8, 0); // front right

        // CCW vertex order!

        vertex(v1);
        uv(vt1);
        normal(norm);

        vertex(v3);
        uv(vt3);
        normal(norm);

        vertex(v2);
        uv(vt2);
        normal(norm);

        colliders.emplace_back(v1, v3, v2);

        vertex(v1);
        uv(vt1);
        normal(norm);

        vertex(v4);
        uv(vt4);
        normal(norm);

        vertex(v3);
        uv(vt3);
        normal(norm);

        colliders.emplace_back(v1, v4, v3);
    };

    for (int i = 0; i < points.size(); i++)
    {
        const auto& next = points[i];

        if (i > 0)
        {
            const auto& prev = points[i - 1];
            auto center = (prev + next) / 2;

            auto distance = prev - next;
            float xmin, xmax, zmin, zmax;
            if (distance.x == 0)
            {
                // vertical
                distance.z = abs(distance.z);
                xmin = -1;
                xmax = 1;
                zmin = -(distance.z - 2) / 2;
                zmax = (distance.z - 2) / 2;
            }
            else
            {
                // horizontal
                distance.x = abs(distance.x);
                xmin = -(distance.x - 2) / 2;
                xmax = (distance.x - 2) / 2;
                zmin = -1;
                zmax = 1;
            }

            if ((distance.x == 0 && distance.z > 2) || (distance.z == 0 && distance.x > 2))
            {
                // only add a corridor segment if the distance is greater than the width of a corridor
                quad(center, Vector3(xmin, 0, zmin), Vector3(xmax, 0, zmax), Vector3(0, 1, 0));  // floor
                quad(center, Vector3(xmax, 3, zmin), Vector3(xmin, 3, zmax), Vector3(0, -1, 0)); // ceiling
            }
        }

        quad(points[i], Vector3(-1, 0, -1), Vector3(1, 0, 1), Vector3(0, 1, 0));
        quad(points[i], Vector3(1, 3, -1), Vector3(-1, 3, 1), Vector3(0, -1, 0));
    }

    return std::make_shared<Mesh>(verts, uvs, normals, colliders);
}

Vector3 avg(const std::vector<Vector3>& points)
{
    Vector3 sum(0, 0, 0);
    for (const auto& point : points) { sum += point; }
    return sum / points.size();
}

struct Corridor : Object
{
    PortalPos p1, p2;

    Corridor(const PortalPos& p1, const PortalPos& p2, const std::vector<Vector3>& points)
        : p1(p1)
        , p2(p2)
    {
        auto avg_point = avg(points);
        std::vector<Vector3> new_points = points;
        for (auto& point : new_points) point -= avg_point;
        this->p1.pos -= avg_point;
        this->p2.pos -= avg_point;

        mesh = create_corridor_mesh(new_points);
        scale = Vector3(1);
    }
};

void ImportedScene::Load(PObjectVec& objs, PPortalVec& portals, Player& player)
{
    std::ifstream file(filename);

    json j;
    file >> j;

    std::vector<RoomType> room_types;
    for (auto& obj : j["room_types"])
    {
        room_types.emplace_back();
        auto& type = room_types.back();
        type.is_corridor = obj["is_corridor"];
        type.texture = AquireTexture("three_room.bmp");
        if (!type.is_corridor)
        {
            type.mesh = AquireMesh(static_cast<std::string>(obj["obj"]).c_str());
            type.scale = Vector3(obj["scale"][0], 1, obj["scale"][1]);
            for (auto& pp : obj["portal_positions"]) { type.portal_positions.push_back(PortalPos::from_json(pp)); }
        }
    }

    std::vector<std::shared_ptr<Object>> rooms;

    int room_x = 0;
    for (auto& obj : j["rooms"])
    {
        auto& type = room_types[obj["type"]];

        std::shared_ptr<Object> room; // = std::make_shared<Object>();
        if (type.is_corridor)
        {
            std::vector<Vector3> points;
            for (auto& point_obj : obj["points"]) { points.emplace_back(point_obj[0], 0, point_obj[1]); }

            if (!obj["portals"][1].is_null())
            {
                room = std::make_shared<Corridor>(
                    PortalPos::from_json(obj["portals"][0]), PortalPos::from_json(obj["portals"][1]), points);
                room->scale = Vector3(1, 1, 1);
            }
            else
            {
                room = nullptr;
            }
        }
        else
        {
            room = std::make_shared<Object>();
            room->mesh = type.mesh;
            room->scale = type.scale;
        }

        if (room != nullptr)
        {
            room->shader = AquireShader("texture");
            room->texture = type.texture;
            room->pos = Vector3(room_x, 0, 0);
            objs.push_back(room);
        }
        rooms.push_back(room);
        room_x += 20;
    }

    for (auto& obj : j["portals"])
    {
        int room1_idx = obj["room1"];
        int room2_idx = obj["room2"];

        // printf("Portal: %d -> %d\n", room1_idx, room2_idx);

        auto& room1 = rooms[room1_idx];
        auto& room2 = rooms[room2_idx];

        auto& room1_type = room_types[j["rooms"][room1_idx]["type"]];
        auto& room2_type = room_types[j["rooms"][room2_idx]["type"]];

        if (obj["p1"].is_null() || obj["p2"].is_null())
            continue;

        int p1_idx = obj["p1"];
        int p2_idx = obj["p2"];

        auto get_portal_pos = [](const RoomType& type, const std::shared_ptr<Object>& room, int portal_idx) {
            if (type.is_corridor)
            {
                return (
                    portal_idx == 0 ? std::dynamic_pointer_cast<Corridor>(room)->p1
                                    : std::dynamic_pointer_cast<Corridor>(room)->p2);
            }
            else
            {
                return type.portal_positions[portal_idx];
            }
        };

        PortalPos p1_pos = get_portal_pos(room1_type, room1, p1_idx);
        PortalPos p2_pos = get_portal_pos(room2_type, room2, p2_idx);

        auto p1 = std::make_shared<Portal>();
        auto p2 = std::make_shared<Portal>();

        auto inset_pos = [](const Vector3& pos) {
            auto sign = [](int i) {
                return i < 0 ? -1 : (i > 0 ? 1 : 0);
            };

            return Vector3(pos.x - sign(pos.x) * 0.001, pos.y, pos.z - sign(pos.z) * 0.001);
        };

        p1->pos = room1->pos + inset_pos(Vector3(p1_pos.pos.x, 1, p1_pos.pos.z));
        p2->pos = room2->pos + inset_pos(Vector3(p2_pos.pos.x, 1, p2_pos.pos.z));

        auto to_radians = [](float deg) {
            return deg / 180 * M_PI;
        };

        float angle = to_radians(static_cast<int>(180 + p1_pos.angle) % 180);
        p1->euler = Vector3(0, angle, 0);
        p2->euler = Vector3(0, angle, 0);

        Portal::Connect(p1, p2);
        portals.push_back(p1);
        portals.push_back(p2);
    }

    player.pos = Vector3(0, GH_PLAYER_HEIGHT, 0);

    // auto ground = std::shared_ptr<Object>(new Ground());
    // ground->scale = Vector3(10, 1, 10);
    // objs.push_back(ground);
}
