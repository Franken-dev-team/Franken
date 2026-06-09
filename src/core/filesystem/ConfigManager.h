#pragma once

#include <unordered_map>
#include <filesystem>
#include <toml++.hpp>
#include <string>

class ConfigManager {
public:
    static bool Init();
    static bool Load();
    static bool Save(const std::string& array_name = "");

    static std::string Get(const std::string& key);
    static void Set(const std::string& key, const std::string& value);

    static const toml::array* GetArray(const std::string& array_name);

private:
    static inline std::string m_path;
    static inline toml::table m_cached_table;
    static inline std::unordered_map<std::string, std::string> m_config;
};
