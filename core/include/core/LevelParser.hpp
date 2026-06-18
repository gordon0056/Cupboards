#pragma once

#include "Graph.hpp"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace core
{

struct LevelData
{
    std::string name;
    int piece_count = 0;
    Graph graph;
    std::vector<int> start_positions;
    std::vector<int> win_positions;
    std::string bg_music;
    std::vector<std::string> piece_colors;
};

class LevelParser
{
public:
    static LevelData load(const std::string& path);
    static LevelData parse(const nlohmann::json& json);
};

}
