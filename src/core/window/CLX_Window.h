#pragma once
#include <SDL3/SDL.h>

class CLX_Window {
private:
    SDL_Window* window;
    bool running;

public:
    CLX_Window();
    ~CLX_Window();
    
    bool isRunning() const;
    void pollEvents();
    SDL_Window* GetNativeWindow() const;
};
