#pragma once
#include <SDL3/SDL.h>

class CLX_Input {
private:
    static const bool* keyboardState;

public:
    static void Update();
    static bool IsKeyPressed(SDL_Scancode key);
    static void GetMousePos(int* x, int* y);
    static bool IsMouseButtonPressed(int button);
};
