#include "2DGizmo.h"
#include <imgui/imgui.h>
#include <cmath>

int Gizmo::activeAxis = 0;

// Shared last-known screen-space center, set each frame by Update()
static ImVec2 s_gizmoCenter = ImVec2(0, 0);
static float  s_gizmoSize   = 100.0f;

static float DistancePointToLine(ImVec2 point, ImVec2 lineStart, ImVec2 lineEnd) {
    ImVec2 lineVec  = ImVec2(lineEnd.x - lineStart.x, lineEnd.y - lineStart.y);
    ImVec2 pointVec = ImVec2(point.x - lineStart.x,   point.y - lineStart.y);

    float lineLength = sqrtf(lineVec.x * lineVec.x + lineVec.y * lineVec.y);
    if (lineLength == 0.0f)
        return sqrtf(pointVec.x * pointVec.x + pointVec.y * pointVec.y);

    float t = (pointVec.x * lineVec.x + pointVec.y * lineVec.y) / (lineLength * lineLength);
    t = (t < 0.0f) ? 0.0f : (t > 1.0f) ? 1.0f : t;

    ImVec2 proj    = ImVec2(lineStart.x + t * lineVec.x, lineStart.y + t * lineVec.y);
    ImVec2 distVec = ImVec2(point.x - proj.x, point.y - proj.y);
    return sqrtf(distVec.x * distVec.x + distVec.y * distVec.y);
}

// center is already in screen space (absolute)
void Gizmo::Update(int& posX, int& posY, ImVec2 center, float size) {
    ImGuiIO& io     = ImGui::GetIO();
    ImVec2 mousePos = ImGui::GetMousePos();

    // Remember for IsHovered()
    s_gizmoCenter = center;
    s_gizmoSize   = size;

    ImVec2 sc       = center;
    ImVec2 xAxisEnd = ImVec2(sc.x + size, sc.y);
    ImVec2 yAxisEnd = ImVec2(sc.x, sc.y - size);

    const float hitbox = 12.0f;

    if (activeAxis == 0) {
        float xDist = DistancePointToLine(mousePos, sc, xAxisEnd);
        float yDist = DistancePointToLine(mousePos, sc, yAxisEnd);

        if (xDist < hitbox && ImGui::IsMouseClicked(0)) {
            activeAxis = 1;
        } else if (yDist < hitbox && ImGui::IsMouseClicked(0)) {
            activeAxis = 2;
        }
    } else {
        if (ImGui::IsMouseDragging(0)) {
            ImVec2 delta = io.MouseDelta;
            if (activeAxis == 1) posX += (int)delta.x;
            if (activeAxis == 2) posY += (int)delta.y;
        }
        if (ImGui::IsMouseReleased(0)) {
            activeAxis = 0;
        }
    }
}

bool Gizmo::IsActive() {
    return activeAxis != 0;
}

bool Gizmo::IsHovered() {
    if (s_gizmoCenter.x == 0 && s_gizmoCenter.y == 0) return false;
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 sc       = s_gizmoCenter;
    ImVec2 xAxisEnd = ImVec2(sc.x + s_gizmoSize, sc.y);
    ImVec2 yAxisEnd = ImVec2(sc.x, sc.y - s_gizmoSize);
    const float hitbox = 12.0f;
    return DistancePointToLine(mousePos, sc, xAxisEnd) < hitbox ||
           DistancePointToLine(mousePos, sc, yAxisEnd) < hitbox;
}

void Gizmo::Deactivate() {
    activeAxis = 0;
}

// ── private helpers ──────────────────────────────────────────────────────────

void Gizmo::DrawCircle(ImDrawList* draw_list, ImVec2 center) {
    draw_list->AddCircleFilled(center, 6.0f, IM_COL32(255, 255, 255, 220));
    draw_list->AddCircle(center, 6.0f, IM_COL32(0, 0, 0, 180), 0, 1.5f);
}

void Gizmo::DrawArrows(ImDrawList* draw_list, ImVec2 sc, float size) {
    const float lineThick = 3.0f;
    const float headSize  = 10.0f;

    // X axis (red)
    ImU32 xCol = (activeAxis == 1) ? IM_COL32(255, 220, 0, 255) : IM_COL32(220, 50, 50, 230);
    ImVec2 xEnd = ImVec2(sc.x + size, sc.y);
    draw_list->AddLine(sc, xEnd, xCol, lineThick);
    draw_list->AddTriangleFilled(
        ImVec2(xEnd.x,            xEnd.y - headSize),
        ImVec2(xEnd.x + headSize, xEnd.y),
        ImVec2(xEnd.x,            xEnd.y + headSize),
        xCol
    );

    // Y axis (green)
    ImU32 yCol = (activeAxis == 2) ? IM_COL32(255, 220, 0, 255) : IM_COL32(50, 220, 50, 230);
    ImVec2 yEnd = ImVec2(sc.x, sc.y - size);
    draw_list->AddLine(sc, yEnd, yCol, lineThick);
    draw_list->AddTriangleFilled(
        ImVec2(yEnd.x - headSize, yEnd.y),
        ImVec2(yEnd.x,            yEnd.y - headSize),
        ImVec2(yEnd.x + headSize, yEnd.y),
        yCol
    );
}

// ── public Render ─────────────────────────────────────────────────────────────
// center_x / center_y are screen-space (absolute) coordinates
void Gizmo::Render(float center_x, float center_y, float size) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 sc             = ImVec2(center_x, center_y);

    DrawArrows(draw_list, sc, size);
    DrawCircle(draw_list, sc);
}
