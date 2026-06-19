#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>

namespace game
{

class IScreen
{
public:
    virtual ~IScreen() = default;

    virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;
    virtual void renderImGui() = 0;

    virtual bool isDone() const = 0;
};

}
