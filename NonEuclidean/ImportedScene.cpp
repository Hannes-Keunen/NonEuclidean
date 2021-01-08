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

auto create_corridor_mesh(const Vector3& entrance, const Vector3& exit, const std::vector<Vector3>& points)
    -> std::shared_ptr<Mesh>
{
    std::vector<float> verts;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<Collider> colliders;

    auto vertex = [&](const Vector3& vert, const Vector3& uv, const Vector3& normal) {
        verts.push_back(vert.x);
        verts.push_back(vert.y);
        verts.push_back(vert.z);

        uvs.push_back(uv.x);
        uvs.push_back(uv.y);

        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    };

    enum Faces
    {
        Left = 0x01,
        Right = 0x02,
        Front = 0x04,
        Back = 0x08,
    };

    auto segment = [&](const Vector3& center, const Vector3& scale, uint8_t exclude) {
        Vector3 v_blb(center.x - scale.x / 2, center.y, center.z - scale.z / 2);           // bottom left back
        Vector3 v_brb(center.x + scale.x / 2, center.y, center.z - scale.z / 2);           // bottom right back
        Vector3 v_blf(center.x - scale.x / 2, center.y, center.z + scale.z / 2);           // bottom left front
        Vector3 v_brf(center.x + scale.x / 2, center.y, center.z + scale.z / 2);           // bottom left front
        Vector3 v_tlb(center.x - scale.x / 2, center.y + scale.y, center.z - scale.z / 2); // top left back
        Vector3 v_trb(center.x + scale.x / 2, center.y + scale.y, center.z - scale.z / 2); // top right back
        Vector3 v_tlf(center.x - scale.x / 2, center.y + scale.y, center.z + scale.z / 2); // top left front
        Vector3 v_trf(center.x + scale.x / 2, center.y + scale.y, center.z + scale.z / 2); // top left front

        Vector3 vt(0, 0, 0);

        // floor
        vertex(v_blf, vt, Vector3(0, 1, 0));
        vertex(v_brb, vt, Vector3(0, 1, 0));
        vertex(v_blb, vt, Vector3(0, 1, 0));
        colliders.emplace_back(v_blb, v_brb, v_blf);
        vertex(v_blf, vt, Vector3(0, 1, 0));
        vertex(v_brf, vt, Vector3(0, 1, 0));
        vertex(v_brb, vt, Vector3(0, 1, 0));
        colliders.emplace_back(v_blf, v_brb, v_brf);

        // ceiling
        vertex(v_tlb, vt, Vector3(0, 1, 0));
        vertex(v_trb, vt, Vector3(0, 1, 0));
        vertex(v_tlf, vt, Vector3(0, 1, 0));
        colliders.emplace_back(v_tlb, v_trb, v_tlf);
        vertex(v_tlf, vt, Vector3(0, 1, 0));
        vertex(v_trb, vt, Vector3(0, 1, 0));
        vertex(v_trf, vt, Vector3(0, 1, 0));
        colliders.emplace_back(v_tlf, v_trb, v_trf);

        if (!(exclude & Left))
        {
            // left
            vertex(v_blf, vt, Vector3(1, 0, 0));
            vertex(v_tlb, vt, Vector3(1, 0, 0));
            vertex(v_tlf, vt, Vector3(1, 0, 0));
            // colliders.emplace_back(v_blf, v_tlb, v_tlf);
            vertex(v_blf, vt, Vector3(1, 0, 0));
            vertex(v_blb, vt, Vector3(1, 0, 0));
            vertex(v_tlb, vt, Vector3(1, 0, 0));
            // colliders.emplace_back(v_blf, v_blb, v_tlb);
        }

        if (!(exclude & Right))
        {
            // right
            vertex(v_brf, vt, Vector3(1, 0, 0));
            vertex(v_trf, vt, Vector3(1, 0, 0));
            vertex(v_trb, vt, Vector3(1, 0, 0));
            // colliders.emplace_back(v_brf, v_trf, v_trb);
            vertex(v_brf, vt, Vector3(1, 0, 0));
            vertex(v_trb, vt, Vector3(1, 0, 0));
            vertex(v_brb, vt, Vector3(1, 0, 0));
            // colliders.emplace_back(v_brf, v_trb, v_brb);
        }

        if (!(exclude & Front))
        {
            // front
            vertex(v_blf, vt, Vector3(0, 0, 1));
            vertex(v_tlf, vt, Vector3(0, 0, 1));
            vertex(v_trf, vt, Vector3(0, 0, 1));
            // colliders.emplace_back(v_blf, v_trf, v_tlf);
            vertex(v_blf, vt, Vector3(0, 0, 1));
            vertex(v_trf, vt, Vector3(0, 0, 1));
            vertex(v_brf, vt, Vector3(0, 0, 1));
            // colliders.emplace_back(v_blf, v_trf, v_brf);
        }

        if (!(exclude & Back))
        {
            // back
            vertex(v_blb, vt, Vector3(0, 0, 1));
            vertex(v_trb, vt, Vector3(0, 0, 1));
            vertex(v_tlb, vt, Vector3(0, 0, 1));
            // colliders.emplace_back(v_blb, v_trb, v_tlb);
            vertex(v_blb, vt, Vector3(0, 0, 1));
            vertex(v_brb, vt, Vector3(0, 0, 1));
            vertex(v_trb, vt, Vector3(0, 0, 1));
            // colliders.emplace_back(v_blb, v_tlb, v_brb);
        }
    };

    static constexpr const float CorridorWidth = 2.0f;
    static constexpr const float CorridorHeight = 3.0f;

    Faces in, out;
    {
        auto distance = points[0] - entrance;
        if (distance.x == 0)
        {
            in = distance.z < 0 ? Front : Back;
        }
        else if (distance.z == 0)
        {
            in = distance.x < 0 ? Right : Left;
        }
    }

    for (int i = 0; i < points.size(); i++)
    {
        const auto& current = points[i];

        Vector3 next;
        if (i < points.size() - 1)
        {
            next = points[i + 1];
        }
        else
        {
            next = exit;
        }

        Vector3 prev;
        if (i > 0)
        {
            prev = points[i - 1];
            auto center = (prev + current) / 2;

            auto distance = prev - current;
            distance.x = abs(distance.x);
            distance.z = abs(distance.z);
            if (distance.x == 0 && distance.z > CorridorWidth)
            {
                // vertical
                float length = distance.z - CorridorWidth;
                segment(center, Vector3(CorridorWidth, CorridorHeight, length), Front | Back);
            }
            else if (distance.z == 0 && distance.x > CorridorWidth)
            {
                // horizontal
                float length = distance.x - CorridorWidth;
                segment(center, Vector3(length, CorridorHeight, CorridorWidth), Left | Right);
            }
        }

        Faces in, out;
        {
            auto d_in = current - prev;
            if (d_in.x == 0)
            {
                in = d_in.z < 0 ? Front : Back;
            }
            else if (d_in.z == 0)
            {
                in = d_in.x < 0 ? Right : Left;
            }

            auto d_out = next - current;
            if (d_out.x == 0)
            {
                out = d_out.z < 0 ? Back : Front;
            }
            else if (d_out.z == 0)
            {
                out = d_out.x < 0 ? Left : Right;
            }
        }

        segment(points[i], Vector3(2, 3, 2), in | out);
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

        mesh = create_corridor_mesh(this->p1.pos, this->p2.pos, new_points);
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

        std::shared_ptr<Object> room;
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
}
