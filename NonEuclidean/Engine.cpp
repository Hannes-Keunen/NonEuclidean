#include "Engine.h"
#include "GraphScene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"
#include "Level5.h"
#include "Level6.h"
#include "Physical.h"
#include "RdwTests/RdwTests.h"

#include <algorithm>
#include <cmath>
#include <iostream>

Engine*      GH_ENGINE = nullptr;
Player*      GH_PLAYER = nullptr;
const Input* GH_INPUT = nullptr;
int          GH_REC_LEVEL = 0;
int64_t      GH_FRAME = 0;

Engine::Engine()
{
    GH_ENGINE = this;
    GH_INPUT = &input;
    isFullscreen = false;

    CreateGLWindow();
    InitGLObjects();
    input.SetupCallbacks(window);

    player.reset(new Player);
    GH_PLAYER = player.get();

    // vScenes.push_back(std::shared_ptr<Scene>(new Level1));
    // vScenes.push_back(std::shared_ptr<Scene>(new Level2(3)));
    // vScenes.push_back(std::shared_ptr<Scene>(new Level2(6)));
    // vScenes.push_back(std::shared_ptr<Scene>(new Level3));
    // vScenes.push_back(std::shared_ptr<Scene>(new RdwTest1));
    // vScenes.push_back(std::shared_ptr<Scene>(new RdwTest2(Corridor::U)));
    // vScenes.push_back(std::shared_ptr<Scene>(new RdwTest2(Corridor::C)));
    // vScenes.push_back(std::shared_ptr<Scene>(new RdwTest3));
    // vScenes.push_back(std::shared_ptr<Scene>(new Level4));
    // vScenes.push_back(std::shared_ptr<Scene>(new Level5));
    // vScenes.push_back(std::shared_ptr<Scene>(new Level6));

    vScenes.push_back(std::shared_ptr<Scene>(new GraphScene(GraphTests::test1())));
    vScenes.push_back(std::shared_ptr<Scene>(new GraphScene(GraphTests::test2())));
    vScenes.push_back(std::shared_ptr<Scene>(new GraphScene2(GraphTests::test3())));
    vScenes.push_back(std::shared_ptr<Scene>(new GraphScene2(GraphTests::test4())));
    vScenes.push_back(std::shared_ptr<Scene>(new GraphScene(GraphTests::test5())));
    vScenes.push_back(std::shared_ptr<Scene>(new GraphScene2(GraphTests::test6())));

    write_to_csv("exported/env_0.csv", GraphTests::test4());
    write_to_csv("exported/env_1.csv", GraphTests::test6());
    write_to_csv("exported/env_2.csv", GraphTests::test3());

    LoadScene(0);

    sky.reset(new Sky);
}

Engine::~Engine()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int Engine::Run()
{
    // Setup the timer
    double cur_time = timer.GetSeconds();
    GH_FRAME = 0;

    // Game loop
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        input.Update();

        if (input.key_press['1'])
        {
            LoadScene(0);
        }
        else if (input.key_press['2'])
        {
            LoadScene(1);
        }
        else if (input.key_press['3'])
        {
            LoadScene(2);
        }
        else if (input.key_press['4'])
        {
            LoadScene(3);
        }
        else if (input.key_press['5'])
        {
            LoadScene(4);
        }
        else if (input.key_press['6'])
        {
            LoadScene(5);
        }
        else if (input.key_press['7'])
        {
            LoadScene(6);
        }

        // Used fixed time steps for updates
        const double new_time = timer.GetSeconds();
        for (int i = 0; cur_time < new_time && i < GH_MAX_STEPS; ++i)
        {
            Update();
            cur_time += GH_DT;
            GH_FRAME += 1;
            input.EndFrame();
        }
        cur_time = (cur_time < new_time ? new_time : cur_time);

        // Setup camera for rendering
        const float n = GH_CLAMP(NearestPortalDist() * 0.5f, GH_NEAR_MIN, GH_NEAR_MAX);
        main_cam.worldView = player->WorldToCam();
        main_cam.SetSize(iWidth, iHeight, n, GH_FAR);
        main_cam.UseViewport();

        // Render scene
        GH_REC_LEVEL = GH_MAX_RECURSION;
        Render(main_cam, 0, nullptr);
        glfwSwapBuffers(window);
    }

    DestroyGLObjects();
    return 0;
}

void Engine::LoadScene(int ix)
{
    // Clear out old scene
    if (curScene)
    {
        curScene->Unload();
    }
    vObjects.clear();
    vPortals.clear();
    player->Reset();

    // Create new scene
    curScene = vScenes[ix];
    curScene->Load(vObjects, vPortals, *player);
    player->SetPosition(Vector3(0.0f, GH_PLAYER_HEIGHT, 0.0f));
    vObjects.push_back(player);
}

