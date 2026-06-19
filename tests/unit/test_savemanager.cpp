#include <catch2/catch_test_macros.hpp>
#include "SaveManager.hpp"
#include "core/GameState.hpp"
#include "core/LevelParser.hpp"
#include "MoveCommand.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("SaveManager roundtrip without move history", "[savemanager]")
{
    core::LevelData level;
    level.name = "Save Test";
    level.piece_count = 2;
    level.graph.nodes = {
        core::Node{1, 100, 100},
        core::Node{2, 200, 200},
        core::Node{3, 300, 300},
    };
    level.graph.edges = {core::Edge{1, 2}, core::Edge{2, 3}};
    level.graph.buildAdjacency();
    level.start_positions = {1, 2};
    level.win_positions = {3, 1};

    core::GameState state;
    state.initializeFromLevel(level);

    std::vector<game::MoveCommand> emptyHistory;
    const std::string savePath = "test_save_roundtrip.json";

    REQUIRE(game::SaveManager::save(savePath, "levels/test.json", state, emptyHistory));

    std::string loadedPath;
    nlohmann::json j;
    REQUIRE(game::SaveManager::load(savePath, loadedPath, j));

    REQUIRE(loadedPath == "levels/test.json");
    REQUIRE(j["name"] == "Save Test (saved)");
    REQUIRE(j["piece_count"] == 2);
    REQUIRE(j["nodes"].size() == 3);
    REQUIRE(j["edges"].size() == 2);
    REQUIRE(j["start_positions"].size() == 2);
    REQUIRE(j["start_positions"][0] == 1);
    REQUIRE(j["start_positions"][1] == 2);
    REQUIRE(j.contains("move_history"));
    REQUIRE(j["move_history"].empty());
    REQUIRE_FALSE(j.contains("type"));
    REQUIRE_FALSE(j.contains("level_path"));

    std::filesystem::remove(savePath);
}

TEST_CASE("SaveManager roundtrip with move history", "[savemanager]")
{
    core::LevelData level;
    level.name = "History Test";
    level.piece_count = 1;
    level.graph.nodes = {
        core::Node{1, 100, 100},
        core::Node{2, 200, 200},
    };
    level.graph.edges = {core::Edge{1, 2}};
    level.graph.buildAdjacency();
    level.start_positions = {1};
    level.win_positions = {2};

    core::GameState state;
    state.initializeFromLevel(level);
    state.movePiece(0, 2);

    game::MoveCommand cmd;
    cmd.pieceId = 0;
    cmd.fromNodeId = 1;
    cmd.toNodeId = 2;
    std::vector<game::MoveCommand> history = {cmd};

    const std::string savePath = "test_save_history.json";

    REQUIRE(game::SaveManager::save(savePath, "levels/history.json", state, history));

    std::string loadedPath;
    nlohmann::json j;
    REQUIRE(game::SaveManager::load(savePath, loadedPath, j));

    REQUIRE(loadedPath == "levels/history.json");
    REQUIRE(j["start_positions"][0] == 2);
    REQUIRE(j.contains("move_history"));
    REQUIRE(j["move_history"].size() == 1);
    REQUIRE(j["move_history"][0]["pieceId"] == 0);
    REQUIRE(j["move_history"][0]["fromNodeId"] == 1);
    REQUIRE(j["move_history"][0]["toNodeId"] == 2);

    std::filesystem::remove(savePath);
}

TEST_CASE("SaveManager saved JSON is loadable by LevelParser", "[savemanager]")
{
    core::LevelData level;
    level.name = "LevelParser Compat";
    level.piece_count = 1;
    level.graph.nodes = {
        core::Node{1, 100, 100},
        core::Node{2, 200, 200},
    };
    level.graph.edges = {core::Edge{1, 2}};
    level.graph.buildAdjacency();
    level.start_positions = {1};
    level.win_positions = {2};
    level.bg_music = "";
    level.piece_colors = {"#FF0000"};

    core::GameState state;
    state.initializeFromLevel(level);

    const std::string savePath = "test_save_compat.json";
    REQUIRE(game::SaveManager::save(savePath, "levels/compat.json", state, {}));

    // The saved file (minus type/level_path) should parse as a level
    std::string loadedPath;
    nlohmann::json j;
    REQUIRE(game::SaveManager::load(savePath, loadedPath, j));

    core::LevelData reloaded = core::LevelParser::parse(j);
    REQUIRE(reloaded.name == "LevelParser Compat (saved)");
    REQUIRE(reloaded.piece_count == 1);
    REQUIRE(reloaded.graph.nodes.size() == 2);
    REQUIRE(reloaded.graph.edges.size() == 1);
    REQUIRE(reloaded.start_positions.size() == 1);
    REQUIRE(reloaded.start_positions[0] == 1);
    REQUIRE(reloaded.win_positions.size() == 1);
    REQUIRE(reloaded.win_positions[0] == 2);
    REQUIRE(reloaded.bg_music == "");
    REQUIRE(reloaded.piece_colors.size() == 1);
    REQUIRE(reloaded.piece_colors[0] == "#FF0000");

    std::filesystem::remove(savePath);
}

TEST_CASE("SaveManager load returns false on missing file", "[savemanager]")
{
    std::string path;
    nlohmann::json j;
    REQUIRE_FALSE(game::SaveManager::load("nonexistent_save.json", path, j));
}

TEST_CASE("SaveManager load returns false on non-save JSON", "[savemanager]")
{
    const std::string path = "test_not_save.json";
    {
        std::ofstream f(path);
        f << R"({"name":"regular level","piece_count":1})";
    }
    std::string loadedPath;
    nlohmann::json j;
    REQUIRE_FALSE(game::SaveManager::load(path, loadedPath, j));
    std::filesystem::remove(path);
}

TEST_CASE("SaveManager load returns false on corrupted JSON", "[savemanager]")
{
    const std::string path = "test_corrupt_save.json";
    {
        std::ofstream f(path);
        f << "not json at all";
    }
    std::string loadedPath;
    nlohmann::json j;
    REQUIRE_FALSE(game::SaveManager::load(path, loadedPath, j));
    std::filesystem::remove(path);
}
