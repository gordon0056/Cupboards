#include <catch2/catch_test_macros.hpp>
#include "core/PathFinder.hpp"
#include "core/Graph.hpp"
#include <unordered_set>
#include <algorithm>

TEST_CASE("BFS finds reachable nodes ignoring occupied", "[pathfinder]") {
    core::Graph graph;
    graph.nodes = {
        core::Node{1, 100, 100}, core::Node{2, 200, 100}, core::Node{3, 300, 100},
        core::Node{4, 100, 200}, core::Node{5, 200, 200}, core::Node{6, 300, 200}
    };
    graph.edges = {
        core::Edge{1, 2}, core::Edge{2, 3},
        core::Edge{1, 4}, core::Edge{2, 5}, core::Edge{3, 6},
        core::Edge{4, 5}, core::Edge{5, 6}
    };
    graph.buildAdjacency();

    // Node 2 is occupied
    std::unordered_set<int> occupied = {2};

    auto reachable = core::PathFinder::bfs(graph, 1, occupied);

    // From 1, we can go to 4 (not to 2 since it's occupied)
    // From 4 we can go to 5, then 3 and 6
    REQUIRE(std::find(reachable.begin(), reachable.end(), 4) != reachable.end());
    REQUIRE(std::find(reachable.begin(), reachable.end(), 5) != reachable.end());
    // Node 2 should NOT be reachable (occupied)
    REQUIRE(std::find(reachable.begin(), reachable.end(), 2) == reachable.end());
}

TEST_CASE("BFS returns empty when all neighbors occupied", "[pathfinder]") {
    core::Graph graph;
    graph.nodes = {core::Node{1, 100, 100}, core::Node{2, 200, 100}, core::Node{3, 300, 100}};
    graph.edges = {core::Edge{1, 2}, core::Edge{1, 3}};
    graph.buildAdjacency();

    std::unordered_set<int> occupied = {2, 3};
    auto reachable = core::PathFinder::bfs(graph, 1, occupied);

    REQUIRE(reachable.empty());
}
