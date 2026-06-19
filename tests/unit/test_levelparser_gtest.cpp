#include <gtest/gtest.h>
#include "core/LevelParser.hpp"
#include <fstream>
#include <filesystem>

// T053: JSON validation — valid and corrupted JSON

TEST(LevelParserGTest, LoadsValidJsonSuccessfully) {
    const std::string validJson = R"({
        "name": "Test Level",
        "piece_count": 2,
        "nodes": [
            {"id": 1, "x": 100, "y": 100},
            {"id": 2, "x": 200, "y": 200}
        ],
        "edges": [
            {"a": 1, "b": 2}
        ],
        "start_positions": [1, 2],
        "win_positions": [2, 1]
    })";

    const std::string path = "test_valid_temp.json";
    std::ofstream ofs(path);
    ofs << validJson;
    ofs.close();

    EXPECT_NO_THROW({
        core::LevelData data = core::LevelParser::load(path);
        EXPECT_EQ(data.name, "Test Level");
        EXPECT_EQ(data.piece_count, 2);
        EXPECT_EQ(data.graph.nodes.size(), 2u);
        EXPECT_EQ(data.graph.edges.size(), 1u);
    });

    std::filesystem::remove(path);
}

TEST(LevelParserGTest, ThrowsOnCorruptedJson) {
    const std::string corruptedJson = "{ this is not valid JSON!!!";

    const std::string path = "test_corrupted_temp.json";
    std::ofstream ofs(path);
    ofs << corruptedJson;
    ofs.close();

    EXPECT_THROW({
        core::LevelParser::load(path);
    }, std::runtime_error);

    std::filesystem::remove(path);
}

TEST(LevelParserGTest, ThrowsOnMissingFile) {
    EXPECT_THROW({
        core::LevelParser::load("nonexistent_file_xyz.json");
    }, std::runtime_error);
}

TEST(LevelParserGTest, ThrowsOnMissingRequiredFields) {
    const std::string incompleteJson = R"({
        "name": "Incomplete",
        "piece_count": 1
    })";

    const std::string path = "test_incomplete_temp.json";
    std::ofstream ofs(path);
    ofs << incompleteJson;
    ofs.close();

    EXPECT_THROW({
        core::LevelParser::load(path);
    }, std::runtime_error);

    std::filesystem::remove(path);
}
