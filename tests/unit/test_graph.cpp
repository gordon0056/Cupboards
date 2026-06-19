#include <catch2/catch_test_macros.hpp>
#include "core/Graph.hpp"

TEST_CASE("Graph builds adjacency list correctly", "[graph]") {
    core::Graph graph;

    core::Node n1{1, 100.0f, 100.0f};
    core::Node n2{2, 200.0f, 100.0f};
    core::Node n3{3, 300.0f, 100.0f};

    graph.nodes = {n1, n2, n3};
    graph.edges = {{1, 2}, {2, 3}};
    graph.buildAdjacency();

    REQUIRE(graph.hasNode(1));
    REQUIRE(graph.hasNode(2));
    REQUIRE(graph.hasNode(3));
    REQUIRE_FALSE(graph.hasNode(99));

    REQUIRE(graph.adjacency.at(1).size() == 1);
    REQUIRE(graph.adjacency.at(2).size() == 2);
    REQUIRE(graph.adjacency.at(3).size() == 1);
}

TEST_CASE("Graph getNode returns correct node", "[graph]") {
    core::Graph graph;
    graph.nodes = {core::Node{1, 10.0f, 20.0f}, core::Node{2, 30.0f, 40.0f}};
    graph.buildAdjacency();

    const core::Node* node = graph.getNode(1);
    REQUIRE(node != nullptr);
    REQUIRE(node->x == 10.0f);
    REQUIRE(node->y == 20.0f);

    REQUIRE(graph.getNode(99) == nullptr);
}
