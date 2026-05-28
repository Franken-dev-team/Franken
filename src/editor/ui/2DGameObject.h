#pragma once
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <string>
#include <vector>

class GameObject {
    private:
        bool selected;
        GameObject(ImTextureID textureID, int id, int posX, int posY, int sizeX, int sizeY, bool selected)
  		: posX(posX), posY(posY), sizeX(sizeX), sizeY(sizeY), textureID(textureID), selected(selected) {}

    public:
        struct CreateArgs {
            SDL_Renderer* renderer = nullptr;
            std::string path = "";
            int posX = 0;
            int posY = 0;
            int sizeX = 100;
            int sizeY = 100;
            bool selected = false;
        };

        SDL_Texture* texture;
       	ImTextureID textureID;
        int id;
        int posX;
        int posY;
        int sizeX;
        int sizeY;

        void toggleSelect();
        bool isSelected() const;
        void setSelected(bool selected);
        void select();
        void deselect();
        void duplicate();
        void destroy();

        static inline std::vector<GameObject> gameObjects;
        static void Create(const CreateArgs& args);
        static void Render();
        static GameObject* GetHoveredObject();
        static void Update(int index);
        static void Duplicate(int index);
        static void DuplicateSelected();
        static void SelectAll();
        static void DeselectAll();
        static void Destroy(int index);
        static void DestroySelected();
        static void DestroyAll();
};
