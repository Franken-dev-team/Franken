#pragma once
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_sdlrenderer3.h>
#include <string>
#include <filesystem>

class Game {
private:
    SDL_Texture* playerTexture;
    float posX, posY;
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

public:
    void Init(SDL_Window* window, SDL_Renderer* render);
    void Update(float dt);
    void Render();
    void Shutdown();
    bool ShouldQuit() const { return shouldQuit; }

private:
    std::string GetResourcePath(const std::string& filename);
};
