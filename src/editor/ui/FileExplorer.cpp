#include "FileExplorer.h"
#include <cstdio>
#include <imgui/imgui.h>
#include "PopUpMenu.h"

void FileExplorer::Render() {
    if (ImGui::Button("Root")) {
        GoToRoot();
    }
    if (ImGui::Button("Parent")) {
        GoToParent();
    }
    if (ImGui::Button("Previous")) {
        GoToPrevious();
    }
    std::string path = m_rootPath + m_currentPath;
    ImGui::Text("Path: %s", path.c_str());
    ImGui::Text("Root Path: %s", m_rootPath.c_str());
    ImGui::Text("Current Path: %s", m_currentPath.c_str());
    ImGui::Text("Previous Path: %s", m_previousPath.c_str());
    if (std::filesystem::exists(path)) {
        std::filesystem::directory_iterator it(path);
        for (const auto& entry : it) {
            if (std::filesystem::is_directory(entry)) {
                ImGui::Text("Folder: %s", entry.path().filename().c_str());
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    m_previousPath = m_currentPath;
                    m_currentPath = m_currentPath + "/" + entry.path().filename().string();
                }
                if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    ImGui::OpenPopup("fileExplorerMenu");
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
                    ImGui::OpenPopup("itemMenu");
                }
            } else {
                ImGui::Text("File: %s", entry.path().filename().c_str());
            }
        }
    }
    PopUpMenu::Draw("itemMenu",
        PopUpMenu::PopUpItem{"Delete", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"Rename", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"Duplicate", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"Copy", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"Paste", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"Cut", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"New File", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"New Folder", "", [&]() { printf("stuff"); }}
    );
    PopUpMenu::Draw("fileExplorerMenu",
        PopUpMenu::PopUpItem{"New File", "", [&]() { printf("stuff"); }},
        PopUpMenu::PopUpItem{"New Folder", "", [&]() { printf("stuff"); }}
    );
}

void FileExplorer::SetRootPath(std::string path) {
    m_rootPath = path;
    m_currentPath = "";
}

void FileExplorer::SetCurrentPath(std::string path) {
    m_currentPath = path;
}

void FileExplorer::SetPreviousPath(std::string path) {
    m_previousPath = path;
}

void FileExplorer::GoToRoot() {
    if (!m_currentPath.empty()) {
        m_previousPath = m_currentPath;
    }
    m_currentPath = "";
}

void FileExplorer::GoToParent() {
    if (!m_currentPath.empty()) {
        m_previousPath = m_currentPath;
    }
    size_t pos = m_currentPath.find_last_of('/');
    if (pos != std::string::npos) {
        m_currentPath = m_currentPath.substr(0, pos);
    }
}

void FileExplorer::GoToPrevious() {
    std::string dest = m_previousPath;
    m_previousPath = m_currentPath;
    m_currentPath = dest;
}

void FileExplorer::GoToNext() {
    m_previousPath = m_currentPath;
    std::string previous = std::filesystem::path(m_currentPath).parent_path().string();
    SetCurrentPath(previous);
}

void FileExplorer::CopyFile(std::string path, std::string destination) {
    std::filesystem::copy_file(path, destination);
}
void FileExplorer::CopyFolder(std::string path, std::string destination) {
    std::filesystem::copy(path, destination, std::filesystem::copy_options::recursive);
}

void FileExplorer::MoveFile(std::string path, std::string destination) {
    std::filesystem::rename(path, destination);
}

void FileExplorer::MoveFolder(std::string path, std::string destination) {
    std::filesystem::rename(path, destination);
}

void FileExplorer::DeleteFile(std::string path) {
    std::filesystem::remove(path);
}
void FileExplorer::DeleteFolder(std::string path) {
    std::filesystem::remove_all(path);
}

void FileExplorer::RenameFile(std::string path, std::string newName) {
    std::filesystem::rename(path, newName);
}
void FileExplorer::RenameFolder(std::string path, std::string newName) {
    std::filesystem::rename(path, newName);
}

void FileExplorer::CreateFile(std::string path) {
    std::ofstream file(path);
    file.close();
}
void FileExplorer::CreateFolder(std::string path) {
    std::filesystem::create_directory(path);
}

std::string FileExplorer::GetFullPath() {
    return std::filesystem::path(m_rootPath + m_currentPath).string();
}
