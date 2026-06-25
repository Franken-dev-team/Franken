#include "Game.h"
#include "../core/audio/AudioManager.h"
#include "../core/font/FontManager.h"
#include "../core/filesystem/ConfigManager.h"
#include "../editor/ui/UITheme.h"
#include "../editor/ui/2DGizmo.h"
#include "../editor/ui/2DGameObject.h"
#include "../editor/ui/SelectionRect.h"
#include "../editor/ui/PopUpMenu.h"
#include "../editor/ui/FileExplorer.h"
#include <SDL3/SDL_scancode.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/imgui.h>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>

GameObject* draggedObject = nullptr;
GameObject* selectedObject = nullptr;
ImVec2 selectionRectStart = ImVec2(0.0f, 0.0f);
GameObject* hoveredObject = nullptr;

char title[256] = "";
char path[256] = "";

void Game::Init(SDL_Window* window, SDL_Renderer* render) {
    sdlWindow = window;
    renderer = render;
    shouldQuit = false;
    ConfigManager::Init();
    GameObject::Create({render, GetResourcePath("Player.bmp"), 100, 100, 64, 64, false});

    if (!AudioManager::GetInstance().Init()) {
        printf("Failed to initialize AudioManager\n");
    } else {
        printf("AudioManager initialized successfully\n");
    }

    if (!FontManager::GetInstance().Init()) {
        printf("Failed to initialize FontManager\n");
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

    SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);

    showDemo = false;
    sliderValue = 0.0f;
    counter = 0;
    #ifdef _WIN32
    strcpy_s(inputText, sizeof(inputText), "Hello ImGui!");
    #else
    strcpy(inputText, "Hello ImGui!");
    #endif

    timer = 0.0f;
    currentFrame = 0;
    showProperties = false;
    showMainViewport = false;
    showAudioMixer = false;
    showProjectManager = true;
    showProjectCreation = false;
    showFileExplorer = true;
    dockspace_id = 0;
    dockspaceInitialized = false;

    clickedTextureX = 0.0f;
    clickedTextureY = 0.0f;
    textureClicked = false;

    isDragging = false;
    dragOffsetX = 0.0f;
    dragOffsetY = 0.0f;
    gizmoActive = false;
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
    if (showProjectManager) {
        RenderProjectManagerWindow();
    }

    if (showProjectCreation) {
        RenderProjectCreationWindow();
    }

    if (showFileExplorer) {
        RenderFileExplorer();
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
            if (ImGui::MenuItem("Projects")) {
                showProjectManager = true;
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
    ImGui::Text("Image Position");
    ImGui::Separator();
    // ImGui::Text("Current Position:");
    // ImGui::Text("X: %.2f", posX);
    // ImGui::Text("Y: %.2f", posY);
    ImGui::Text("current workspace: %s", currentWorkspace.c_str());

    if (isDragging) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Dragging");
    } else {
        ImGui::Text("Status: Idle");
    }

    ImGui::Separator();
    ImGui::Text("Texture Click Coordinates");
    ImGui::Separator();

    if (textureClicked) {
        ImGui::Text("Clicked Position:");
        ImGui::Text("X: %.2f", clickedTextureX);
        ImGui::Text("Y: %.2f", clickedTextureY);
    } else {
        ImGui::Text("No texture clicked yet");
        ImGui::Text("Click on texture in Main Viewport");
    }

    ImGui::End();
}

void Game::RenderMainViewportWindow() {
    ImGui::Begin("Main Viewport", &showMainViewport);

    ImGui::Text("Main Game Viewport");
    ImGui::Text("Resolution: %dx%d", displayWidth, displayHeight);
    GameObject::Render();

    ImVec2 cursorPos = ImGui::GetCursorPos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 textureScreenPos = ImVec2(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y);

    if (!ImGui::IsPopupOpen("menuObj")) {
        hoveredObject = GameObject::GetHoveredObject();
    }

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_MouseLeft) && hoveredObject != nullptr) {
        hoveredObject->toggleSelect();
    }

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_A)) {
        GameObject::SelectAll();
    }

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V)) {
        GameObject::DuplicateSelected();
    }

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D)) {
        if (selectedObject != nullptr) {
            selectedObject = nullptr;
            Gizmo::Deactivate();
        }
        GameObject::DestroySelected();
    }

    if (hoveredObject == nullptr && ImGui::IsMouseClicked(0) && !Gizmo::IsActive()) {
        if (!ImGui::IsPopupOpen("menu")) {
            // Only deselect if we didn't just click a gizmo arrow
            if (!Gizmo::IsHovered()) {
                GameObject::DeselectAll();
                selectedObject = nullptr;
            }
        }
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
        ImGui::OpenPopup("menu");
    }
    if (hoveredObject != nullptr) {
        PopUpMenu::Draw("menu",
            PopUpMenu::PopUpItem{"Select", "", [&]() {hoveredObject->select(); }},
            PopUpMenu::PopUpItem{"Deselect", "", [&]() {hoveredObject->deselect(); }},
            PopUpMenu::PopUpItem{"Duplicate", "", [&]() { hoveredObject->duplicate(); }},
            PopUpMenu::PopUpItem{"Delete", "", [&]() {
                if (selectedObject == hoveredObject) { selectedObject = nullptr; Gizmo::Deactivate(); }
                hoveredObject->destroy();
            }}
        );
    } else {
        PopUpMenu::Draw("menu",
            PopUpMenu::PopUpItem{"New Object", "ctrl+a", [&]() { GameObject::Create({Game::renderer, GetResourcePath("Player.bmp"), int(ImGui::GetMousePos().x), int(ImGui::GetMousePos().y), 64, 64, false}); }},
            PopUpMenu::PopUpItem{"Select All", "ctrl+a", [&]() { GameObject::SelectAll(); }},
            PopUpMenu::PopUpItem{"Delete All", "", [&]() { GameObject::DestroyAll(); }},
            PopUpMenu::PopUpItem{"Duplicate Selected", "ctrl+v", [&]() { GameObject::DuplicateSelected(); }},
            PopUpMenu::PopUpItem{"Delete Selected", "ctrl+d", [&]() { GameObject::DestroySelected(); }}
        );
    }

    if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsMouseDragging(0)) {
        if (selectionRectStart.x == 0.0f && selectionRectStart.y == 0.0f) {
            selectionRectStart = ImGui::GetMousePos();
        }
        SelectionRect::Render(selectionRectStart, ImGui::GetMousePos());
    }

    if (hoveredObject != nullptr && ImGui::IsMouseClicked(0) && !Gizmo::IsActive()) {
        ImVec2 mousePos = ImGui::GetMousePos();

        selectedObject = hoveredObject;   // <-- persist selection
        draggedObject  = hoveredObject;

        float maxTexX = (float)draggedObject->sizeX - 1.0f;
        float maxTexY = (float)draggedObject->sizeY - 1.0f;

        clickedTextureX = (maxTexX >= 0.0f) ? std::clamp(mousePos.x - textureScreenPos.x, 0.0f, maxTexX) : 0.0f;
        clickedTextureY = (maxTexY >= 0.0f) ? std::clamp(mousePos.y - textureScreenPos.y, 0.0f, maxTexY) : 0.0f;
        textureClicked = true;

        dragOffsetX = mousePos.x - draggedObject->posX;
        dragOffsetY = mousePos.y - draggedObject->posY;
        isDragging = true;
    }

    if (isDragging && draggedObject != nullptr && ImGui::IsMouseDragging(0) && !Gizmo::IsActive()) {
        ImVec2 mousePos = ImGui::GetMousePos();

        float maxPosX = displayWidth - (float)draggedObject->sizeX;
        float maxPosY = displayHeight - (float)draggedObject->sizeY;

        draggedObject->posX = (maxPosX >= 0.0f) ? std::clamp(mousePos.x - dragOffsetX, 0.0f, maxPosX) : 0.0f;
        draggedObject->posY = (maxPosY >= 0.0f) ? std::clamp(mousePos.y - dragOffsetY, 0.0f, maxPosY) : 0.0f;
    }

    if (!ImGui::IsMouseDown(0)) {
        isDragging = false;
        draggedObject = nullptr;
        if (selectionRectStart.x != 0.0f && selectionRectStart.y != 0.0f) {
            SelectionRect::SelectIntersectingGameObjects(GameObject::gameObjects);
        }
        selectionRectStart = ImVec2(0.0f, 0.0f);
    }

	if (selectedObject != nullptr) {
		float gcx = (float)selectedObject->posX + (float)selectedObject->sizeX / 2.0f;
		float gcy = (float)selectedObject->posY + (float)selectedObject->sizeY / 2.0f;
		Gizmo::Update(selectedObject->posX, selectedObject->posY, ImVec2(gcx, gcy));
		Gizmo::Render(gcx, gcy);
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

void Game::RenderProjectManagerWindow() {
    ImGui::Begin("Project Manager", &showProjectManager);
    if (ImGui::Button("New Project", ImVec2(100, 25))) {
        showProjectCreation = true;
    }
    if(ImGui::BeginTable("projects grid", 4)) {
        const toml::array* projects = ConfigManager::GetArray("projects");
        if (!projects) {
            ImGui::EndTable();
            return;
        }

        for (int i = 0; i < projects->size(); i++)
           {
                ImGui::TableNextColumn();
                if (auto* project = (*projects)[i].as_table()) {
                    if (auto title_node = project->get("title")) {
                        std::string title_str = title_node->value_or(std::string("Untitled"));
                        auto* path_node = project->get("path");
                        std::string workspace_str = path_node ? path_node->value_or(std::string("Untitled")) : std::string("Untitled");
                        ImGui::PushID(i);
                        if (ImGui::Button(title_str.c_str(), ImVec2(100, 25))) {
                            if (!workspace_str.empty() && std::filesystem::exists(workspace_str)) {
                                showProjectManager = false;
                                currentWorkspace = workspace_str;
                                FileExplorer::SetRootPath(workspace_str);
                                FileExplorer::SetPreviousPath(workspace_str);
                                showMainViewport = true;
                            } else {
                                showProjectNotFound = true;
                            }
                        }
                        ImGui::PopID();
                    } else {
                        ImGui::Text("Untitled");
                    }
                }
           }
           ImGui::EndTable();
    }
        if (showProjectNotFound) {
            ImGui::OpenPopup("Project Not Found");
        }

        if (ImGui::BeginPopupModal("Project Not Found", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Error: Project not found");
            if (ImGui::Button("OK", ImVec2(100, 25))) {
                showProjectNotFound = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    ImGui::EndChild();
}

void Game::RenderProjectCreationWindow() {
    ImGui::Begin("New Project", &showProjectCreation);
    ImGui::InputText("title", title, IM_ARRAYSIZE(title));
    ImGui::InputText("path", path, IM_ARRAYSIZE(path));

    if (ImGui::Button("Create", ImVec2(100, 25))) {
        ConfigManager::Set("path", path);
        ConfigManager::Set("title", title);
        ConfigManager::Save("projects");
        showProjectCreation = false;
        #ifdef _WIN32
        strcpy_s(title, IM_ARRAYSIZE(title), "");
        strcpy_s(path, IM_ARRAYSIZE(path), "");
        #else
        strcpy(title, "");
        strcpy(path, "");
        #endif
    }

    ImGui::End();
}

void Game::RenderFileExplorer() {
    ImGui::Begin("File Explorer");
    FileExplorer::Render();
    ImGui::End();
}

void Game::Shutdown() {
    GameObject::DestroyAll();

    AudioManager::GetInstance().Shutdown();
    FontManager::GetInstance().Shutdown();

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
