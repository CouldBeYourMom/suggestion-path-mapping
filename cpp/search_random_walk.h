#ifndef SEARCH_RANDOM_WALK_H
#define SEARCH_RANDOM_WALK_H

#include "graph.h"
#include <string>
#include <vector>
#include <utility>

// Returns a list of (video ID, elapsed time timestamp)
std::vector<std::pair<std::string, double>> runRandomWalk(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
);

#endif