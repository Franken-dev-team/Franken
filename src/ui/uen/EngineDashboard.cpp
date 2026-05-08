#include "EngineDashboard.h"
#include "../../core/renderer/CLX_Renderer2D.h"
#include "../../core/input/CLX_Input.h"
#include "../../core/time/CLX_Time.h"
#include "../../../include/imgui/imgui.h"
#include "../../../include/imgui/backends/imgui_impl_sdl3.h"
#include "../../../include/imgui/backends/imgui_impl_sdlrenderer3.h"
#include <cstdio>

namespace UEN {
    float playerSpeed = 5.0f;

    void ApplyBlackWhiteTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
        colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.7f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.7f, 0.7f, 0.7f, 0.9f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
        
        style.WindowRounding = 2.0f;
        style.FrameRounding = 2.0f;
        style.PopupRounding = 2.0f;
        style.ScrollbarRounding = 2.0f;
        style.GrabRounding = 2.0f;
        style.TabRounding = 2.0f;
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
    }

    void RenderEngineDashboard() {
        ApplyBlackWhiteTheme();
        
        ImGui::Begin("Cluxe Editor");
        
        ImGui::SliderFloat("Player Speed", &playerSpeed, 1.0f, 20.0f);
        
        float fps = CLX_Time::GetFPS();
        ImGui::Text("FPS: %.1f", fps);
        
        int mouseX, mouseY;
        CLX_Input::GetMousePos(&mouseX, &mouseY);
        ImGui::Text("Mouse: (%d, %d)", mouseX, mouseY);
        
        bool leftClick = CLX_Input::IsMouseButtonPressed(1);
        bool rightClick = CLX_Input::IsMouseButtonPressed(3);
        ImGui::Text("Left Click: %s", leftClick ? "Pressed" : "Released");
        ImGui::Text("Right Click: %s", rightClick ? "Pressed" : "Released");
        
        ImGui::End();
    }
}
