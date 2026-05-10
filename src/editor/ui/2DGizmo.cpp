#include "2DGizmo.h"
#include <imgui/imgui.h>

void Gizmo::Render(int center_x, int center_y) {
	drawArrow(center_x, center_y);
	drawCircle(center_x, center_y);
}

void Gizmo::drawCircle(int center_x, int center_y) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddCircleFilled(ImVec2(center_x, center_y), 70.0f, IM_COL32(255, 255, 255, 200));
	ImGui::Dummy(ImVec2(center_x, center_y));
}

void Gizmo::drawArrow(int center_x, int center_y) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 arrow_x_p1 = ImVec2(center_x, center_y);
	ImVec2 arrow_x_p2 = ImVec2(center_x + 100, center_y);
	ImVec2 arrow_y_p1 = ImVec2(center_x, center_y);
	ImVec2 arrow_y_p2 = ImVec2(center_x, center_y - 100);
	draw_list->AddLine(arrow_x_p1, arrow_x_p2, IM_COL32(255, 0, 0, 200), 3.0f);
	draw_list->AddTriangleFilled(
			ImVec2(arrow_x_p2.x, arrow_x_p2.y - 12),
			ImVec2(arrow_x_p2.x + 12, arrow_x_p2.y),
		  ImVec2(arrow_x_p2.x, arrow_x_p2.y + 12),
			IM_COL32(255, 0, 0, 200));
	draw_list->AddLine(arrow_y_p1, arrow_y_p2, IM_COL32(0, 255, 0, 200), 3.0f);
	draw_list->AddTriangleFilled(
			ImVec2(arrow_y_p2.x, arrow_y_p2.y - 12),
			ImVec2(arrow_y_p2.x - 12, arrow_y_p2.y),
		  ImVec2(arrow_y_p2.x + 12, arrow_y_p2.y),
			IM_COL32(0, 255, 0, 200));
}
