#include "core/LevelParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <set>

namespace core
{

static std::string readFileAsString(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

LevelData LevelParser::load(const std::string& path)
{
    std::string content = readFileAsString(path);
    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(content);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        throw std::runtime_error("Invalid JSON: " + std::string(e.what()));
    }
    return parse(json);
}

LevelData LevelParser::parse(const nlohmann::json& json)
{
    LevelData data;

    auto requireField = [&json](const std::string& name)
    {
        if (!json.contains(name))
        {
            throw std::runtime_error("Missing field: " + name);
        }
    };

    requireField("name");
    requireField("piece_count");
    requireField("nodes");
    requireField("edges");
    requireField("start_positions");
    requireField("win_positions");

    data.name = json["name"].get<std::string>();
    data.piece_count = json["piece_count"].get<int>();
    data.start_positions = json["start_positions"].get<std::vector<int>>();
    data.win_positions = json["win_positions"].get<std::vector<int>>();

    if (data.name.length() > 256)
    {
        throw std::runtime_error("Level name too long");
    }
    if (data.piece_count < 1 || data.piece_count > 64)
    {
        throw std::runtime_error("piece_count out of range (1-64)");
    }
    if (json["nodes"].size() > 2048)
    {
        throw std::runtime_error("Too many nodes (max 2048)");
    }
    if (json["edges"].size() > 4096)
    {
        throw std::runtime_error("Too many edges (max 4096)");
    }

    if (static_cast<int>(data.start_positions.size()) != data.piece_count)
    {
        throw std::runtime_error("piece_count mismatch with start_positions size");
    }
    if (static_cast<int>(data.win_positions.size()) != data.piece_count)
    {
        throw std::runtime_error("piece_count mismatch with win_positions size");
    }

    std::set<int> nodeIds;
    for (const auto& nodeJson : json["nodes"])
    {
        if (!nodeJson.contains("id") || !nodeJson.contains("x") || !nodeJson.contains("y"))
        {
            throw std::runtime_error("Node missing required field (id, x, y)");
        }
        int id = nodeJson["id"].get<int>();
        if (nodeIds.count(id) > 0)
        {
            throw std::runtime_error("Duplicate node ID: " + std::to_string(id));
        }
        nodeIds.insert(id);
        Node node;
        node.id = id;
        node.x = nodeJson["x"].get<float>();
        node.y = nodeJson["y"].get<float>();
        data.graph.nodes.push_back(node);
    }

    for (const auto& edgeJson : json["edges"])
    {
        if (!edgeJson.contains("a") || !edgeJson.contains("b"))
        {
            throw std::runtime_error("Edge missing required field (a, b)");
        }
        int a = edgeJson["a"].get<int>();
        int b = edgeJson["b"].get<int>();
        if (nodeIds.find(a) == nodeIds.end())
        {
            throw std::runtime_error("Edge references unknown node: " + std::to_string(a));
        }
        if (nodeIds.find(b) == nodeIds.end())
        {
            throw std::runtime_error("Edge references unknown node: " + std::to_string(b));
        }
        Edge edge;
        edge.a = a;
        edge.b = b;
        data.graph.edges.push_back(edge);
    }

    auto validatePositionIds = [&nodeIds, &data](const std::vector<int>& positions, const std::string& fieldName)
    {
        for (int id : positions)
        {
            if (nodeIds.find(id) == nodeIds.end())
            {
                throw std::runtime_error("Invalid position ID in " + fieldName + ": " + std::to_string(id));
            }
        }
    };
    validatePositionIds(data.start_positions, "start_positions");
    validatePositionIds(data.win_positions, "win_positions");

    data.graph.buildAdjacency();

    if (json.contains("bg_music"))
    {
        data.bg_music = json["bg_music"].get<std::string>();
        if (data.bg_music.find("..") != std::string::npos ||
            data.bg_music.find('/') != std::string::npos ||
            data.bg_music.find('\\') != std::string::npos)
        {
            throw std::runtime_error("Invalid bg_music path: " + data.bg_music);
        }
    }
    if (json.contains("piece_colors"))
    {
        data.piece_colors = json["piece_colors"].get<std::vector<std::string>>();
    }

    return data;
}

}
