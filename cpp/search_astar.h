#ifndef SEARCH_ASTAR_H
#define SEARCH_ASTAR_H

#include "graph.h"
#include <vector>
#include <string>

// Kevin Yu: Implementing A* search algorithm

class SearchAStar {
public:
    static std::vector<std::string> astar(const Graph& graph, const std::string& startNode, const std::string& endNode);
};

#endif