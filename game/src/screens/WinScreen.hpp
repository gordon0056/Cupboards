#pragma once

#include "screens/IScreen.hpp"
#include "Localization.hpp"
#include <memory>
#include <functional>

namespace game
{

class WinScreen : public IScreen
{
public:
    explicit WinScreen(int language = 0);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void renderImGui() override;
    bool isDone() const override;

    std::function<void()> onMenu;

private:
    int language_ = 0;
    bool done_ = false;
};

}
