#pragma once

#include "screens/IScreen.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace game
{

class Application
{
public:
    void pushScreen(std::unique_ptr<IScreen> screen);
    void popScreen();
    void popToBottom();

    void applyPending();

    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(float dt);
    void render(sf::RenderTarget& target);
    void renderImGui();
    bool isRunning() const;

    std::function<void(const std::string& levelPath)> onLevelSelected;

private:
    std::vector<std::unique_ptr<IScreen>> screens_;
    int pendingPopCount_ = 0;
    std::vector<std::unique_ptr<IScreen>> pendingPushes_;
};

}
