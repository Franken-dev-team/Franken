#include "CLX_Renderer2D.h"

SDL_Renderer* CLX_Renderer2D::renderer = nullptr;

void CLX_Renderer2D::Init(CLX_Window& window) {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    renderer = SDL_CreateRenderer(window.GetNativeWindow(), nullptr);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return;
    }
}

void CLX_Renderer2D::Shutdown() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
}

void CLX_Renderer2D::BeginFrame() {
    if (renderer) {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);
    }
}

void CLX_Renderer2D::EndFrame() {
    if (renderer) {
        SDL_RenderPresent(renderer);
    }
}

void CLX_Renderer2D::DrawQuad(float x, float y, float w, float h, SDL_Color color) {
    if (!renderer) {
        return;
    }
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    SDL_RenderFillRect(renderer, &rect);
}

SDL_Texture* CLX_Renderer2D::LoadTexture(const char* filePath) {
    SDL_Surface* surface = SDL_LoadBMP(filePath);
    if (!surface) {
        SDL_Log("Failed to load BMP: %s", SDL_GetError());
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    
    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }
    
    return texture;
}

void CLX_Renderer2D::DrawSprite(SDL_Texture* tex, float destX, float destY, int frameIndex, int totalFrames) {
    if (!renderer || !tex) {
        return;
    }
    
    float textureWidth, textureHeight;
    SDL_GetTextureSize(tex, &textureWidth, &textureHeight);
    
    float frameWidth = textureWidth / totalFrames;
    
    SDL_FRect srcRect;
    srcRect.x = frameIndex * frameWidth;
    srcRect.y = 0;
    srcRect.w = frameWidth;
    srcRect.h = textureHeight;
    
    SDL_FRect destRect;
    destRect.x = destX;
    destRect.y = destY;
    destRect.w = frameWidth;
    destRect.h = textureHeight;
    
    SDL_RenderTexture(renderer, tex, &srcRect, &destRect);
}

void CLX_Renderer2D::DrawRectUI(float x, float y, float w, float h, SDL_Color color) {
    if (!renderer) {
        return;
    }
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    SDL_RenderFillRect(renderer, &rect);
}

bool CLX_Renderer2D::PointInRect(float px, float py, float rx, float ry, float rw, float rh) {
    return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}
