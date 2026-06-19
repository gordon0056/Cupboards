#pragma once

#include "screens/IScreen.hpp"
#include "AudioManager.hpp"
#include "SettingsScreen.hpp"
#include "Localization.hpp"
#include <string>
#include <functional>

namespace game
{

class MainMenuScreen : public IScreen
{
public:
    MainMenuScreen(const std::string& levelsDir, const std::string& assetsBase,
                   AppSettings* settings);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void renderImGui() override;
    bool isDone() const override;

    void syncSettings();
    AudioManager& getAudioManager() { return audio_; }

    std::function<void()> onNewGame;
    std::function<void()> onContinue;
    std::function<void()> onSettings;
    std::function<void()> onExit;

private:
    void renderHelpModal();
    void renderAboutModal();

    AudioManager audio_;
    std::string assetsBase_;
    std::string levelsDir_;
    AppSettings* settings_ = nullptr;
    bool done_ = false;
    bool showHelp_ = false;
    bool showAbout_ = false;
};

}
