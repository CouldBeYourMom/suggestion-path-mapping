#include "graph_viz.h"
#include "external/json.hpp"
#include <sqlite3.h>
#include <fstream>
#include <iostream>
#include <unordered_set>

using json = nlohmann::json;

/*--------------------------------------------------
  Export Full Graph to JSON (for ForceGraph3D)
----------------------------------------------------
  Includes all nodes from videoStatsMap and links 
  from adjList. Used for rendering the full network.
--------------------------------------------------*/
void exportFullGraphToJSON(const Graph& graph, const std::string& filename) {
    json output;
    json nodes = json::array();
    json links = json::array();

    const auto& statsMap = graph.getVideoStatsMap();
    for (const auto& [videoId, stats] : statsMap) {
        json nodeJson = {
            {"id", videoId},
            {"title", stats.title},
            {"flag_count", stats.flagCount},
            {"like_count", stats.likeCount},
            {"view_count", stats.viewCount},
            {"comment_count", stats.commentCount},
            {"group", classifyFlagLevel(stats.flagCount)}
        };
        nodes.push_back(nodeJson);
    }

    const auto& adj = graph.getAdjList();
    for (const auto& [parent, suggestions] : adj) {
        for (const auto& [suggested, _] : suggestions) {
            json linkJson = {
                {"source", parent},
                {"target", suggested}
            };
            links.push_back(linkJson);
        }
    }

    output["nodes"] = nodes;
    output["links"] = links;

    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << std::setw(2) << output << std::endl;
    } else {
        std::cerr << "[ERROR] Could not open file: " << filename << std::endl;
    }
}

/*--------------------------------------------------
  Export Traversal Path (for default pair format)
--------------------------------------------------*/
void exportTraversalToJSON(
    const Graph& graph,
    const std::vector<std::pair<std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode)
{
    std::unordered_set<std::string> visitedSet;
    std::unordered_set<std::string> traversedEdges;

    for (const auto& [node, _] : path)
        visitedSet.insert(node);

    for (size_t i = 1; i < path.size(); ++i)
        traversedEdges.insert(path[i - 1].first + "->" + path[i].first);

    json exportJson = {
        {"nodes", json::array()},
        {"links", json::array()}
    };

    for (const auto& [node, _] : path) {
        json nodeJson = {
            {"id", node},
            {"visited", true}
        };
        if (node == startNode) nodeJson["start"] = true;
        exportJson["nodes"].push_back(nodeJson);
    }

    for (const auto& [parent, suggestions] : graph.getAdjList()) {
        if (!visitedSet.count(parent)) continue;
        for (const auto& [child, _] : suggestions) {
            if (!visitedSet.count(child)) continue;
            bool traversed = traversedEdges.count(parent + "->" + child);
            exportJson["links"].push_back({
                {"source", parent},
                {"target", child},
                {"traversed", traversed}
            });
        }
    }

    std::string filename = "visualization/graph_" + searchType + "_" + stat + ".json";
    std::ofstream out(filename);
    if (out.is_open()) {
        out << exportJson.dump(2);
    } else {
        std::cerr << "[ERROR] Could not write file: " << filename << std::endl;
    }
}

/*--------------------------------------------------
  Export Traversal Path (for tuple w/ parent tracking)
--------------------------------------------------*/
void exportTraversalToJSON(
    const Graph& graph,
    const std::vector<std::tuple<std::string, std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode)
{
    std::unordered_set<std::string> visitedSet;
    std::unordered_map<std::string, std::string> parentMap;
    std::unordered_set<std::string> traversedEdges;

    for (const auto& [node, parent, _] : path) {
        visitedSet.insert(node);
        if (!parent.empty()) parentMap[node] = parent;
    }

    for (const auto& [child, parent] : parentMap)
        traversedEdges.insert(parent + "->" + child);

    json exportJson = {
        {"nodes", json::array()},
        {"links", json::array()}
    };

    for (const auto& [node, parent, _] : path) {
        json nodeJson = {
            {"id", node},
            {"visited", true}
        };

        if (node == startNode) nodeJson["start"] = true;

        auto it = graph.getVideoStatsMap().find(node);
        if (it != graph.getVideoStatsMap().end()) {
            nodeJson["group"] = classifyFlagLevel(it->second.flagCount);
            nodeJson["flag_count"] = it->second.flagCount;
        }

        exportJson["nodes"].push_back(nodeJson);
    }

    for (const auto& [parent, suggestions] : graph.getAdjList()) {
        if (!visitedSet.count(parent)) continue;
        for (const auto& [child, _] : suggestions) {
            if (!visitedSet.count(child)) continue;
            bool traversed = traversedEdges.count(parent + "->" + child);
            exportJson["links"].push_back({
                {"source", parent},
                {"target", child},
                {"traversed", traversed}
            });
        }
    }

    std::string filename = "visualization/graph_" + searchType + "_" + stat + ".json";
    std::ofstream out(filename);
    if (out.is_open()) {
        out << exportJson.dump(2);
    } else {
        std::cerr << "[ERROR] Could not write file: " << filename << std::endl;
    }
}

/*--------------------------------------------------
  Export + Copy Traversal File for Visualization
--------------------------------------------------*/
void exportAndPrepareForViewer(Graph& graph,
    const std::vector<std::pair<std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode)
{
    exportTraversalToJSON(graph, path, searchType, stat, startNode);
    copyTraversalToSearchJSON(searchType, stat);
}

void exportAndPrepareForViewer(Graph& graph,
    const std::vector<std::tuple<std::string, std::string, double>>& path,
    const std::string& searchType,
    const std::string& stat,
    const std::string& startNode)
{
    exportTraversalToJSON(graph, path, searchType, stat, startNode);
    copyTraversalToSearchJSON(searchType, stat);
}

/*--------------------------------------------------
  Copy Output File to search.json for Browser Viewer
--------------------------------------------------*/
void copyTraversalToSearchJSON(const std::string& searchType, const std::string& stat) {
    std::string source = "visualization/graph_" + searchType + "_" + stat + ".json";
    std::string dest = "visualization/search.json";

    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(dest, std::ios::binary);

    if (src && dst) {
        dst << src.rdbuf();
    } else {
        std::cerr << "[ERROR] Copy failed: " << source << " → " << dest << std::endl;
    }
}

/*--------------------------------------------------
  Classify Flag Severity into Visual Groups
--------------------------------------------------*/
std::string classifyFlagLevel(int flags) {
    if (flags == 0) return "Safe";
    if (flags <= 5) return "Mild";
    if (flags <= 15) return "Borderline";
    if (flags <= 30) return "Risky";
    if (flags <= 100) return "Avoid";
    return "Extreme";
}