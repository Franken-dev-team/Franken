#include "Game.h"
#include "../core/audio/AudioManager.h"
#include "../core/font/FontManager.h"
#include "../editor/ui/AudioPanel.h"
#include "../editor/ui/UITheme.h"
#include "../editor/ui/2DGizmo.h"
#include "../editor/ui/2DGameObject.h"
#include "../editor/ui/SelectionRect.h"
#include "../editor/ui/PopUpMenu.h"
#include <SDL3/SDL_scancode.h>
#include <cstdio>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <commdlg.h>
#endif

GameObject* draggedObject = nullptr;
int selectedIndex = -1;
ImVec2 selectionRectStart = ImVec2(0.0f, 0.0f);
GameObject* hoveredObject = nullptr;

void Game::Init(SDL_Window* window, SDL_Renderer* render) {
    sdlWindow = window;
    renderer = render;
    shouldQuit = false;
    showInspector = true;
    showProperties = true;
    showMainViewport = true;
    showAudioMixer = true;
    showDemo = false;
    dockspaceInitialized = false;

    // Create initial object
    GameObject::Create({render, GetResourcePath("Player.bmp"), 100, 100, 64, 64, false});

    AudioManager::GetInstance().Init();
    FontManager::GetInstance().Init();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "imgui.ini";

    SetupProfessionalTheme();

    ImGui_ImplSDL3_InitForSDLRenderer(sdlWindow, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);
}

std::string Game::GetResourcePath(const std::string& filename) {
    std::vector<std::string> searchPaths = { "assets/" + filename, filename, "../assets/" + filename };
    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path)) return path;
    }
    return "assets/" + filename; // Fallback
}

void Game::Update(float dt) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT) shouldQuit = true;
    }
}

void Game::SetupDockingLayout() {
    ImGuiID ds_id = ImGui::GetID("FrankenDockSpace");
    ImGui::DockBuilderRemoveNode(ds_id);
    ImGui::DockBuilderAddNode(ds_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(ds_id, ImGui::GetMainViewport()->Size);

    ImGuiID dock_main = ds_id;
    ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.20f, NULL, &dock_main);
    ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.25f, NULL, &dock_main);
    ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.20f, NULL, &dock_main);

    ImGui::DockBuilderDockWindow("Main Viewport", dock_main);
    ImGui::DockBuilderDockWindow("Properties", dock_left);
    ImGui::DockBuilderDockWindow("Inspector", dock_right);
    ImGui::DockBuilderDockWindow("Audio Mixer", dock_bottom);
    ImGui::DockBuilderFinish(ds_id);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderClear(renderer);

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("FrankenDockHost", nullptr, flags);
    ImGui::PopStyleVar();

    ImGuiID ds_id = ImGui::GetID("FrankenDockSpace");
    ImGui::DockSpace(ds_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    if (!dockspaceInitialized) {
        SetupDockingLayout();
        dockspaceInitialized = true;
    }

    RenderMenuBar();
    ImGui::End();

    if (showProperties) RenderPropertiesWindow();
    if (showMainViewport) RenderMainViewportWindow();
    if (showAudioMixer) RenderAudioMixerWindow();
    if (showInspector) RenderInspectorWindow();
    if (showDemo) ImGui::ShowDemoWindow(&showDemo);

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);

    lastMousePos = ImGui::GetMousePos();
}

void Game::RenderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project")) { GameObject::DestroyAll(); selectedIndex = -1; }
            if (ImGui::MenuItem("Open Project (.frp)")) {
                std::string path = OpenFileDialog();
                if (!path.empty()) GameObject::LoadProject(path, renderer);
            }
            if (ImGui::MenuItem("Save Project")) {
                std::string path = SaveFileDialog();
                if (!path.empty()) GameObject::SaveProject(path);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) shouldQuit = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Main Viewport", nullptr, &showMainViewport);
            ImGui::MenuItem("Properties", nullptr, &showProperties);
            ImGui::MenuItem("Inspector", nullptr, &showInspector);
            ImGui::MenuItem("Audio Mixer", nullptr, &showAudioMixer);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo", nullptr, &showDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void Game::RenderPropertiesWindow() {
    ImGui::Begin("Properties", &showProperties);
    if (selectedIndex == -1 || selectedIndex >= GameObject::gameObjects.size()) {
        ImGui::TextDisabled("No active selection");
    } else {
        GameObject& sel = GameObject::gameObjects[selectedIndex];
        ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Object ID: %d", sel.id);
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Sprite Data", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("File: %s", sel.texturePath.c_str());
        }
    }
    ImGui::End();
}

