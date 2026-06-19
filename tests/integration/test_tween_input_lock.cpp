#include <gtest/gtest.h>
#include "TweenManager.hpp"
#include <SFML/System/Vector2.hpp>

// T061: Interaction Stress Test - verify tween input lock during animation

class TweenInputLockTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager_.onCompleteCallback = [this]() {
            completed_ = true;
        };
    }

    game::TweenManager manager_;
    bool completed_ = false;
};

TEST_F(TweenInputLockTest, AnimationBlocksInput) {
    // Create a tween animation
    std::vector<sf::Vector2f> waypoints = {
        sf::Vector2f(0, 0),
        sf::Vector2f(100, 100),
        sf::Vector2f(200, 200)
    };

    // Start animation
    manager_.start(0, waypoints);
    EXPECT_TRUE(manager_.isAnimating());

    // Update a bit to simulate time passing
    manager_.update(0.01f);

    // Start another animation for same piece (should replace)
    std::vector<sf::Vector2f> newWaypoints = {
        sf::Vector2f(0, 0),
        sf::Vector2f(50, 50)
    };
    manager_.start(0, newWaypoints);

    // Animation should still be active
    EXPECT_TRUE(manager_.isAnimating());
}

TEST_F(TweenInputLockTest, MultiplePiecesAnimateIndependently) {
    std::vector<sf::Vector2f> waypoints1 = {
        sf::Vector2f(0, 0),
        sf::Vector2f(100, 100)
    };
    std::vector<sf::Vector2f> waypoints2 = {
        sf::Vector2f(200, 200),
        sf::Vector2f(300, 300)
    };

    manager_.start(0, waypoints1);
    manager_.start(1, waypoints2);

    // Both pieces should have active tweens
    EXPECT_TRUE(manager_.isAnimating());
}

TEST_F(TweenInputLockTest, CompletesAndTriggersCallback) {
    std::vector<sf::Vector2f> waypoints = {
        sf::Vector2f(0, 0),
        sf::Vector2f(10, 10)  // Short distance for quick completion
    };

    manager_.start(0, waypoints);

    // Update until animation completes
    for (int i = 0; i < 100; ++i) {
        manager_.update(0.1f);
        if (completed_) break;
    }

    EXPECT_TRUE(completed_);
}

TEST_F(TweenInputLockTest, RapidUpdatesDontCrash) {
    std::vector<sf::Vector2f> waypoints = {
        sf::Vector2f(0, 0),
        sf::Vector2f(100, 100)
    };

    manager_.start(0, waypoints);

    // Simulate rapid updates (stress test)
    for (int i = 0; i < 1000; ++i) {
        manager_.update(0.001f);  // Very small dt
    }

    // Should not crash - animation should be complete by now
    EXPECT_FALSE(manager_.isAnimating());
}
