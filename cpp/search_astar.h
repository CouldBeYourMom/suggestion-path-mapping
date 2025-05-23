#ifndef SEARCH_ASTAR_H
#define SEARCH_ASTAR_H

#include "graph.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <utility>


// Kevin Yu: Implementing A* search algorithm

// Returns a list of (video ID, elapsed time timestamp)
std::vector<std::tuple<std::string, std::string, double>>  runAStar(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
);


#endif