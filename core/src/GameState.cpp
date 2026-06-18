#include "core/GameState.hpp"
#include "core/PathFinder.hpp"
#include <stdexcept>

namespace core
{

bool GameState::isWinConditionMet() const
{
    if (static_cast<int>(pieces.size()) != level.piece_count)
    {
        return false;
    }
    if (static_cast<int>(level.win_positions.size()) < level.piece_count)
    {
        return false;
    }
    for (int i = 0; i < level.piece_count; ++i)
    {
        if (pieces[i].currentNodeId != level.win_positions[i])
        {
            return false;
        }
    }
    return true;
}

std::vector<int> GameState::getReachableNodes(int pieceId) const
{
    if (pieceId < 0 || pieceId >= static_cast<int>(pieces.size()))
    {
        return {};
    }

    const Piece& piece = pieces[pieceId];

    std::unordered_set<int> occupied;
    for (const auto& p : pieces)
    {
        if (p.id != pieceId && p.isOccupied)
        {
            occupied.insert(p.currentNodeId);
        }
    }

    return PathFinder::bfs(level.graph, piece.currentNodeId, occupied);
}

void GameState::movePiece(int pieceId, int targetNodeId)
{
    if (pieceId < 0 || pieceId >= static_cast<int>(pieces.size()))
    {
        throw std::runtime_error("Invalid piece ID: " + std::to_string(pieceId));
    }
    if (!level.graph.hasNode(targetNodeId))
    {
        throw std::runtime_error("Invalid target node ID: " + std::to_string(targetNodeId));
    }

    pieces[pieceId].currentNodeId = targetNodeId;
}

void GameState::initializeFromLevel(const LevelData& levelData,
                                      const std::vector<std::string>& defaultColors)
{
    level = levelData;
    pieces.clear();

    for (int i = 0; i < levelData.piece_count; ++i)
    {
        Piece piece;
        piece.id = i;
        piece.currentNodeId = levelData.start_positions[i];
        piece.isOccupied = true;

        if (i < static_cast<int>(levelData.piece_colors.size()))
        {
            piece.color = levelData.piece_colors[i];
        }
        else if (i < static_cast<int>(defaultColors.size()))
        {
            piece.color = defaultColors[i];
        }
        else
        {
            piece.color = "#FFFFFF";
        }

        pieces.push_back(piece);
    }

    selectedPieceId.reset();
}

}
