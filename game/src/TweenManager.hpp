#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <functional>

namespace game
{

struct TweenAnimation
{
    int pieceId = 0;
    std::vector<sf::Vector2f> waypoints;
    int currentWaypointIndex = 0;
    float progress = 0.0f;
    float speed = 200.0f;

    bool isComplete = false;

    sf::Vector2f currentPosition() const;
};

class TweenManager
{
public:
    std::vector<TweenAnimation> activeTweens;
    std::function<void()> onCompleteCallback;

    void start(int pieceId, const std::vector<sf::Vector2f>& waypoints, float speed = 200.0f);
    void update(float dt);
    bool isAnimating() const;
};

}
