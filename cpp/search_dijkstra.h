#ifndef SEARCH_DIJKSTRA_H
#define SEARCH_DIJKSTRA_H

#include "graph.h"
#include <string>
#include <vector>
#include <utility>

// Returns a list of (video ID, elapsed time timestamp)
std::vector<std::tuple<std::string, std::string, double>> runDijkstra(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
);

#endif