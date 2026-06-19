#include <gtest/gtest.h>
#include "core/PathFinder.hpp"
#include "core/Graph.hpp"
#include <unordered_set>

// T054-T056: Pathfinding edge cases

TEST(PathFinderGTest, BfsOnEmptyGraphReturnsEmpty) {
    core::Graph graph;
    // No nodes, no edges

    std::unordered_set<int> occupied;
    // Can't call bfs with invalid nodeId on empty graph
    // Just verify it doesn't crash — bfs will return empty for non-existent node
    std::vector<int> reachable = core::PathFinder::bfs(graph, 1, occupied);

    EXPECT_TRUE(reachable.empty());
}

TEST(PathFinderGTest, BfsWithAllNeighborsOccupiedReturnsEmpty) {
    core::Graph graph;

    // Linear graph: 1-2-3
    graph.nodes.push_back({1, 0, 0});
    graph.nodes.push_back({2, 10, 0});
    graph.nodes.push_back({3, 20, 0});

    graph.edges.push_back({1, 2});
    graph.edges.push_back({2, 3});

    graph.buildAdjacency();

    // Node 2 is occupied, so from node 1 we can't reach anything
    std::unordered_set<int> occupied = {2};
    std::vector<int> reachable = core::PathFinder::bfs(graph, 1, occupied);

    // Node 1's only neighbor is node 2, which is occupied
    EXPECT_TRUE(reachable.empty());
}

TEST(PathFinderGTest, BfsFindsShortestPath) {
    core::Graph graph;

    // Graph: 1-2-3-4-5 (linear)
    for (int i = 1; i <= 5; ++i) {
        graph.nodes.push_back({i, static_cast<float>(i * 10), 0});
    }
    for (int i = 1; i < 5; ++i) {
        graph.edges.push_back({i, i + 1});
    }
    graph.buildAdjacency();

    // No nodes occupied, BFS from node 1 should find 2, 3, 4, 5
    std::unordered_set<int> occupied;
    std::vector<int> reachable = core::PathFinder::bfs(graph, 1, occupied);

    ASSERT_EQ(reachable.size(), 4u);
    EXPECT_EQ(reachable[0], 2);
    EXPECT_EQ(reachable[1], 3);
    EXPECT_EQ(reachable[2], 4);
    EXPECT_EQ(reachable[3], 5);
}

TEST(PathFinderGTest, BfsRespectsOccupiedNodesInMiddle) {
    core::Graph graph;

    // Graph: 1-2-3-4
    for (int i = 1; i <= 4; ++i) {
        graph.nodes.push_back({i, static_cast<float>(i * 10), 0});
    }
    graph.edges.push_back({1, 2});
    graph.edges.push_back({2, 3});
    graph.edges.push_back({3, 4});
    graph.buildAdjacency();

    // Node 3 is occupied, so from node 1 we can only reach node 2
    std::unordered_set<int> occupied = {3};
    std::vector<int> reachable = core::PathFinder::bfs(graph, 1, occupied);

    ASSERT_EQ(reachable.size(), 1u);
    EXPECT_EQ(reachable[0], 2);
}

TEST(PathFinderGTest, BfsOnDisconnectedGraph) {
    core::Graph graph;

    // Two disconnected components: {1,2} and {3,4}
    graph.nodes.push_back({1, 0, 0});
    graph.nodes.push_back({2, 10, 0});
    graph.nodes.push_back({3, 100, 0});
    graph.nodes.push_back({4, 110, 0});

    graph.edges.push_back({1, 2});
    graph.edges.push_back({3, 4});

    graph.buildAdjacency();

    // From node 1, we can only reach node 2
    std::unordered_set<int> occupied;
    std::vector<int> reachable = core::PathFinder::bfs(graph, 1, occupied);

    ASSERT_EQ(reachable.size(), 1u);
    EXPECT_EQ(reachable[0], 2);
}
