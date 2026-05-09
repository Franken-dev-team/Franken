#include "AudioPanel.h"
#include <imgui.h>

float AudioPanel::volume = 1.0f;
bool AudioPanel::isMuted = false;

void AudioPanel::Render() {
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Audio Panel", nullptr, ImGuiWindowFlags_NoCollapse)) {
        AudioManager& audio = AudioManager::GetInstance();
        
        ImGui::Text("Status");
        ImGui::Separator();
        
        if (audio.IsMusicPlaying()) {
            ImGui::Text("Music: Playing");
        } else {
            ImGui::Text("Music: Stopped");
        }
        
        ImGui::Text("Current: assets/bg/bg.mp3");
        
        ImGui::Spacing();
        ImGui::Text("Controls");
        ImGui::Separator();
        
        if (audio.IsMusicPlaying()) {
            if (ImGui::Button("Pause", ImVec2(80, 30))) {
                audio.PauseMusic();
            }
        } else {
            if (ImGui::Button("Play", ImVec2(80, 30))) {
                audio.ResumeMusic();
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Stop", ImVec2(80, 30))) {
            audio.StopMusic();
        }
        
        ImGui::Spacing();
        ImGui::Text("Volume");
        ImGui::Separator();
        
        if (ImGui::SliderFloat("Master Volume", &volume, 0.0f, 1.0f)) {
            audio.SetMasterVolume(volume);
        }
        
        if (ImGui::Checkbox("Mute", &isMuted)) {
            audio.SetMuted(isMuted);
        }
        
        ImGui::End();
    }
}
