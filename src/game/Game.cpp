#include "Game.h"
#include <cstdio>
#include <string>
#include <filesystem>

void Game::Init(SDL_Window* window, SDL_Renderer* render) {
    sdlWindow = window;
    renderer = render;
    shouldQuit = false;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "imgui.ini";
    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.PopupRounding = 4.0f;
    
    ImGui_ImplSDL3_InitForSDLRenderer(sdlWindow, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    
    std::string playerPath = GetResourcePath("Player.bmp");
    if (playerPath.empty()) {
        printf("Failed to resolve Player.bmp path\n");
    }
    
    SDL_Surface* playerSurface = SDL_LoadBMP(playerPath.c_str());
    playerTexture = nullptr;
    if (playerSurface) {
        playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
        SDL_DestroySurface(playerSurface);
        if (!playerTexture) {
            printf("Failed to create texture from Player.bmp: %s\n", SDL_GetError());
        }
    } else {
        printf("Failed to load Player.bmp: %s\n", SDL_GetError());
    }
    
    SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);
    
    showDemo = false;
    sliderValue = 0.0f;
    counter = 0;
    strcpy_s(inputText, sizeof(inputText), "Hello ImGui!");
    
    posX = 300.0f;
    posY = 200.0f;
    timer = 0.0f;
    currentFrame = 0;
}

std::string Game::GetResourcePath(const std::string& filename) {
    std::string resourcePath = "assets/" + filename;
    
    if (!std::filesystem::exists(resourcePath)) {
        printf("Warning: Resource not found at %s, trying current directory\n", resourcePath.c_str());
        resourcePath = filename;
        
        if (!std::filesystem::exists(resourcePath)) {
            printf("Error: Resource not found: %s\n", resourcePath.c_str());
            return "";
        }
    }
    
    return resourcePath;
}

void Game::Update(float dt) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            shouldQuit = true;
        }
    }
    
    timer += dt;
    if (timer > 0.1f) {
        currentFrame = (currentFrame + 1) % 6;
        timer = 0.0f;
    }
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);
    
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, displayHeight));
    ImGui::Begin("Left Sidebar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    
    if (ImGui::Button("Click Me!", ImVec2(-1, 40))) {
        printf("Button Clicked!\n");
        counter++;
    }
    
    ImGui::Text("Counter: %d", counter);
    
    ImGui::Separator();
    
    if (ImGui::Button("Toggle Demo")) {
        showDemo = !showDemo;
    }
    
    ImGui::SliderFloat("Float Value", &sliderValue, 0.0f, 1.0f);
    
    ImGui::InputText("Input Text", inputText, sizeof(inputText));
    
    if (playerTexture) {
        ImGui::Separator();
        ImGui::Text("Test Texture Load:");
        ImGui::Image((ImTextureID)playerTexture, ImVec2(128, 128));
    }
    
    ImGui::End();
    
    if (showDemo) {
        ImGui::ShowDemoWindow(&showDemo);
    }
    
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Game::Shutdown() {
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
    }
    
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
