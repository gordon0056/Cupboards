#pragma once

#include "screens/IScreen.hpp"
#include "AudioManager.hpp"
#include "SettingsScreen.hpp"
#include "Localization.hpp"
#include <string>
#include <vector>
#include <functional>

namespace game
{

class MenuScreen : public IScreen
{
public:
    MenuScreen(const std::string& levelsDir, const std::string& assetsBase = "",
               AppSettings* settings = nullptr);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void renderImGui() override;
    bool isDone() const override;

    void syncSettings();
    AudioManager& getAudioManager() { return audio_; }

    std::function<void(const std::string&)> onLevelSelected;
    std::function<void()> onBack;

private:
    struct LevelEntry
    {
        std::string path;
        std::string name;
        bool valid = true;
        std::string errorMessage;
    };

    void scanLevels(const std::string& levelsDir);

    AudioManager audio_;
    std::vector<LevelEntry> levels_;
    AppSettings* settings_ = nullptr;
    bool done_ = false;
};

}
