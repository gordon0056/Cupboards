#include <gtest/gtest.h>
#include "core/GameState.hpp"
#include "core/LevelParser.hpp"
#include <fstream>
#include <filesystem>

// T057-T058: Win condition tests

class GameStateGTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a minimal test level
        const std::string levelJson = R"({
            "name": "Win Test Level",
            "piece_count": 2,
            "nodes": [
                {"id": 1, "x": 100, "y": 100},
                {"id": 2, "x": 200, "y": 200}
            ],
            "edges": [
                {"a": 1, "b": 2}
            ],
            "start_positions": [1, 2],
            "win_positions": [2, 1]
        })";

        const std::string path = "test_win_level.json";
        std::ofstream ofs(path);
        ofs << levelJson;
        ofs.close();

        levelData = core::LevelParser::load(path);
        std::filesystem::remove(path);
    }

    core::LevelData levelData;
};

TEST_F(GameStateGTest, WinConditionTrueWhenAllPiecesOnWinPositions) {
    core::GameState state;
    state.initializeFromLevel(levelData);

    // Move all pieces to their win positions
    // Piece 0: start=1, win=2
    // Piece 1: start=2, win=1
    state.movePiece(0, 2);
    state.movePiece(1, 1);

    EXPECT_TRUE(state.isWinConditionMet());
}

TEST_F(GameStateGTest, WinConditionFalseWhenPiecesMisplaced) {
    core::GameState state;
    state.initializeFromLevel(levelData);

    // Swap: piece 0 on win position 1 (should be on 2)
    // piece 1 on win position 2 (should be on 1)
    state.movePiece(0, 1);  // Not on win position
    state.movePiece(1, 2);  // Not on win position

    EXPECT_FALSE(state.isWinConditionMet());
}

TEST_F(GameStateGTest, WinConditionFalseWhenOnlySomePiecesCorrect) {
    core::GameState state;
    state.initializeFromLevel(levelData);

    // Only piece 0 on correct win position
    state.movePiece(0, 2);
    // Piece 1 stays at start (position 2, but should be on 1)

    EXPECT_FALSE(state.isWinConditionMet());
}

TEST_F(GameStateGTest, WinConditionFalseAtStart) {
    core::GameState state;
    state.initializeFromLevel(levelData);

    // At start, pieces are on start_positions, not win_positions
    // start: [1, 2], win: [2, 1]
    // Unless start == win by coincidence
    // In this test, start != win, so should be false
    EXPECT_FALSE(state.isWinConditionMet());
}
