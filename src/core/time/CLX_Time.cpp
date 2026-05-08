#include "CLX_Time.h"
#include <iostream>

Uint64 CLX_Time::lastTime = 0;
double CLX_Time::deltaTime = 0.0;
double CLX_Time::fps = 0.0;
double CLX_Time::fpsTimer = 0.0;
int CLX_Time::frameCount = 0;
int CLX_Time::fpsLimit = 0;
double CLX_Time::targetFrameTime = 0.0;
Uint64 CLX_Time::frameStartTime = 0;
double CLX_Time::maxDeltaTime = 0.1;

void CLX_Time::Update() {
    frameStartTime = SDL_GetPerformanceCounter();
    Uint64 currentTime = frameStartTime;
    if (lastTime == 0) {
        lastTime = currentTime;
    }
    
    Uint64 frequency = SDL_GetPerformanceFrequency();
    deltaTime = static_cast<double>(currentTime - lastTime) / static_cast<double>(frequency);
    if (deltaTime > maxDeltaTime) {
        deltaTime = maxDeltaTime;
    }
    lastTime = currentTime;
    
    frameCount++;
    fpsTimer += deltaTime;
    
    if (fpsTimer >= 1.0) {
        fps = frameCount / fpsTimer;
        frameCount = 0;
        fpsTimer = 0.0;
    }
}

double CLX_Time::GetDeltaTime() {
    return deltaTime;
}

double CLX_Time::GetFPS() {
    return fps;
}

void CLX_Time::SetFPSLimit(int limit) {
    fpsLimit = limit;
    if (limit > 0) {
        targetFrameTime = 1000.0 / limit;
    } else {
        targetFrameTime = 0.0;
    }
}

void CLX_Time::Delay() {
    if (fpsLimit <= 0) {
        return;
    }
    
#ifdef _WIN32
    Uint64 currentFrameTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    double elapsedFrameTime = (static_cast<double>(currentFrameTime - frameStartTime) / static_cast<double>(frequency)) * 1000.0;
    
    if (elapsedFrameTime < targetFrameTime) {
        double remainingTime = targetFrameTime - elapsedFrameTime;
        
        if (remainingTime > 2.0) {
            SDL_Delay(static_cast<Uint32>(remainingTime - 2.0));
        }
        
        while (true) {
            currentFrameTime = SDL_GetPerformanceCounter();
            elapsedFrameTime = (static_cast<double>(currentFrameTime - frameStartTime) / static_cast<double>(frequency)) * 1000.0;
            if (elapsedFrameTime >= targetFrameTime) {
                break;
            }
        }
    }
#else
    Uint32 currentFrameTime = SDL_GetTicks();
    Uint32 elapsedFrameTime = currentFrameTime - static_cast<Uint32>(frameStartTime);
    
    if (elapsedFrameTime < static_cast<Uint32>(targetFrameTime)) {
        Uint32 remainingTime = static_cast<Uint32>(targetFrameTime) - elapsedFrameTime;
        SDL_Delay(remainingTime);
    }
#endif
}
