#include <gtest/gtest.h>
#include "core/GameState.hpp"
#include "core/LevelParser.hpp"
#include "core/PathFinder.hpp"
#include <fstream>
#include <filesystem>

// T060: Headless game loop test - full cycle without SFML window

class HeadlessGameLoopTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a minimal winnable level with extra nodes for movement
        const std::string levelJson = R"({
            "name": "Headless Test Level",
            "piece_count": 2,
            "nodes": [
                {"id": 1, "x": 100, "y": 100},
                {"id": 2, "x": 200, "y": 100},
                {"id": 3, "x": 300, "y": 100},
                {"id": 4, "x": 100, "y": 200},
                {"id": 5, "x": 200, "y": 200}
            ],
            "edges": [
                {"a": 1, "b": 2},
                {"a": 2, "b": 3},
                {"a": 1, "b": 4},
                {"a": 4, "b": 5},
                {"a": 5, "b": 3}
            ],
            "start_positions": [1, 4],
            "win_positions": [3, 5]
        })";

        const std::string path = "test_headless_level.json";
        std::ofstream ofs(path);
        ofs << levelJson;
        ofs.close();

        levelData = core::LevelParser::load(path);
        std::filesystem::remove(path);
    }

    core::LevelData levelData;
};

TEST_F(HeadlessGameLoopTest, FullGameLoopWithoutWindow) {
    // Simulate: load level -> select piece -> move -> check win
    core::GameState state;
    state.initializeFromLevel(levelData);

    // Initially not won (pieces at [1, 4], win positions [3, 5])
    EXPECT_FALSE(state.isWinConditionMet());

    // Get reachable nodes from piece 0 (at node 1)
    // Neighbors of 1: 2, 4. But piece 1 is at 4 (occupied).
    // So reachable: [2]
    auto reachable = state.getReachableNodes(0);
    EXPECT_FALSE(reachable.empty());

    // Move piece 0 towards its win position (node 3)
    // Path: 1 -> 2 -> 3
    state.movePiece(0, 2);
    EXPECT_FALSE(state.isWinConditionMet());

    // Now piece 0 is at 2, can reach 3
    state.movePiece(0, 3);

    // Piece 0 is now on win position (3), but piece 1 still at 4
    EXPECT_FALSE(state.isWinConditionMet());

    // Move piece 1 towards its win position (node 5)
    // Path: 4 -> 5
    state.movePiece(1, 5);

    // Now should be won: piece 0 on 3, piece 1 on 5
    EXPECT_TRUE(state.isWinConditionMet());
}

TEST_F(HeadlessGameLoopTest, MultipleLevelsLoadable) {
    // Test that we can load different levels sequentially
    core::GameState state;

    // Load level 1
    state.initializeFromLevel(levelData);
    EXPECT_FALSE(state.isWinConditionMet());

    // Reset and reload (simulates level change)
    state.initializeFromLevel(levelData);
    EXPECT_FALSE(state.isWinConditionMet());
}

TEST_F(HeadlessGameLoopTest, BFSConsistencyAcrossMoves) {
    core::GameState state;
    state.initializeFromLevel(levelData);

    // Initial BFS from piece 0 (at node 1)
    // Neighbors of 1: 2, 4. Piece 1 is at 4 (occupied).
    // So reachable should be [2]
    auto reachableBefore = state.getReachableNodes(0);
    EXPECT_FALSE(reachableBefore.empty());

    // Move piece 0 to node 2
    if (!reachableBefore.empty()) {
        int targetNode = reachableBefore[0];
        state.movePiece(0, targetNode);

        // Now piece 0 is at 2, piece 1 at 4
        // From node 2, neighbors are: 1, 3
        // Both should be reachable now
        auto reachableAfter = state.getReachableNodes(0);
        EXPECT_FALSE(reachableAfter.empty());
    }
}
