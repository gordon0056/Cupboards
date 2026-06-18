#pragma once

#include <vector>
#include <unordered_map>

namespace core
{

struct Node
{
    int id;
    float x;
    float y;
};

struct Edge
{
    int a;
    int b;
};

class Graph
{
public:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::unordered_map<int, std::vector<int>> adjacency;

    void buildAdjacency();
    bool hasNode(int id) const;
    const Node* getNode(int id) const;
};

}