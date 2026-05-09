#include "AudioManager.h"

AudioManager& AudioManager::GetInstance() {
    static AudioManager instance;
    return instance;
}

bool AudioManager::Init() {
    if (initialized) return true;
    
    ma_result result = ma_engine_init(nullptr, &engine);
    if (result != MA_SUCCESS) {
        printf("AudioManager: Failed to initialize audio engine (error %d)\n", result);
        return false;
    }
    
    printf("AudioManager: Successfully initialized\n");
    initialized = true;
    return true;
}

void AudioManager::Shutdown() {
    if (!initialized) return;
    
    StopAll();
    
    for (auto& pair : sfxSounds) {
        ma_sound_uninit(&pair.second);
    }
    sfxSounds.clear();
    
    ma_engine_uninit(&engine);
    
    initialized = false;
}

void AudioManager::PlaySFX(const std::string& path) {
    if (!initialized) return;
    
    auto it = sfxSounds.find(path);
    if (it == sfxSounds.end()) {
        ma_sound sound;
        ma_result result = ma_sound_init_from_file(&engine, path.c_str(), MA_SOUND_FLAG_DECODE, nullptr, nullptr, &sound);
        if (result != MA_SUCCESS) {
            return;
        }
        sfxSounds[path] = sound;
        it = sfxSounds.find(path);
    }
    
    ma_sound_stop(&it->second);
    ma_sound_seek_to_pcm_frame(&it->second, 0);
    ma_sound_start(&it->second);
}

void AudioManager::PlayMusic(const std::string& path, bool loop) {
    if (!initialized) {
        printf("AudioManager: Cannot play music - not initialized\n");
        return;
    }
    
    currentMusicPath = path;
    ma_result result = ma_sound_init_from_file(&engine, path.c_str(), MA_SOUND_FLAG_DECODE | (loop ? MA_SOUND_FLAG_LOOPING : 0), nullptr, nullptr, &musicSound);
    if (result != MA_SUCCESS) {
        printf("AudioManager: Failed to load music file '%s' (error %d)\n", path.c_str(), result);
        return;
    }
    
    printf("AudioManager: Playing music file '%s'\n", path.c_str());
    ma_sound_start(&musicSound);
}

void AudioManager::PauseMusic() {
    if (!initialized) return;
    ma_sound_stop(&musicSound);
}

void AudioManager::ResumeMusic() {
    if (!initialized) return;
    ma_sound_start(&musicSound);
}

void AudioManager::StopMusic() {
    if (!initialized) return;
    ma_sound_stop(&musicSound);
    ma_sound_seek_to_pcm_frame(&musicSound, 0);
}

bool AudioManager::IsMusicPlaying() {
    if (!initialized) return false;
    return ma_sound_is_playing(&musicSound);
}

void AudioManager::SetMasterVolume(float volume) {
    if (!initialized) return;
    
    volume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
    lastVolume = volume;
    if (!isMuted) {
        ma_engine_set_volume(&engine, volume);
    }
}

float AudioManager::GetMasterVolume() {
    if (!initialized) return 0.0f;
    return isMuted ? 0.0f : lastVolume;
}

void AudioManager::SetMuted(bool muted) {
    if (!initialized) return;
    
    isMuted = muted;
    if (muted) {
        ma_engine_set_volume(&engine, 0.0f);
    } else {
        ma_engine_set_volume(&engine, lastVolume);
    }
}

bool AudioManager::IsMuted() {
    if (!initialized) return false;
    return isMuted;
}

void AudioManager::StopAll() {
    if (!initialized) return;
    
    for (auto& pair : sfxSounds) {
        ma_sound_stop(&pair.second);
    }
}
