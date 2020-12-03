#pragma once

#include <glad/glad.h>

#include "Camera.h"
#include "GameHeader.h"
#include "Input.h"
#include "Object.h"
#include "Player.h"
#include "Portal.h"
#include "RoomGraph.h"
#include "Scene.h"
#include "Sky.h"
#include "Timer.h"

#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

class Engine
{
    friend class Input;

public:
    Engine(const char* filename);
    ~Engine();

    int  Run();
    void Update();
    void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
    void LoadScene(int ix);

    const Player& GetPlayer() const { return *player; }
    float         NearestPortalDist() const;

private:
    void CreateGLWindow();
    void InitGLObjects();
    void DestroyGLObjects();
    void ToggleFullscreen();

    GLFWwindow* window;

    int  iWidth;       // window width
    int  iHeight;      // window height
    bool isFullscreen; // fullscreen state

    Camera main_cam;
    Input  input;
    Timer  timer;

    std::vector<std::shared_ptr<Object>> vObjects;
    std::vector<std::shared_ptr<Portal>> vPortals;
    std::shared_ptr<Sky>                 sky;
    std::shared_ptr<Player>              player;

    GLint occlusionCullingSupported;

    std::vector<std::shared_ptr<Scene>> vScenes;
    std::shared_ptr<Scene>              curScene;
};
