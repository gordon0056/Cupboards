#include "core/PathFinder.hpp"
#include <queue>
#include <algorithm>

namespace core
{

std::vector<int> PathFinder::bfs(const Graph& graph, int startNodeId,
                                   const std::unordered_set<int>& occupiedNodeIds)
{
    std::vector<int> reachable;

    if (!graph.hasNode(startNodeId))
    {
        return reachable;
    }

    std::unordered_set<int> visited;
    std::queue<int> q;

    q.push(startNodeId);
    visited.insert(startNodeId);

    while (!q.empty())
    {
        int current = q.front();
        q.pop();

        auto it = graph.adjacency.find(current);
        if (it == graph.adjacency.end())
        {
            continue;
        }

        for (int neighbor : it->second)
        {
            if (visited.count(neighbor) > 0) continue;
            if (occupiedNodeIds.count(neighbor) > 0) continue;

            visited.insert(neighbor);
            reachable.push_back(neighbor);
            q.push(neighbor);
        }
    }

    return reachable;
}

std::vector<int> PathFinder::bfsPath(const Graph& graph, int startNodeId, int targetNodeId,
                                       const std::unordered_set<int>& occupiedNodeIds)
{
    if (!graph.hasNode(startNodeId) || !graph.hasNode(targetNodeId))
    {
        return {};
    }
    if (startNodeId == targetNodeId)
    {
        return {startNodeId};
    }

    std::unordered_set<int> visited;
    std::queue<int> q;
    std::unordered_map<int, int> parent;

    q.push(startNodeId);
    visited.insert(startNodeId);
    parent[startNodeId] = -1;

    while (!q.empty())
    {
        int current = q.front();
        q.pop();

        if (current == targetNodeId)
        {
            std::vector<int> path;
            for (int v = targetNodeId; v != -1; v = parent[v])
            {
                path.push_back(v);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        auto it = graph.adjacency.find(current);
        if (it == graph.adjacency.end()) continue;

        for (int neighbor : it->second)
        {
            if (visited.count(neighbor) > 0) continue;
            if (occupiedNodeIds.count(neighbor) > 0 &&
                neighbor != targetNodeId) continue;

            visited.insert(neighbor);
            parent[neighbor] = current;
            q.push(neighbor);
        }
    }

    return {};
}

}