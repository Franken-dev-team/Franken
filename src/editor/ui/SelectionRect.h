#pragma once
#include <imgui/imgui.h>
#include "2DGameObject.h"

class SelectionRect {
    private:
        inline static ImVec2 m_start;
        inline static ImVec2 m_end;

    public:
        static void Render(ImVec2 start, ImVec2 end);
        static void SelectIntersectingGameObjects(std::vector<GameObject> gameObjects);
};
