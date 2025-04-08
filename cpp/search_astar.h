#ifndef SEARCH_ASTAR_H
#define SEARCH_ASTAR_H

#include "graph.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>


// Kevin Yu: Implementing A* search algorithm

class SearchAStar {
  // Keep track of f values and g values for each node.
    static std::unordered_map<std::string, float> gValues;
    static std::unordered_map<std::string, float> fValues;
public:
    static float heuristic(const std::string& startNode);
    static std::vector<std::string> astar(const Graph& graph, const std::string& startNode, const std::string& endNode);
    std::vector<std::pair<std::string, double>> runAStar(
        const Graph& g,
        const std::string& startNode,
        int maxNodesToVisit,
        const std::string& weightType
    );
};

#endif