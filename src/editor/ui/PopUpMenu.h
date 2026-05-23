#pragma once
#include <functional>
#include <imgui/imgui.h>
#include <string>
#include <utility>

class PopUpMenu {
    public:
        template<typename F>
        struct PopUpItem {
         const char* label;
         const char* shortcut;
         F callback;
         PopUpItem(const char* l, const char* s, F&& c)
                     : label(l), shortcut(s), callback(std::forward<F>(c)) {}
        };
        template<typename F>
        PopUpItem(const char* label, const char* shortcut, F callback) -> PopUpItem<F>;

        template<typename Item>
        static void DrawItem(Item&& item) {
            if (ImGui::MenuItem(item.label, item.shortcut)) {
                item.callback();
            }
        }

        template<typename... Items>
        static void Draw(const std::string& title, Items&&... items) {
            if (ImGui::BeginPopup(title.c_str(), ImGuiPopupFlags_MouseButtonRight)) {
                (DrawItem(std::forward<Items>(items)), ...);
                ImGui::EndPopup();
            }
        }
};
