#include "2DGizmo.h"
#include <imgui/imgui.h>
#include <cmath>

int Gizmo::activeAxis = 0;

float DistancePointToLine(ImVec2 point, ImVec2 lineStart, ImVec2 lineEnd) {
	ImVec2 lineVec = ImVec2(lineEnd.x - lineStart.x, lineEnd.y - lineStart.y);
	ImVec2 pointVec = ImVec2(point.x - lineStart.x, point.y - lineStart.y);

	float lineLength = sqrtf(lineVec.x * lineVec.x + lineVec.y * lineVec.y);
	if (lineLength == 0.0f) return sqrtf(pointVec.x * pointVec.x + pointVec.y * pointVec.y);

	float t = (pointVec.x * lineVec.x + pointVec.y * lineVec.y) / (lineLength * lineLength);
	t = (t < 0.0f) ? 0.0f : (t > 1.0f) ? 1.0f : t;

	ImVec2 projection = ImVec2(lineStart.x + t * lineVec.x, lineStart.y + t * lineVec.y);
	ImVec2 distanceVec = ImVec2(point.x - projection.x, point.y - projection.y);

	return sqrtf(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);
}

void Gizmo::Update(int posX, int posY, ImVec2 center, float size) {
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 screenCenter = ImVec2(windowPos.x + center.x, windowPos.y + center.y);

	ImVec2 xAxisStart = screenCenter;
	ImVec2 xAxisEnd = ImVec2(screenCenter.x + size, screenCenter.y);
	ImVec2 yAxisStart = screenCenter;
	ImVec2 yAxisEnd = ImVec2(screenCenter.x, screenCenter.y - size);

	float hitboxSize = 20.0f;

	if (activeAxis == 0) {
		float xDist = DistancePointToLine(mousePos, xAxisStart, xAxisEnd);
		float yDist = DistancePointToLine(mousePos, yAxisStart, yAxisEnd);

		if (xDist < hitboxSize) {
			if (ImGui::IsMouseClicked(0)) {
				activeAxis = 1;
			}
		} else if (yDist < hitboxSize) {
			if (ImGui::IsMouseClicked(0)) {
				activeAxis = 2;
			}
		}
	} else {
		if (ImGui::IsMouseDragging(0)) {
			ImVec2 delta = io.MouseDelta;
			if (activeAxis == 1) {
				posX += delta.x;
			} else if (activeAxis == 2) {
				posY += delta.y;
			}
		}

		if (ImGui::IsMouseReleased(0)) {
			activeAxis = 0;
		}
	}
}

bool Gizmo::IsActive() {
	return activeAxis != 0;
}

void Gizmo::Deactivate() {
	activeAxis = 0;
}

void Gizmo::Render(int center_x, int center_y) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 screenPos = ImVec2(windowPos.x + center_x, windowPos.y + center_y);

	if (activeAxis != 0) {
		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();

		ImU32 axisColor = (activeAxis == 1) ? IM_COL32(255, 0, 0, 40) : IM_COL32(0, 255, 0, 40);

		if (activeAxis == 1) {
			draw_list->AddLine(ImVec2(windowPos.x, screenPos.y), ImVec2(windowPos.x + windowWidth, screenPos.y), axisColor, 1.0f);

			draw_list->AddRectFilledMultiColor(
				ImVec2(screenPos.x - 100, screenPos.y - 1),
				ImVec2(screenPos.x + 100, screenPos.y + 1),
				IM_COL32(255, 0, 0, 0), IM_COL32(255, 0, 0, 150),
				IM_COL32(255, 0, 0, 150), IM_COL32(255, 0, 0, 0)
			);
		} else if (activeAxis == 2) {
			draw_list->AddLine(ImVec2(screenPos.x, windowPos.y), ImVec2(screenPos.x, windowPos.y + windowHeight), axisColor, 1.0f);

			draw_list->AddRectFilledMultiColor(
				ImVec2(screenPos.x - 1, screenPos.y - 100),
				ImVec2(screenPos.x + 1, screenPos.y + 100),
				IM_COL32(0, 255, 0, 0), IM_COL32(0, 255, 0, 0),
				IM_COL32(0, 255, 0, 150), IM_COL32(0, 255, 0, 150)
			);
		}
	}
}

void Gizmo::drawCircle(ImDrawList* draw_list, ImVec2 center) {
	draw_list->AddCircleFilled(center, 25.0f, IM_COL32(255, 255, 255, 100));
}

void Gizmo::drawArrow(ImDrawList* draw_list, ImVec2 center) {
	float arrowSize = 100.0f;
	float headSize = 10.0f;

	ImVec2 arrow_x_end = ImVec2(center.x + arrowSize, center.y);
	ImVec2 arrow_y_end = ImVec2(center.x, center.y - arrowSize);

	draw_list->AddLine(center, arrow_x_end, IM_COL32(255, 0, 0, 200), 3.0f);
	draw_list->AddTriangleFilled(
		ImVec2(arrow_x_end.x, arrow_x_end.y - headSize),
		ImVec2(arrow_x_end.x + headSize, arrow_x_end.y),
		ImVec2(arrow_x_end.x, arrow_x_end.y + headSize),
		IM_COL32(255, 0, 0, 200)
	);

	draw_list->AddLine(center, arrow_y_end, IM_COL32(0, 255, 0, 200), 3.0f);
	draw_list->AddTriangleFilled(
		ImVec2(arrow_y_end.x - headSize, arrow_y_end.y),
		ImVec2(arrow_y_end.x, arrow_y_end.y - headSize),
		ImVec2(arrow_y_end.x + headSize, arrow_y_end.y),
		IM_COL32(0, 255, 0, 200)
	);
}
