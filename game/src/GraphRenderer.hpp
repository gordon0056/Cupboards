#pragma once

#include <SFML/Graphics.hpp>
#include "core/GameState.hpp"
#include <vector>
#include <unordered_set>

namespace game
{

class GraphRenderer
{
public:
    void render(const core::GameState& state,
                sf::RenderTarget& target,
                const std::vector<int>& highlightedNodes = {},
                const sf::Color& highlightColor = sf::Color(255, 255, 0, 128),
                float pieceRadius = 20.0f,
                float nodeRadius = 15.0f,
                bool showWinTargets = true,
                const sf::Vector2f& offset = sf::Vector2f(0.0f, 0.0f));

    void setPieceScreenPosition(int pieceId, sf::Vector2f screenPos);
    sf::Vector2f getPieceScreenPosition(int pieceId) const;

private:
    void renderWinTargets(const core::GameState& state,
                          sf::RenderTarget& target,
                          float pieceRadius,
                          const sf::Vector2f& offset);

    std::vector<sf::Vector2f> pieceScreenPositions;
};

}
