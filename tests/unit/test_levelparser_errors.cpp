#include <catch2/catch_test_macros.hpp>
#include "core/LevelParser.hpp"

TEST_CASE("LevelParser throws on missing fields", "[levelparser]") {
    REQUIRE_THROWS_AS(core::LevelParser::parse(R"({
        "name": "Test",
        "piece_count": 2
    })"_json), std::runtime_error);
}

TEST_CASE("LevelParser throws on invalid JSON structure", "[levelparser]") {
    REQUIRE_THROWS_AS(core::LevelParser::parse(R"({
        "name": "Test",
        "piece_count": 2,
        "nodes": [
            {"id": 1, "x": 100, "y": 100}
        ],
        "edges": [],
        "start_positions": [1, 2],
        "win_positions": [1, 2]
    })"_json), std::runtime_error); // piece_count mismatch
}

TEST_CASE("LevelParser throws on duplicate node IDs", "[levelparser]") {
    REQUIRE_THROWS_AS(core::LevelParser::parse(R"({
        "name": "Test",
        "piece_count": 1,
        "nodes": [
            {"id": 1, "x": 100, "y": 100},
            {"id": 1, "x": 200, "y": 200}
        ],
        "edges": [],
        "start_positions": [1],
        "win_positions": [1]
    })"_json), std::runtime_error);
}
