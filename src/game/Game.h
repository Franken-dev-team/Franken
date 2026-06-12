#pragma once
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_sdlrenderer3.h>
#include <string>
#include <filesystem>
#include "../core/font/FontManager.h"

class Game {
private:
    float timer;
    int currentFrame;

    float buttonX, buttonY, buttonW, buttonH;
    SDL_Color buttonColor;
    bool isHovered;

    bool showDemo;
    float sliderValue;
    int counter;
    char inputText[256];
    SDL_Window* sdlWindow;
    SDL_Renderer* renderer;
    int displayWidth, displayHeight;
    bool shouldQuit;
    bool showProperties;
    bool showMainViewport;
    bool showAudioMixer;
    ImGuiID dockspace_id;
    bool dockspaceInitialized;

    float clickedTextureX;
    float clickedTextureY;
    bool textureClicked;

    bool isDragging;
    float dragOffsetX;
    float dragOffsetY;
    bool gizmoActive;

    int selectedIndex = -1; // Track by index in vector
    int selectedObjectID = -1; // Track by unique ID
    
    ImVec2 cameraOffset = {0.0f, 0.0f};
    bool isPanning = false;

    bool showInspector;
    void RenderInspectorWindow();

public:
    void Init(SDL_Window* window, SDL_Renderer* render);
    void Update(float dt);
    void Render();
    void Shutdown();
    bool ShouldQuit() const { return shouldQuit; }

private:
    std::string GetResourcePath(const std::string& filename);
    void SetupDockingLayout();
    void RenderMenuBar();
    void RenderPropertiesWindow();
    void RenderMainViewportWindow();
    void RenderAudioMixerWindow();

    std::string OpenFileDialog();
    std::string SaveFileDialog();
    
    float masterVol = 1.0f;
    float bgmVol = 0.8f;
    float sfxVol = 0.6f;
    float uiVol = 1.0f;
    
    ImVec2 lastMousePos = {0,0};
};
