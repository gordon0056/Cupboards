#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace core
{
class GameState;
}

namespace game
{

struct MoveCommand;

class SaveManager
{
public:
    static bool save(const std::string& path,
                     const std::string& levelPath,
                     const core::GameState& state,
                     const std::vector<MoveCommand>& moveHistory);

    static bool load(const std::string& path,
                     std::string& outLevelPath,
                     nlohmann::json& outLevelJson);
};

}
