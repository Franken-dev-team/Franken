#pragma once
#include <SDL3/SDL.h>
#include "../window/CLX_Window.h"

class CLX_Renderer2D {
private:
    static SDL_Renderer* renderer;

public:
    static void Init(CLX_Window& window);
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame();
    static void DrawQuad(float x, float y, float w, float h, SDL_Color color);
    static SDL_Texture* LoadTexture(const char* filePath);
    static void DrawSprite(SDL_Texture* tex, float destX, float destY, int frameIndex, int totalFrames);
    static void DrawRectUI(float x, float y, float w, float h, SDL_Color color);
    static bool PointInRect(float px, float py, float rx, float ry, float rw, float rh);
};
