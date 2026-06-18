#pragma once

#include "Graph.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace core
{

class PathFinder
{
public:
    static std::vector<int> bfs(const Graph& graph, int startNodeId,
                                 const std::unordered_set<int>& occupiedNodeIds);

    static std::vector<int> bfsPath(const Graph& graph, int startNodeId, int targetNodeId,
                                     const std::unordered_set<int>& occupiedNodeIds);
};

}
