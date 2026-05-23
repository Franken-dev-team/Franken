#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

class FontManager {
public:
    static FontManager& GetInstance();

    // Call once at startup
    bool Init();

    // Load a font at a given point size; key is an alias you choose (e.g. "default", "title")
    // Returns true on success. The font is cached and reused on repeated calls with the same key.
    bool LoadFont(const std::string& key, const std::string& path, float ptSize);

    // Retrieve a loaded font (returns nullptr if not found)
    TTF_Font* GetFont(const std::string& key) const;

    // Convenience: returns the "default" font
    TTF_Font* GetDefault() const;

    // Call once at shutdown
    void Shutdown();

    FontManager(const FontManager&)            = delete;
    FontManager& operator=(const FontManager&) = delete;

private:
    FontManager() = default;
    ~FontManager() = default;

    bool m_initialized = false;
    std::unordered_map<std::string, TTF_Font*> m_fonts;
};
