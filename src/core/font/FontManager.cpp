#include "FontManager.h"
#include <cstdio>

FontManager& FontManager::GetInstance() {
    static FontManager instance;
    return instance;
}

bool FontManager::Init() {
    if (m_initialized) return true;

    if (!TTF_Init()) {
        printf("FontManager: TTF_Init failed: %s\n", SDL_GetError());
        return false;
    }

    m_initialized = true;
    printf("FontManager: initialized successfully\n");

    // Load the default project font automatically
    if (!LoadFont("default", "assets/font/font.ttf", 26.0f)) {
        printf("FontManager: warning - default font not loaded\n");
    }

    return true;
}

bool FontManager::LoadFont(const std::string& key, const std::string& path, float ptSize) {
    if (!m_initialized) {
        printf("FontManager: not initialized\n");
        return false;
    }

    // Free existing font under the same key
    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        TTF_CloseFont(it->second);
        m_fonts.erase(it);
    }

    TTF_Font* font = TTF_OpenFont(path.c_str(), ptSize);
    if (!font) {
        printf("FontManager: failed to load '%s' (%s): %s\n",
               key.c_str(), path.c_str(), SDL_GetError());
        return false;
    }

    m_fonts[key] = font;
    printf("FontManager: loaded font '%s' from '%s' at %.0fpt\n",
           key.c_str(), path.c_str(), ptSize);
    return true;
}

TTF_Font* FontManager::GetFont(const std::string& key) const {
    auto it = m_fonts.find(key);
    if (it == m_fonts.end()) return nullptr;
    return it->second;
}

TTF_Font* FontManager::GetDefault() const {
    return GetFont("default");
}

void FontManager::Shutdown() {
    for (auto& [key, font] : m_fonts) {
        TTF_CloseFont(font);
    }
    m_fonts.clear();

    if (m_initialized) {
        TTF_Quit();
        m_initialized = false;
        printf("FontManager: shutdown\n");
    }
}
