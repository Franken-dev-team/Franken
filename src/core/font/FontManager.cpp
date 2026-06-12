#include "FontManager.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

FontManager& FontManager::GetInstance() {
    static FontManager instance;
    return instance;
}

bool FontManager::Init() {
    if (m_initialized) return true;

    // SDL3_ttf returns true on success, false on failure
    if (!TTF_Init()) {
        printf("FontManager: SDL_ttf initialization failed: %s\n", SDL_GetError());
        return false;
    }

    m_initialized = true;
    printf("FontManager: TTF_Init Success\n");

    std::string basepath = "";
    const char* sdlBasePath = SDL_GetBasePath();
    if (sdlBasePath) {
        basepath = sdlBasePath;
        SDL_free((void*)sdlBasePath); // Cast to void* for SDL_free
    }
    printf("FontManager: Base path = %s\n", basepath.c_str());

    std::vector<std::string> fontPaths;
    
    // 1. Try paths relative to executable (Most reliable for deployed builds)
    if (!basepath.empty()) {
        fontPaths.push_back(basepath + "assets/font/font.ttf");
        fontPaths.push_back(basepath + "assets/fonts/font.ttf");
    }
    
    // 2. Try paths relative to working directory (For dev/IDE runs)
    fontPaths.push_back("assets/font/font.ttf");
    fontPaths.push_back("assets/fonts/font.ttf");
    fontPaths.push_back("../assets/font/font.ttf");
    fontPaths.push_back("../../assets/font/font.ttf");

    #ifdef _WIN32
        fontPaths.push_back("C:/Windows/Fonts/arial.ttf");
        fontPaths.push_back("C:/Windows/Fonts/consola.ttf");
    #endif

    bool fontLoaded = false;
    for (std::string& path : fontPaths) {
        // Normalize path slashes for Windows if needed
        std::replace(path.begin(), path.end(), '/', '\\');
        
        if (LoadFont("default", path, 32.0f)) {
            printf("FontManager: SUCCESS! Loaded font from: %s\n", path.c_str());
            TTF_SetFontStyle(GetFont("default"), TTF_STYLE_BOLD);
            fontLoaded = true;
            break;
        } else {
            printf("FontManager: Failed to load from: %s\n", path.c_str());
        }
    }

    if (!fontLoaded) {
        printf("FontManager: CRITICAL - Failed to load any font!\n");
        return false;
    }

    return true;
}

bool FontManager::LoadFont(const std::string& key, const std::string& path, float ptSize) {
    if (!m_initialized) {
        printf("FontManager: Not initialized\n");
        return false;
    }

    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        TTF_CloseFont(it->second);
        m_fonts.erase(it);
    }

    TTF_Font* font = TTF_OpenFont(path.c_str(), ptSize);

    if (!font) {
        printf("FontManager: TTF_OpenFont error: %s\n", SDL_GetError());
        return false;
    }

    m_fonts[key] = font;
    printf("FontManager: SUCCESS loaded font %s\n", path.c_str());
    return true;
}

TTF_Font* FontManager::GetFont(const std::string& key) const {
    auto it = m_fonts.find(key);
    if (it == m_fonts.end()) {
        printf("FontManager: Font with key '%s' not found\n", key.c_str());
        return nullptr;
    }
    return it->second;
}

TTF_Font* FontManager::GetDefault() const {
    return GetFont("default");
}

void FontManager::Shutdown() {
    for (auto& pair : m_fonts) {
        TTF_CloseFont(pair.second);
    }
    m_fonts.clear();

    if (m_initialized) {
        TTF_Quit();
        m_initialized = false;
        printf("FontManager: Shutdown\n");
    }
}
