#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

namespace core
{
class GameState;
}

namespace game
{

struct MoveCommand
{
    int pieceId = 0;
    int fromNodeId = 0;
    int toNodeId = 0;
    std::vector<sf::Vector2f> waypoints;

    void execute(core::GameState& state);
    void undo(core::GameState& state);
};

}
