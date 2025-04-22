#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <sqlite3.h>

// Custom hash for tracking (node, parent) pairs in DFS
struct pair_hash {
    std::size_t operator()(const std::pair<std::string, std::string>& p) const {
        std::hash<std::string> hash_fn;
        return hash_fn(p.first) ^ (hash_fn(p.second) << 1);
    }
};

class Graph {
public:
    Graph() = default;
    ~Graph(); // Cleanup adjacency list and flagCounts
    void userOptions(); // Prompts user for algorithm and stat type, builds graph, and runs selected search
    const std::vector<std::pair<std::string, int>>& getNeighbors(const std::string& nodeId) const; // Returns a const reference to the neighbors and edge weights of a given node

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

    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> adjList; // Directed graph: parent → list of (child, weight)
    std::unordered_map<std::string, VideoStats> videoStatsMap; // Maps video ID to stat info
    std::unordered_map<std::string, int> flagCounts; // May be deprecated — used in legacy visualization

    /*-------------
        GRAPH SETUP
    ---------------*/

    void createGraphFromDB(const std::string& dbPath, const std::string& stat); // Loads edges and weights from database
    void loadVideoStatsMap(sqlite3* db); // Populates videoStatsMap with title and stat info

    /*-------------
        SEARCH LOGIC
    ---------------*/

    void runSelectedSearch(const std::string& searchType, const std::string& stat); // Routes to A*, Dijkstra, DFS, or RandomWalk
    std::vector<std::pair<std::string, double>> dfs(const std::string& startNode) const; // Standard DFS traversal with timestamp

    void bfs(const std::string& startNode) const; // Basic unweighted BFS (console output only)

    /*-------------
        EXPORT & VIZ
    ---------------*/

    void exportResults(
        const std::vector<std::pair<std::string, double>>& result,
        const std::string& searchType,
        const std::string& stat) const; // Outputs CSV with timestamps and stats

    void printGraph(sqlite3* db) const; // Debug: print adjList with live lookups
    void vizGraphFromDB(const std::string& filename, const std::string& dbPath); // Exports graph in JSON for ForceGraph3D
    void exportCSVWithStats(const std::string& filename, sqlite3* db); // [Unused?] — Generic stat export

    /*-------------
       DB UTILITIES
    ---------------*/

    std::string getTitle(const std::string& videoId, sqlite3* db) const;
    int getFlagCount(const std::string& videoId, sqlite3* db) const;
    int getLikeCount(const std::string& videoId, sqlite3* db) const;
    int getViewCount(const std::string& videoId, sqlite3* db) const;
    int getCommentCount(const std::string& videoId, sqlite3* db) const;
    std::string classifyFlagLevel(int flags) const; // Returns emoji-coded label (🟢, 🔴, ☠️)
};

#endif