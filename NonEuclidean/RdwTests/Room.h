#pragma once

#include "Object.h"

class Room : public Object
{
public:
    Room(float width, float depth)
    {
        mesh = AquireMesh("box.obj");
        shader = AquireShader("texture");
        texture = AquireTexture("three_room.bmp");
        scale = Vector3(width, 1.0f, depth);
    }

    void AddPortal(Object& portal, const Vector3& position) { portal.pos = pos + position; }
};
