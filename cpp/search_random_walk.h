#ifndef SEARCH_RANDOM_WALK_H
#define SEARCH_RANDOM_WALK_H

#include "graph.h"
#include <vector>
#include <string>

class SearchRandomWalk {
public:
    static std::vector<std::string> randomWalk(const Graph& graph, const std::string& startNode, int steps);
};

#endif