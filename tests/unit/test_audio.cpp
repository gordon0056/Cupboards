#include <catch2/catch_test_macros.hpp>
#include "AudioManager.hpp"

TEST_CASE("AudioManager gracefully handles missing files", "[audio]") {
    game::AudioManager manager;

    // Missing music file — should return false, not crash
    REQUIRE_FALSE(manager.loadMusic("nonexistent/path/music.ogg"));

    // Missing sound effect — should return false, not crash
    REQUIRE_FALSE(manager.loadSound("click", "nonexistent/click.wav"));

    // Playing sound that was never loaded — should not crash
    manager.playSound("click"); // No assertion needed — just shouldn't crash

    // Stopping when nothing loaded — should not crash
    manager.stopMusic();
    manager.stopAll();
}
