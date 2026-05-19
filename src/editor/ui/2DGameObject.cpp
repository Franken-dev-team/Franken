#include "2DGameObject.h"
#include <cstdio>
#include <imgui/imgui.h>


void GameObject::Create(const CreateArgs& args) {
    if (args.path.empty()) {
        printf("Failed to resolve Player.bmp path\n");
        return;
    }

    SDL_Surface* surface = SDL_LoadBMP(args.path.c_str());
    if (!surface) {
        printf("Failed to load Player.bmp: %s\n", SDL_GetError());
        return;
    }

    SDL_Texture* texture = nullptr;
    texture = SDL_CreateTextureFromSurface(args.renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
        printf("Failed to create texture from Player.bmp: %s\n", SDL_GetError());
        return;
    }
    GameObject object = GameObject(
        (ImTextureID)texture,
        args.posX,
        args.posY,
        args.sizeX,
        args.sizeY,
        args.selected);

	gameObjects.push_back(std::move(object));
}

void GameObject::Render() {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for (int i = 0; i < gameObjects.size(); ++i) {
        ImGui::SetCursorScreenPos(ImVec2(gameObjects[i].posX, gameObjects[i].posY));
        ImGui::Image(GameObject::gameObjects[i].textureID, ImVec2(gameObjects[i].sizeX, gameObjects[i].sizeY));
        if (gameObjects[i].selected) {
            ImVec2 minPos = ImGui::GetItemRectMin();
            ImVec2 maxPos = ImGui::GetItemRectMax();
            draw_list->AddRectFilled(minPos, maxPos, IM_COL32(0, 0, 255, 128));
        };
    }
}

GameObject* GameObject::GetHoveredObject() {
    for (int i = 0; i < gameObjects.size(); ++i) {
        if (ImGui::IsMouseHoveringRect(ImVec2(gameObjects[i].posX, gameObjects[i].posY), ImVec2(gameObjects[i].posX + gameObjects[i].sizeX, gameObjects[i].posY + gameObjects[i].sizeY))) {
            return &gameObjects[i];
        }
    }
    return nullptr;
}

void GameObject::Update(int index) {
    gameObjects[index].posX += 1;
}

void GameObject::Duplicate(int index) {
    GameObject copy = gameObjects[index];
    copy.posX -= 10;
    copy.posY += 10;
    gameObjects.push_back(copy);
}

void GameObject::DuplicateSelected() {
    int count = gameObjects.size();
    for (int i = 0; i < count; ++i) {
        if (gameObjects[i].selected) {
            Duplicate(i);
        }
    }
}

void GameObject::toggleSelect() {
    this->selected = !this->selected;
}

bool GameObject::isSelected() const {
    return this->selected;
}

void GameObject::setSelected(bool selected) {
    this->selected = selected;
}

void GameObject::select() {
    this->selected = true;
}

void GameObject::deselect() {
    this->selected = false;
}

void GameObject::DeselectAll() {
    for (int i = 0; i < gameObjects.size(); ++i) {
        gameObjects[i].selected = false;
    }
}

void GameObject::SelectAll() {
    for (int i = 0; i < gameObjects.size(); ++i) {
        gameObjects[i].selected = true;
    }
}

void GameObject::Destroy(int index) {
    SDL_DestroyTexture(gameObjects[index].texture);
    gameObjects.erase(gameObjects.begin() + index);
}

void GameObject::DestroySelected() {
    for (int i = static_cast<int>(gameObjects.size()) - 1; i >= 0; --i) {
        if (gameObjects[i].selected) {
            Destroy(i);
        }
    }
}

void GameObject::DestroyAll() {
    for (int i = 0; i < gameObjects.size(); ++i) {
        Destroy(i);
    }
}
