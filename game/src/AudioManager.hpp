#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <memory>

namespace game
{

class AudioManager
{
public:
    bool loadMusic(const std::string& path);
    bool loadSound(const std::string& name, const std::string& path);
    void playSound(const std::string& name);
    void playMusic();
    void stopMusic();
    void stopAll();

    void setMasterVolume(float vol);
    void setMusicVolume(float vol);
    void setSfxVolume(float vol);

    float getMasterVolume() const;
    float getMusicVolume() const;
    float getSfxVolume() const;

    void stopAllSounds();

private:
    void applyMusicVolume();
    void applySfxVolume();

    std::unique_ptr<sf::Music> music_;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers_;
    std::unordered_map<std::string, std::unique_ptr<sf::Sound>> sounds_;
    float masterVolume_ = 80.0f;
    float musicVolume_ = 70.0f;
    float sfxVolume_ = 90.0f;
};

}
