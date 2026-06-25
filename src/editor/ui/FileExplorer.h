#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <imgui.h>
#include <fstream>

class FileExplorer {
public:
    static void Render();
    static void SetRootPath(std::string path);
    static void SetCurrentPath(std::string path = "");
    static void SetPreviousPath(std::string path);
    static void GoToRoot();
    static void GoToParent();
    static void GoToPrevious();
    static void GoToNext();
    static void DeleteFile(std::string path);
    static void DeleteFolder(std::string path);
    static void CreateFile(std::string path);
    static void CreateFolder(std::string path);
    static void RenameFile(std::string path, std::string newName);
    static void RenameFolder(std::string path, std::string newName);
    static void CopyFile(std::string path, std::string destination);
    static void CopyFolder(std::string path, std::string destination);
    static void MoveFile(std::string path, std::string destination);
    static void MoveFolder(std::string path, std::string destination);
    static std::string GetFullPath();

private:
    static inline std::string m_rootPath;
    static inline std::string m_currentPath = m_rootPath + "/";
    static inline std::string m_previousPath;
};
