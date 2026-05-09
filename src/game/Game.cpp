#include "Game.h"
#include "../core/audio/AudioManager.h"
#include "../editor/ui/AudioPanel.h"
#include "../editor/ui/UITheme.h"
#include <cstdio>
#include <string>
#include <filesystem>

void Game::Init(SDL_Window* window, SDL_Renderer* render) {
    sdlWindow = window;
    renderer = render;
    shouldQuit = false;
    
    if (!AudioManager::GetInstance().Init()) {
        printf("Failed to initialize AudioManager\n");
    } else {
        printf("AudioManager initialized successfully\n");
    }
    
    std::string musicPath = GetResourcePath("bg/bg.mp3");
    if (!musicPath.empty()) {
        printf("Found music file at: %s\n", musicPath.c_str());
        AudioManager::GetInstance().PlayMusic(musicPath, true);
    } else {
        printf("Error: Background music file not found at assets/bg/bg.mp3\n");
    }
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "imgui.ini";
    
    SetupProfessionalTheme();
    
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
    showProperties = true;
    showMainViewport = true;
    showAudioMixer = true;
    dockspace_id = 0;
    dockspaceInitialized = false;
    
    clickedTextureX = 0.0f;
    clickedTextureY = 0.0f;
    textureClicked = false;
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
    SDL_SetRenderDrawColor(renderer, 13, 13, 13, 255);
    SDL_RenderClear(renderer);
    
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
    RenderMenuBar();
    
    ImGui::End();
    if (!dockspaceInitialized) {
        SetupDockingLayout();
        dockspaceInitialized = true;
    }
    
    if (showProperties) {
        RenderPropertiesWindow();
    }
    if (showMainViewport) {
        RenderMainViewportWindow();
    }
    if (showAudioMixer) {
        RenderAudioMixerWindow();
    }
    
    if (showDemo) {
        ImGui::ShowDemoWindow(&showDemo);
    }
    
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Game::SetupDockingLayout() {
}

void Game::RenderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                shouldQuit = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Toggle Demo")) {
                showDemo = !showDemo;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Properties", nullptr, showProperties)) {
                showProperties = !showProperties;
            }
            if (ImGui::MenuItem("Main Viewport", nullptr, showMainViewport)) {
                showMainViewport = !showMainViewport;
            }
            if (ImGui::MenuItem("Audio Mixer", nullptr, showAudioMixer)) {
                showAudioMixer = !showAudioMixer;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Layout")) {
            if (ImGui::MenuItem("Reset Layout")) {
                dockspaceInitialized = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void Game::RenderPropertiesWindow() {
    ImGui::Begin("Properties", &showProperties);
    
    if (ImGui::Button("Click Me!", ImVec2(-1, 40))) {
        printf("Button Clicked!\n");
        counter++;
    }
    
    ImGui::Text("Counter: %d", counter);
    
    ImGui::Separator();
    
    ImGui::SliderFloat("Float Value", &sliderValue, 0.0f, 1.0f);
    
    ImGui::InputText("Input Text", inputText, sizeof(inputText));
    
    ImGui::Separator();
    ImGui::Text("Texture Click Coordinates");
    ImGui::Separator();
    
    if (textureClicked) {
        ImGui::Text("Clicked Position:");
        ImGui::Text("X: %.2f", clickedTextureX);
        ImGui::Text("Y: %.2f", clickedTextureY);
    } else {
        ImGui::Text("No texture clicked yet");
        ImGui::Text("Click on the texture in Main Viewport");
    }
    
    ImGui::End();
}

void Game::RenderMainViewportWindow() {
    ImGui::Begin("Main Viewport", &showMainViewport);
    
    ImGui::Text("Main Game Viewport");
    ImGui::Text("Resolution: %dx%d", displayWidth, displayHeight);
    
    if (playerTexture) {
        ImGui::SetCursorPos(ImVec2(posX, posY));
        
        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 textureSize = ImVec2(64, 64);
        ImVec2 textureScreenPos = ImVec2(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y);
        
        ImGui::Image((ImTextureID)playerTexture, textureSize);
        
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            clickedTextureX = mousePos.x - textureScreenPos.x;
            clickedTextureY = mousePos.y - textureScreenPos.y;
            textureClicked = true;
            
            if (clickedTextureX < 0) clickedTextureX = 0;
            if (clickedTextureY < 0) clickedTextureY = 0;
            if (clickedTextureX >= textureSize.x) clickedTextureX = textureSize.x - 1;
            if (clickedTextureY >= textureSize.y) clickedTextureY = textureSize.y - 1;
        }
    } else {
        ImGui::Text("Player texture not loaded");
    }
    
    ImGui::End();
}

void Game::RenderAudioMixerWindow() {
    ImGui::Begin("Audio Mixer", &showAudioMixer);
    
    AudioManager& audio = AudioManager::GetInstance();
    
    ImGui::Text("Audio Status");
    ImGui::Separator();
    
    if (audio.IsMusicPlaying()) {
        ImGui::Text("Music: Playing");
    } else {
        ImGui::Text("Music: Stopped");
    }
    
    ImGui::Text("Current: assets/bg/bg.mp3");
    
    ImGui::Spacing();
    ImGui::Text("Controls");
    ImGui::Separator();
    
    if (audio.IsMusicPlaying()) {
        if (ImGui::Button("Pause", ImVec2(80, 30))) {
            audio.PauseMusic();
        }
    } else {
        if (ImGui::Button("Play", ImVec2(80, 30))) {
            audio.ResumeMusic();
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Stop", ImVec2(80, 30))) {
        audio.StopMusic();
    }
    
    ImGui::Spacing();
    ImGui::Text("Volume");
    ImGui::Separator();
    
    static float volume = audio.GetMasterVolume();
    if (ImGui::SliderFloat("Master Volume", &volume, 0.0f, 1.0f)) {
        audio.SetMasterVolume(volume);
    }
    
    static bool isMuted = audio.IsMuted();
    if (ImGui::Checkbox("Mute", &isMuted)) {
        audio.SetMuted(isMuted);
    }
    
    ImGui::End();
}

void Game::Shutdown() {
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
    }
    
    AudioManager::GetInstance().Shutdown();
    
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
