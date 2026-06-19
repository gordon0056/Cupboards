#pragma once

#include "screens/IScreen.hpp"
#include "Localization.hpp"
#include <functional>

namespace game
{

struct AppSettings
{
    float masterVolume = 80.0f;
    float musicVolume = 70.0f;
    float sfxVolume = 90.0f;
    bool darkTheme = true;
    int language = 0;
};

class SettingsScreen : public IScreen
{
public:
    SettingsScreen(AppSettings& settings);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void renderImGui() override;
    bool isDone() const override;

    std::function<void()> onBack;
    std::function<void()> onApply;
    std::function<void()> onReset;

private:
    AppSettings& settings_;
    AppSettings edit_;
    bool done_ = false;
};

}
