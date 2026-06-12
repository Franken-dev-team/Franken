#pragma once
#include <string>
#include <unordered_map>
#include <miniaudio.h>

class AudioManager {
public:
    static AudioManager& GetInstance();

    bool Init();
    void Shutdown();

    void PlaySFX(const std::string& path);
    void PlayMusic(const std::string& path, bool loop = true);
    void LoadAndPlayMusic(const std::string& path, bool loop = true);
    void PauseMusic();
    void ResumeMusic();
    void StopMusic();
    bool IsMusicPlaying();
    void SetMasterVolume(float volume);
    float GetMasterVolume();
    void SetBGMVolume(float volume);
    void SetSFXVolume(float volume);
    void SetMuted(bool muted);
    bool IsMuted();
    void StopAll();

    std::string GetCurrentMusicPath() { return currentMusicPath; }

private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    ma_engine engine;
    ma_sound musicSound;
    std::unordered_map<std::string, ma_sound> sfxSounds;
    bool initialized = false;
    std::string currentMusicPath;
    float lastVolume = 1.0f;
    bool isMuted = false;
};
