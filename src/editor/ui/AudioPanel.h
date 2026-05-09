#pragma once
#include "../../core/audio/AudioManager.h"

class AudioPanel {
public:
    static void Render();
    
private:
    static float volume;
    static bool isMuted;
};
