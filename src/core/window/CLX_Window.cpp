#include "CLX_Window.h"

CLX_Window::CLX_Window() : window(nullptr), running(false) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return;
    }
    
    window = SDL_CreateWindow("Cluxe Engine", 800, 600, 0);
    if (!window) {
        SDL_Quit();
        return;
    }
    
    running = true;
}

CLX_Window::~CLX_Window() {
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool CLX_Window::isRunning() const {
    return running;
}

void CLX_Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
    }
}


SDL_Window* CLX_Window::GetNativeWindow() const {
    return window;
}
