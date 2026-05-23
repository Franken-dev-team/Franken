#pragma once
#include <imgui/imgui.h>

class Gizmo {
public:
    static void Update(int& posX, int& posY, ImVec2 center, float size = 100.0f);
    static void Render(float center_x, float center_y, float size = 100.0f);
    static bool IsActive();
    static bool IsHovered();   // true when mouse is over any gizmo axis
    static void Deactivate();

private:
    static void DrawArrows(ImDrawList* draw_list, ImVec2 screenCenter, float size);
    static void DrawCircle(ImDrawList* draw_list, ImVec2 center);
    static int activeAxis;
};
