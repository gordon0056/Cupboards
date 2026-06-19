#include "AudioManager.hpp"
#include <iostream>
#include <algorithm>

namespace game
{

bool AudioManager::loadMusic(const std::string& path)
{
    stopMusic();
    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(path))
    {
        std::cerr << "Warning: Cannot load music: " << path << std::endl;
        return false;
    }
    music->setLoop(true);
    applyMusicVolume();
    music_ = std::move(music);
    return true;
}

bool AudioManager::loadSound(const std::string& name, const std::string& path)
{
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(path))
    {
        std::cerr << "Warning: Cannot load sound: " << path << std::endl;
        return false;
    }
    auto sound = std::make_unique<sf::Sound>();
    sound->setBuffer(*buffer);
    sound->setVolume(sfxVolume_ * (masterVolume_ / 100.0f));
    soundBuffers_[name] = std::move(buffer);
    sounds_[name] = std::move(sound);
    return true;
}

void AudioManager::playSound(const std::string& name)
{
    auto it = sounds_.find(name);
    if (it != sounds_.end())
    {
        it->second->setVolume(sfxVolume_ * (masterVolume_ / 100.0f));
        it->second->play();
    }
}

void AudioManager::playMusic()
{
    if (music_)
    {
        applyMusicVolume();
        music_->play();
    }
}

void AudioManager::stopMusic()
{
    if (music_)
    {
        music_->stop();
        music_.reset();
    }
}

void AudioManager::stopAll()
{
    stopMusic();
    stopAllSounds();
}

void AudioManager::stopAllSounds()
{
    for (auto& pair : sounds_)
        pair.second->stop();
}

void AudioManager::setMasterVolume(float vol)
{
    masterVolume_ = std::clamp(vol, 0.0f, 100.0f);
    applyMusicVolume();
    applySfxVolume();
}

void AudioManager::setMusicVolume(float vol)
{
    musicVolume_ = std::clamp(vol, 0.0f, 100.0f);
    applyMusicVolume();
}

void AudioManager::setSfxVolume(float vol)
{
    sfxVolume_ = std::clamp(vol, 0.0f, 100.0f);
    applySfxVolume();
}

float AudioManager::getMasterVolume() const { return masterVolume_; }
float AudioManager::getMusicVolume() const { return musicVolume_; }
float AudioManager::getSfxVolume() const { return sfxVolume_; }

void AudioManager::applyMusicVolume()
{
    if (music_)
        music_->setVolume(musicVolume_ * (masterVolume_ / 100.0f));
}

void AudioManager::applySfxVolume()
{
    float effective = sfxVolume_ * (masterVolume_ / 100.0f);
    for (auto& pair : sounds_)
        pair.second->setVolume(effective);
}

}