void Game::RenderMainViewportWindow() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    bool open = true;
    ImGui::Begin("Main Viewport", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();

    ImVec2 viewportPos = ImGui::GetCursorScreenPos();
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::GetWindowDrawList()->AddRectFilled(viewportPos, ImVec2(viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y), IM_COL32(25, 25, 25, 255));

    GameObject::Render(viewportPos, cameraOffset);

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        cameraOffset.x -= delta.x;
        cameraOffset.y -= delta.y;
    }

    ImGui::Dummy(viewportSize);
    if (!ImGui::IsPopupOpen("viewport_context")) hoveredObject = GameObject::GetHoveredObject(viewportPos, cameraOffset);

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) ImGui::OpenPopup("viewport_context");

    if (ImGui::BeginPopup("viewport_context")) {
        if (hoveredObject) {
            if (ImGui::MenuItem("Duplicate")) { hoveredObject->duplicate(); selectedIndex = (int)GameObject::gameObjects.size()-1; }
            if (ImGui::MenuItem("Delete")) { hoveredObject->destroy(); selectedIndex = -1; Gizmo::Deactivate(); }
        } else {
            if (ImGui::MenuItem("Add New GameObject")) {
                GameObject::Create({renderer, GetResourcePath("Player.bmp"), (int)cameraOffset.x + 50, (int)cameraOffset.y + 50, 64, 64, false});
                GameObject::DeselectAll();
                selectedIndex = (int)GameObject::gameObjects.size()-1;
                GameObject::gameObjects[selectedIndex].select();
            }
        }
        ImGui::EndPopup();
    }

    if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !Gizmo::IsActive()) {
        if (!Gizmo::IsHovered()) {
            if (hoveredObject) {
                if (!ImGui::GetIO().KeyCtrl) GameObject::DeselectAll();
                for(int i=0; i<GameObject::gameObjects.size(); i++) if(&GameObject::gameObjects[i] == hoveredObject) { selectedIndex = i; break; }
                GameObject::gameObjects[selectedIndex].select();
                isDragging = true;
            } else {
                GameObject::DeselectAll();
                selectedIndex = -1;
            }
        }
    }

    if (isDragging && selectedIndex != -1 && ImGui::IsMouseDragging(0) && !Gizmo::IsActive()) {
        ImVec2 delta = { ImGui::GetMousePos().x - lastMousePos.x, ImGui::GetMousePos().y - lastMousePos.y };
        for (auto& obj : GameObject::gameObjects) if (obj.isSelected()) { obj.posX += (int)delta.x; obj.posY += (int)delta.y; }
    }
    if (!ImGui::IsMouseDown(0)) isDragging = false;

    if (selectedIndex != -1 && selectedIndex < GameObject::gameObjects.size()) {
        GameObject& sel = GameObject::gameObjects[selectedIndex];
        float gcx = viewportPos.x + sel.posX + sel.sizeX/2.0f - cameraOffset.x;
        float gcy = viewportPos.y + sel.posY + sel.sizeY/2.0f - cameraOffset.y;
        Gizmo::Update(sel.posX, sel.posY, ImVec2(gcx, gcy));
        Gizmo::Render(gcx, gcy);
    }
    ImGui::End();
}

void Game::RenderAudioMixerWindow() {
    ImGui::Begin("Audio Mixer", &showAudioMixer);
    ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.9f, 1.0f), "MASTER CONSOLE");
    ImGui::Separator();
    float* sl[] = { &masterVol, &bgmVol, &sfxVol, &uiVol };
    const char* lb[] = { "Master", "BGM", "SFX", "UI" };
    ImGui::Columns(4, "mixer_cols", false);
    for (int i = 0; i < 4; i++) {
        ImGui::PushID(i);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - 35) / 2);
        if (ImGui::VSliderFloat("##f", ImVec2(35, 140), sl[i], 0.0f, 1.0f, "")) {
            if (i == 0) AudioManager::GetInstance().SetMasterVolume(*sl[i]);
            if (i == 1) AudioManager::GetInstance().SetBGMVolume(*sl[i]);
            if (i == 2) AudioManager::GetInstance().SetSFXVolume(*sl[i]);
        }
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize(lb[i]).x) / 2);
        ImGui::Text("%s", lb[i]);
        ImGui::PopID();
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::End();
}

void Game::RenderInspectorWindow() {
    ImGui::Begin("Inspector", &showInspector);
    if (selectedIndex != -1 && selectedIndex < GameObject::gameObjects.size()) {
        GameObject& sel = GameObject::gameObjects[selectedIndex];
        ImGui::Text("Entity: Sprite_%d", sel.id);
        ImGui::DragInt("X", &sel.posX);
        ImGui::DragInt("Y", &sel.posY);
        ImGui::DragInt("W", &sel.sizeX);
        ImGui::DragInt("H", &sel.sizeY);
    } else {
        ImGui::TextDisabled("Select an entity to inspect");
    }
    ImGui::End();
}

std::string Game::OpenFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn; char szF[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn)); ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    ofn.lpstrFile = szF; ofn.nMaxFile = sizeof(szF);
    ofn.lpstrFilter = "Franken Project (*.frp)\0*.frp\0"; ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn)) return std::string(szF);
#endif
    return "";
}

std::string Game::SaveFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn; char szF[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn)); ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    ofn.lpstrFile = szF; ofn.nMaxFile = sizeof(szF);
    ofn.lpstrFilter = "Franken Project (*.frp)\0*.frp\0"; ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    if (GetSaveFileNameA(&ofn)) { std::string res = szF; if (res.find(".frp") == std::string::npos) res += ".frp"; return res; }
#endif
    return "";
}

void Game::Shutdown() { GameObject::DestroyAll(); AudioManager::GetInstance().Shutdown(); }
