#ifndef SEARCH_ASTAR_H
#define SEARCH_ASTAR_H

#include "graph.h"
#include <string>
#include <vector>
#include <utility>

// Returns a list of (video ID, elapsed time timestamp)
std::vector<std::pair<std::string, double>> runAStar(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
);

#endif