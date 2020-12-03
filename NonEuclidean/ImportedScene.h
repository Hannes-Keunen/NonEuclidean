#pragma once

#include "Scene.h"

class ImportedScene : public Scene
{
public:
    ImportedScene(const char* filename)
        : filename(filename)
    {
    }

    void Load(PObjectVec& objs, PPortalVec& portals, Player& player);

private:
    const char* filename;
};
