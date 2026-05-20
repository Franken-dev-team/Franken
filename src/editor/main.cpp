#include "../core/window/CLX_Window.h"
#include "../game/Game.h"
#include <SDL3/SDL.h>

int main() {
    CLX_Window window;
    SDL_Window* sdlWindow = window.GetNativeWindow();
    SDL_Renderer* renderer = SDL_CreateRenderer(sdlWindow, nullptr);

    Game game;
    game.Init(sdlWindow, renderer);

    while (window.isRunning() && !game.ShouldQuit()) {
        game.Update(0.016f);
        game.Render();
    }

    game.Shutdown();
    SDL_DestroyRenderer(renderer);

    return 0;
}
