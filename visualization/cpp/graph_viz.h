#ifndef GRAPH_VIZ_H
#define GRAPH_VIZ_H

#include <string>
#include "graph.h"
#include "external/json.hpp"
using json = nlohmann::json;

// Full graph export for browser viewer
void exportFullGraphToJSON(const Graph& graph, const std::string& filename);

// Search result exports (both path formats)
void exportTraversalToJSON(const Graph& graph,
    const std::vector<std::pair<std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode);

void exportTraversalToJSON(const Graph& graph,
    const std::vector<std::tuple<std::string, std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode);

// Wrapper to export + copy to viewer file
void exportAndPrepareForViewer(const Graph& graph,
    const std::vector<std::pair<std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode);

void exportAndPrepareForViewer(const Graph& graph,
    const std::vector<std::tuple<std::string, std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode);

// Copies output to visualization/search.json
void copyTraversalToSearchJSON(const std::string& searchType, const std::string& stat);

// Maps numeric flag count to severity group
std::string classifyFlagLevel(int flags);

#endif