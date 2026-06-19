#include "SaveManager.hpp"
#include "MoveCommand.hpp"
#include "core/GameState.hpp"
#include <fstream>
#include <iostream>

namespace game
{

bool SaveManager::save(const std::string& path,
                       const std::string& levelPath,
                       const core::GameState& state,
                       const std::vector<MoveCommand>& moveHistory)
{
    try
    {
        nlohmann::json j;
        j["type"] = "save";
        j["level_path"] = levelPath;
        j["name"] = state.level.name + " (saved)";
        j["piece_count"] = state.level.piece_count;
        j["bg_music"] = state.level.bg_music;
        j["piece_colors"] = state.level.piece_colors;

        j["nodes"] = nlohmann::json::array();
        for (const auto& node : state.level.graph.nodes)
        {
            nlohmann::json n;
            n["id"] = node.id; n["x"] = node.x; n["y"] = node.y;
            j["nodes"].push_back(n);
        }

        j["edges"] = nlohmann::json::array();
        for (const auto& edge : state.level.graph.edges)
        {
            nlohmann::json e;
            e["a"] = edge.a; e["b"] = edge.b;
            j["edges"].push_back(e);
        }

        nlohmann::json startPos = nlohmann::json::array();
        for (const auto& piece : state.pieces)
            startPos.push_back(piece.currentNodeId);
        j["start_positions"] = startPos;
        j["win_positions"] = state.level.win_positions;

        nlohmann::json history = nlohmann::json::array();
        for (const auto& cmd : moveHistory)
        {
            nlohmann::json h;
            h["pieceId"] = cmd.pieceId;
            h["fromNodeId"] = cmd.fromNodeId;
            h["toNodeId"] = cmd.toNodeId;
            history.push_back(h);
        }
        j["move_history"] = history;

        std::ofstream file(path);
        if (!file.is_open()) return false;
        file << j.dump(2);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Save error: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::load(const std::string& path,
                       std::string& outLevelPath,
                       nlohmann::json& outLevelJson)
{
    try
    {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        nlohmann::json j;
        file >> j;

        if (!j.contains("type") || j["type"] != "save")
            return false;

        outLevelPath = j["level_path"].get<std::string>();
        j.erase("type");
        j.erase("level_path");
        outLevelJson = j;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Load save error: " << e.what() << std::endl;
        return false;
    }
}

}
