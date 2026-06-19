#include <catch2/catch_test_macros.hpp>
#include "core/GameState.hpp"
#include <algorithm>

TEST_CASE("GameState detects win condition", "[gamestate]") {
    core::LevelData level;
    level.name = "Test";
    level.piece_count = 2;
    level.graph.nodes = {core::Node{1, 100, 100}, core::Node{2, 200, 200}};
    level.graph.buildAdjacency();
    level.start_positions = {1, 2};
    level.win_positions = {2, 1};

    core::GameState gs;
    gs.initializeFromLevel(level);

    REQUIRE_FALSE(gs.isWinConditionMet());

    // Move pieces to win positions
    gs.movePiece(0, 2);
    gs.movePiece(1, 1);

    REQUIRE(gs.isWinConditionMet());
}

TEST_CASE("GameState getReachableNodes respects occupied", "[gamestate]") {
    core::LevelData level;
    level.piece_count = 3;
    level.graph.nodes = {
        core::Node{1, 100, 100}, core::Node{2, 200, 100},
        core::Node{3, 300, 100}, core::Node{4, 400, 100}
    };
    level.graph.edges = {
        core::Edge{1, 2}, core::Edge{2, 3}, core::Edge{3, 4}
    };
    level.graph.buildAdjacency();
    level.start_positions = {1, 2, 3};
    level.win_positions = {2, 3, 4};

    core::GameState gs;
    gs.initializeFromLevel(level);

    // Piece 0 at node 1, pieces 1 and 2 at nodes 2 and 3
    auto reachable = gs.getReachableNodes(0);

    // Node 2 is occupied by piece 1, so piece 0 can't reach it
    REQUIRE(std::find(reachable.begin(), reachable.end(), 2) == reachable.end());
}
