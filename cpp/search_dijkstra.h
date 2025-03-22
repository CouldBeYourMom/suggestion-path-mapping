#ifndef SEARCH_DIJKSTRA_H
#define SEARCH_DIJKSTRA_H

#include "graph.h"
#include <vector>
#include <string>

class SearchDijkstra {
public:
    static std::vector<std::string> dijkstra(const Graph& graph, const std::string& startNode, const std::string& endNode);
};

#endif
