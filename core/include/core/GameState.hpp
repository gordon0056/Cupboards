#pragma once

#include "LevelParser.hpp"
#include <vector>
#include <optional>
#include <string>

namespace core
{

struct Piece
{
    int id = 0;
    int currentNodeId = 0;
    std::string color = "#FFFFFF";
    bool isOccupied = true;
};

class GameState
{
public:
    LevelData level;
    std::vector<Piece> pieces;
    std::optional<int> selectedPieceId;

    bool isWinConditionMet() const;
    std::vector<int> getReachableNodes(int pieceId) const;
    void movePiece(int pieceId, int targetNodeId);
    void initializeFromLevel(const LevelData& levelData,
                              const std::vector<std::string>& defaultColors = {});
};

}
