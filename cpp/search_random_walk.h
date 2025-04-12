#ifndef SEARCH_RANDOM_WALK_H
#define SEARCH_RANDOM_WALK_H

#include "graph.h"
#include <vector>
#include <string>

// Teammate 3: Simplified Random Walk Search Implementation
class SearchRandomWalk {
public:
    // Performs a random walk on the graph starting from 'startNode' for 'steps' steps.
    // Returns the list of nodes visited along the walk.
    static std::vector<std::string> randomWalk(const Graph& graph, const std::string& startNode, int steps);
};

#endif
