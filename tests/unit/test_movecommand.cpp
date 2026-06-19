#include <catch2/catch_test_macros.hpp>
#include "MoveCommand.hpp"
#include "core/GameState.hpp"

TEST_CASE("MoveCommand executes and undoes", "[movecommand]") {
    core::LevelData level;
    level.piece_count = 1;
    level.graph.nodes = {{1, 100, 100}, {2, 200, 200}};
    level.graph.buildAdjacency();
    level.start_positions = {1};
    level.win_positions = {2};

    core::GameState gs;
    gs.initializeFromLevel(level);

    game::MoveCommand cmd;
    cmd.pieceId = 0;
    cmd.fromNodeId = 1;
    cmd.toNodeId = 2;

    REQUIRE(gs.pieces[0].currentNodeId == 1);

    cmd.execute(gs);
    REQUIRE(gs.pieces[0].currentNodeId == 2);

    cmd.undo(gs);
    REQUIRE(gs.pieces[0].currentNodeId == 1);
}
