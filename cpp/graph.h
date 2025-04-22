#ifndef GRAPH_H
#define GRAPH_H

#include <sqlite3.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Custom hash for tracking (node, parent) pairs
struct pair_hash {
    std::size_t operator()(const std::pair<std::string, std::string>& p) const {
        std::hash<std::string> hash_fn;
        return hash_fn(p.first) ^ (hash_fn(p.second) << 1);
    }
};

class Graph {
private:
    /*-------------
      STRUCTURES
    ---------------*/

    // Holds stats and metadata for each video
    struct VideoStats {
        std::string title;
        int flagCount = 0;
        int likeCount = 0;
        int viewCount = 0;
        int commentCount = 0;
    };

    /*-------------
      GRAPH DATA
    ---------------*/

    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> adjList; // Directed graph: parent → list of (child, weight)
    std::unordered_map<std::string, VideoStats> videoStatsMap; // Maps video ID to stat info

    /*-------------
      GRAPH SETUP
    ---------------*/

    void createGraphFromDB(const std::string& dbPath, const std::string& stat); // Loads edges and weights from database
    void loadVideoStatsMap(sqlite3* db); // Populates videoStatsMap with title and stat info

    /*-------------
      SEARCH LOGIC
    ---------------*/

    void runSelectedSearch(const std::string& searchType, const std::string& stat); // Routes to A*, BFS, Dijkstra, DFS, or RandomWalk
    std::vector<std::tuple<std::string, std::string, double>> dfs(const std::string& startNode) const; // Standard DFS traversal with timestamp
    std::vector<std::pair<std::string, double>> bfs(const std::string& startNode) const; // Standard BFS traversal with timestamp

    /*-------------
      EXPORT & VIZ
    ---------------*/

    void exportResults(
        const std::vector<std::pair<std::string, double>>& result,
        const std::string& searchType,
        const std::string& stat) const; // Outputs CSV with timestamps and stats

    //void printGraph(sqlite3* db) const; // Debug: print adjList with live lookups

public:
    Graph() = default;
    ~Graph(); // Cleanup adjacency list
    
    void userOptions(); // Main program loop
    const std::vector<std::pair<std::string, double>>& getNeighbors(const std::string& nodeId) const; // Returns a const reference to the neighbors and edge weights of a given node
    
    // Accessors for visualization
    const std::unordered_map<std::string, Graph::VideoStats>& getVideoStatsMap() const;
    const std::unordered_map<std::string, std::vector<std::pair<std::string, double>>>& getAdjList() const;
};

#endif