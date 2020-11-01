#pragma once

#include "Object.h"
#include "Resources.h"

class Corridor : public Object
{
public:
    enum Shape
    {
        Straight,
        U,
        C,
    };

public:
    Corridor(Shape shape)
    {
        switch (shape)
        {
            case U: mesh = AquireMesh("corridor-u.obj"); break;
            case C: mesh = AquireMesh("corridor-c.obj"); break;
        }

        shader = AquireShader("texture");
        texture = AquireTexture("three_room.bmp");
    }

    void SetEntrancePortal(Object& portal) { portal.pos = LocalToWorld().MulPoint(Vector3(1.0f, 1.0f, 0.001f)); }

    void SetExitPortal(Object& portal) { portal.pos = LocalToWorld().MulPoint(Vector3(8.0f, 1.0f, 0.001f)); }
};
