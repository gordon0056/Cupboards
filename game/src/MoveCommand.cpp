#include "MoveCommand.hpp"
#include "core/GameState.hpp"

namespace game
{

void MoveCommand::execute(core::GameState& state)
{
    state.movePiece(pieceId, toNodeId);
}

void MoveCommand::undo(core::GameState& state)
{
    state.movePiece(pieceId, fromNodeId);
}

}
