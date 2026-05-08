#include "CLX_Input.h"

const bool* CLX_Input::keyboardState = nullptr;

void CLX_Input::Update() {
    keyboardState = SDL_GetKeyboardState(NULL);
}

bool CLX_Input::IsKeyPressed(SDL_Scancode key) {
    if (!keyboardState) {
        return false;
    }
    return keyboardState[key];
}

void CLX_Input::GetMousePos(int* x, int* y) {
    float fx, fy;
    SDL_GetMouseState(&fx, &fy);
    *x = static_cast<int>(fx);
    *y = static_cast<int>(fy);
}

bool CLX_Input::IsMouseButtonPressed(int button) {
    Uint32 state = SDL_GetMouseState(NULL, NULL);
    return (state & SDL_BUTTON_MASK(button)) != 0;
}
