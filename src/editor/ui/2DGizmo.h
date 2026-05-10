#pragma once
#include <imgui/imgui.h>

class Gizmo {
	public:
		static void Update(float& posX, float& posY, ImVec2 center, float size = 100.0f);
		static void Render(int center_x, int center_y);
		static bool IsActive();
		static void Deactivate();

	private:
		static void drawCircle(ImDrawList* draw_list, ImVec2 center);
		static void drawArrow(ImDrawList* draw_list, ImVec2 center);
		static int activeAxis;
};
