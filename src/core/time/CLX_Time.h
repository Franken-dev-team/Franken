#pragma once
#include <SDL3/SDL.h>

class CLX_Time {
private:
    static Uint64 lastTime;
    static double deltaTime;
    static double fps;
    static double fpsTimer;
    static int frameCount;
    static int fpsLimit;
    static double targetFrameTime;
    static Uint64 frameStartTime;
    static double maxDeltaTime;

public:
    static void Update();
    static double GetDeltaTime();
    static double GetFPS();
    static void SetFPSLimit(int limit);
    static void Delay();
};
