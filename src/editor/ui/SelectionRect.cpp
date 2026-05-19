#include "SelectionRect.h"
#include <imgui/imgui.h>

void SelectionRect::Render(ImVec2 start, ImVec2 end)
{
    m_start = start;
    m_end = end;
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRect(m_start, m_end, IM_COL32(240, 240, 255, 125), 0.0f, 0, 1.0f);
    drawList->AddRectFilled(m_start, m_end, IM_COL32(240, 240, 255, 125));
}

void SelectionRect::SelectIntersectingGameObjects(std::vector<GameObject> gameObjects)
{
    float minX = std::min(m_start.x, m_end.x);
    float maxX = std::max(m_start.x, m_end.x);
    float minY = std::min(m_start.y, m_end.y);
    float maxY = std::max(m_start.y, m_end.y);

    for (GameObject& gameObject : GameObject::gameObjects)
    {
        if (gameObject.posX < maxX && gameObject.posX + gameObject.sizeX > minX &&
            gameObject.posY < maxY && gameObject.posY + gameObject.sizeY > minY)
        {
            gameObject.select();
        }
    }
}
