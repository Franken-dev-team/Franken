#include "ConfigManager.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>

bool ConfigManager::Init() {
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
    std::filesystem::path target_path = std::filesystem::path(home ? home : ".") / "FrankenProjects" / "FrankenEngine";
#else
    const char* home = std::getenv("HOME");
    std::filesystem::path target_path = std::filesystem::path(home ? home : ".") / ".config" / "FrankenEngine";
#endif

    std::filesystem::create_directories(target_path);
    m_path = (target_path / "config.toml").string();

    if (!std::filesystem::exists(m_path)) {
        return Save();
    }

    return Load();
}

bool ConfigManager::Load() {
    try {
        m_cached_table = toml::parse_file(m_path);

        m_config.clear();
        m_config.reserve(m_cached_table.size());

        for (const auto& [key, value] : m_cached_table) {
            std::string key_str{key.str()};
            if (auto str_node = value.as_string()) {
                m_config[key_str] = str_node->get();
            } else {
                m_config[key_str] = value.value_or(std::string(""));
            }
        }
        return true;
    } catch (const toml::parse_error& e) {
        std::fprintf(stderr, "Failed to parse config file: %s\n", e.what());
        return false;
    }
}

bool ConfigManager::Save(const std::string& array_name) {
    if (!array_name.empty()) {
        toml::array* table_array = m_cached_table[array_name].as_array();

        if (!table_array) {
            m_cached_table.insert_or_assign(array_name, toml::array{});
            table_array = m_cached_table[array_name].as_array();
        }

        toml::table new_project_item;
        for (const auto& [key, value] : m_config) {
            if (key != array_name) {
                new_project_item.insert_or_assign(key, value);
            }
        }

        if (table_array) {
            table_array->push_back(std::move(new_project_item));
        }
    } else {
        for (const auto& [key, value] : m_config) {
            m_cached_table.insert_or_assign(key, value);
        }
    }

    std::ofstream file(m_path, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file << m_cached_table;
    return true;
}

std::string ConfigManager::Get(const std::string& key) {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        return it->second;
    }
    return "";
}

void ConfigManager::Set(const std::string& key, const std::string& value) {
    m_config[key] = value;
}

const toml::array* ConfigManager::GetArray(const std::string& array_name) {
    if (auto existing_node = m_cached_table[array_name].as_array()) {
        return existing_node;
    }
    return nullptr;
}
