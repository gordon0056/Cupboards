#include <catch2/catch_test_macros.hpp>
#include "core/LevelParser.hpp"

TEST_CASE("LevelParser loads valid JSON", "[levelparser]") {
    core::LevelData data = core::LevelParser::parse(R"({
        "name": "Test",
        "piece_count": 2,
        "nodes": [
            {"id": 1, "x": 100, "y": 100},
            {"id": 2, "x": 200, "y": 200}
        ],
        "edges": [{"a": 1, "b": 2}],
        "start_positions": [1, 2],
        "win_positions": [2, 1]
    })"_json);

    REQUIRE(data.name == "Test");
    REQUIRE(data.piece_count == 2);
    REQUIRE(data.graph.nodes.size() == 2);
    REQUIRE(data.graph.edges.size() == 1);
    REQUIRE(data.start_positions.size() == 2);
    REQUIRE(data.win_positions.size() == 2);
    REQUIRE(data.graph.adjacency.size() == 2);
}