void Engine::Update()
{
    // Update
    for (size_t i = 0; i < vObjects.size(); ++i)
    {
        assert(vObjects[i].get());
        vObjects[i]->Update();
    }

    // Collisions
    // For each physics object
    for (size_t i = 0; i < vObjects.size(); ++i)
    {
        Physical* physical = vObjects[i]->AsPhysical();
        if (!physical)
        {
            continue;
        }
        Matrix4 worldToLocal = physical->WorldToLocal();

        // For each object to collide with
        for (size_t j = 0; j < vObjects.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            Object& obj = *vObjects[j];
            if (!obj.mesh)
            {
                continue;
            }

            // For each hit sphere
            for (size_t s = 0; s < physical->hitSpheres.size(); ++s)
            {
                // Brings point from collider's local coordinates to hits's
                // local coordinates.
                const Sphere& sphere = physical->hitSpheres[s];
                Matrix4       worldToUnit = sphere.LocalToUnit() * worldToLocal;
                Matrix4       localToUnit = worldToUnit * obj.LocalToWorld();
                Matrix4       unitToWorld = worldToUnit.Inverse();

                // For each collider
                for (size_t c = 0; c < obj.mesh->colliders.size(); ++c)
                {
                    Vector3         push;
                    const Collider& collider = obj.mesh->colliders[c];
                    if (collider.Collide(localToUnit, push))
                    {
                        // If push is too small, just ignore
                        push = unitToWorld.MulDirection(push);
                        vObjects[j]->OnHit(*physical, push);
                        physical->OnCollide(*vObjects[j], push);

                        worldToLocal = physical->WorldToLocal();
                        worldToUnit = sphere.LocalToUnit() * worldToLocal;
                        localToUnit = worldToUnit * obj.LocalToWorld();
                        unitToWorld = worldToUnit.Inverse();
                    }
                }
            }
        }
    }

    // Portals
    for (size_t i = 0; i < vObjects.size(); ++i)
    {
        Physical* physical = vObjects[i]->AsPhysical();
        if (physical)
        {
            for (size_t j = 0; j < vPortals.size(); ++j)
            {
                if (physical->TryPortal(*vPortals[j]))
                {
                    break;
                }
            }
        }
    }
}

void Engine::Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal)
{
    // Clear buffers
    if (GH_USE_SKY)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        sky->Draw(cam);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Create queries (if applicable)
    GLuint queries[GH_MAX_PORTALS];
    GLuint drawTest[GH_MAX_PORTALS];
    assert(vPortals.size() <= GH_MAX_PORTALS);
    if (occlusionCullingSupported)
    {
        glGenQueries((GLsizei) vPortals.size(), queries);
    }

    // Draw scene
    for (size_t i = 0; i < vObjects.size(); ++i)
    {
        vObjects[i]->Draw(cam, curFBO);
    }

    // Draw portals if possible
    if (GH_REC_LEVEL > 0)
    {
        // Draw portals
        GH_REC_LEVEL -= 1;
        if (occlusionCullingSupported && GH_REC_LEVEL > 0)
        {
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDepthMask(GL_FALSE);
            for (size_t i = 0; i < vPortals.size(); ++i)
            {
                if (vPortals[i].get() != skipPortal)
                {
                    glBeginQuery(GL_SAMPLES_PASSED, queries[i]);
                    vPortals[i]->DrawPink(cam);
                    glEndQuery(GL_SAMPLES_PASSED);
                }
            }
            for (size_t i = 0; i < vPortals.size(); ++i)
            {
                if (vPortals[i].get() != skipPortal)
                {
                    glGetQueryObjectuiv(queries[i], GL_QUERY_RESULT, &drawTest[i]);
                }
            };
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
            glDeleteQueries((GLsizei) vPortals.size(), queries);
        }
        for (size_t i = 0; i < vPortals.size(); ++i)
        {
            if (vPortals[i].get() != skipPortal)
            {
                if (occlusionCullingSupported && (GH_REC_LEVEL > 0) && (drawTest[i] == 0))
                {
                    continue;
                }
                else
                {
                    vPortals[i]->Draw(cam, curFBO);
                }
            }
        }
        GH_REC_LEVEL += 1;
    }

#if 0
  //Debug draw colliders
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->DebugDraw(cam);
  }
#endif
}

void Engine::CreateGLWindow()
{
    // Always start in windowed mode
    iWidth = GH_SCREEN_WIDTH;
    iHeight = GH_SCREEN_HEIGHT;

    if (!glfwInit())
    {
        throw std::runtime_error("Failed to init GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_MAXIMIZED, GH_START_FULLSCREEN ? GLFW_TRUE : GLFW_FALSE);

    window = glfwCreateWindow(iWidth, iHeight, GH_TITLE, nullptr, nullptr);
    if (window == nullptr)
    {
        throw std::runtime_error("Failed to create a window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (gladLoadGL() == 0)
    {
        throw std::runtime_error("Failed to load opengl");
    }
}

void Engine::InitGLObjects()
{
    // Basic global variables
    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // Check GL functionality
    glGetQueryiv(GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &occlusionCullingSupported);
}

void Engine::DestroyGLObjects()
{
    // curScene->Unload();
    vObjects.clear();
    vPortals.clear();
}

float Engine::NearestPortalDist() const
{
    float dist = FLT_MAX;
    for (size_t i = 0; i < vPortals.size(); ++i)
    {
        dist = GH_MIN(dist, vPortals[i]->DistTo(player->pos));
    }
    return dist;
}

void Engine::ToggleFullscreen()
{
    isFullscreen = !isFullscreen;
    if (isFullscreen)
    {
        // TODO
    }
    else
    {
        // TODO
    }
}
