#include "core/Graph.hpp"
#include <algorithm>

namespace core
{

void Graph::buildAdjacency()
{
    adjacency.clear();
    for (const auto& node : nodes)
    {
        adjacency[node.id] = {};
    }
    for (const auto& edge : edges)
    {
        adjacency[edge.a].push_back(edge.b);
        adjacency[edge.b].push_back(edge.a);
    }
}

bool Graph::hasNode(int id) const
{
    return adjacency.find(id) != adjacency.end();
}

const Node* Graph::getNode(int id) const
{
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [id](const Node& n) { return n.id == id; });
    if (it != nodes.end())
    {
        return &(*it);
    }
    return nullptr;
}

}
