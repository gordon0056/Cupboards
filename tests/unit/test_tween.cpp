#include <catch2/catch_test_macros.hpp>
#include "TweenManager.hpp"
#include <cmath>

TEST_CASE("TweenAnimation progresses along waypoints", "[tween]") {
    game::TweenManager manager;

    std::vector<sf::Vector2f> waypoints = {
        sf::Vector2f(0, 0),
        sf::Vector2f(100, 0),
        sf::Vector2f(200, 0)
    };

    manager.start(0, waypoints, 100.0f); // 100 pixels/sec
    REQUIRE(manager.isAnimating());

    // After 1 second at 100 px/s, should traverse first segment (100px)
    manager.update(1.0f);

    // Should have completed first segment and be on second
    REQUIRE(manager.activeTweens.size() == 1);
}

TEST_CASE("Tween completes and triggers callback", "[tween]") {
    game::TweenManager manager;
    bool callbackTriggered = false;

    manager.onCompleteCallback = [&callbackTriggered]() {
        callbackTriggered = true;
    };

    std::vector<sf::Vector2f> waypoints = {
        sf::Vector2f(0, 0),
        sf::Vector2f(50, 0)
    };

    manager.start(0, waypoints, 50.0f); // 50 px/s, 50px distance = 1s
    manager.update(1.0f);

    REQUIRE(callbackTriggered);
    REQUIRE_FALSE(manager.isAnimating());
}
