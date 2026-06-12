#include "2DGameObject.h"
#include <cstdio>
#include <fstream>
#include <imgui/imgui.h>

void GameObject::Create(const CreateArgs& args) {
    if (args.path.empty()) {
        printf("Empty path provided to Create\n");
        return;
    }

    SDL_Surface* surface = SDL_LoadBMP(args.path.c_str());
    if (!surface) {
        printf("Failed to load: %s, Error: %s\n", args.path.c_str(), SDL_GetError());
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(args.renderer, surface);
    SDL_DestroySurface(surface);
    if (!tex) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        return;
    }

    GameObject object = GameObject(
        (ImTextureID)tex,
        (int)gameObjects.size(),
        args.posX,
        args.posY,
        args.sizeX,
        args.sizeY,
        args.selected);
    
    object.texture = tex;
    object.texturePath = args.path;

	gameObjects.push_back(std::move(object));
}

void GameObject::Render(ImVec2 originPos, ImVec2 offset) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    for (auto& obj : gameObjects) {
        ImVec2 pos = ImVec2(originPos.x + obj.posX - offset.x, originPos.y + obj.posY - offset.y);
        ImGui::SetCursorScreenPos(pos);
        ImGui::Image(obj.textureID, ImVec2((float)obj.sizeX, (float)obj.sizeY));
        if (obj.selected) {
            ImVec2 minPos = pos;
            ImVec2 maxPos = ImVec2(pos.x + obj.sizeX, pos.y + obj.sizeY);
            draw_list->AddRect(minPos, maxPos, IM_COL32(0, 120, 255, 255), 0.0f, 0, 2.0f);
        }
    }
}

GameObject* GameObject::GetHoveredObject(ImVec2 originPos, ImVec2 offset) {
    ImVec2 mousePos = ImGui::GetMousePos();
    for (auto& obj : gameObjects) {
        ImVec2 pos = ImVec2(originPos.x + obj.posX - offset.x, originPos.y + obj.posY - offset.y);
        if (mousePos.x >= pos.x && mousePos.x <= pos.x + obj.sizeX &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + obj.sizeY) {
            return &obj;
        }
    }
    return nullptr;
}

void GameObject::Update(int index) {
    // Custom update logic here
}

void GameObject::Duplicate(int index) {
    GameObject copy = gameObjects[index];
    copy.id = (int)gameObjects.size();
    copy.posX += 20;
    copy.posY += 20;
    gameObjects.push_back(copy);
}

void GameObject::DuplicateSelected() {
    int count = (int)gameObjects.size();
    for (int i = 0; i < count; ++i) {
        if (gameObjects[i].selected) {
            Duplicate(i);
        }
    }
}

void GameObject::toggleSelect() { this->selected = !this->selected; }
bool GameObject::isSelected() const { return this->selected; }
void GameObject::setSelected(bool selected) { this->selected = selected; }
void GameObject::select() { this->selected = true; }
void GameObject::deselect() { this->selected = false; }
void GameObject::duplicate() { Duplicate(this->id); }
void GameObject::destroy() { Destroy(this->id); }

void GameObject::DeselectAll() {
    for (auto& obj : gameObjects) obj.selected = false;
}

void GameObject::SelectAll() {
    for (auto& obj : gameObjects) obj.selected = true;
}

void GameObject::Destroy(int index) {
    if (index >= 0 && index < gameObjects.size()) {
        if (gameObjects[index].texture) SDL_DestroyTexture(gameObjects[index].texture);
        gameObjects.erase(gameObjects.begin() + index);
        // Re-index remaining objects
        for(int i=0; i<gameObjects.size(); ++i) gameObjects[i].id = i;
    }
}

void GameObject::DestroySelected() {
    for (int i = (int)gameObjects.size() - 1; i >= 0; --i) {
        if (gameObjects[i].selected) Destroy(i);
    }
}

void GameObject::DestroyAll() {
    for (auto& obj : gameObjects) {
        if (obj.texture) SDL_DestroyTexture(obj.texture);
    }
    gameObjects.clear();
}

json GameObject::ToJson() const {
    return json{
        {"id", id},
        {"posX", posX},
        {"posY", posY},
        {"sizeX", sizeX},
        {"sizeY", sizeY},
        {"texturePath", texturePath}
    };
}

void GameObject::FromJson(const json& j, SDL_Renderer* renderer) {
    CreateArgs args;
    args.renderer = renderer;
    args.path = j.at("texturePath").get<std::string>();
    args.posX = j.at("posX").get<int>();
    args.posY = j.at("posY").get<int>();
    args.sizeX = j.at("sizeX").get<int>();
    args.sizeY = j.at("sizeY").get<int>();
    args.selected = false;
    Create(args);
}

void GameObject::SaveProject(const std::string& path) {
    json j = json::array();
    for (const auto& obj : gameObjects) {
        j.push_back(obj.ToJson());
    }
    std::ofstream file(path);
    if (file.is_open()) {
        file << j.dump(4);
        printf("Project saved to %s\n", path.c_str());
    }
}

void GameObject::LoadProject(const std::string& path, SDL_Renderer* renderer) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    
    json j;
    file >> j;
    
    DestroyAll();
    for (const auto& item : j) {
        FromJson(item, renderer);
    }
    printf("Project loaded from %s\n", path.c_str());
}
