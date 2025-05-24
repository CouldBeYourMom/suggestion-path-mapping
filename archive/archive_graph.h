#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <sqlite3.h>

class Graph {
public:
    
    ~Graph(); // Destructor to ensure cleanup
    void loadGraphFromDB(const std::string& dbPath);
    void exportFullGraphFromDB(const std::string& filename, const std::string& dbPath);
    void printGraph(sqlite3* db) const;
    void bfs(const std::string& startNode) const;
    void dfs(const std::string& startNode) const;

private:
    void exportCSVWithStats(const std::string& filename, sqlite3* db);
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> adjList;
    std::unordered_map<std::string, int> flagCounts;
    std::string classifyFlagLevel(int flags) const;
    int getFlagCount(const std::string& videoId, sqlite3* db) const;
    std::string getTitle(const std::string& videoId, sqlite3* db) const;
    int getLikeCount(const std::string& videoId, sqlite3* db) const;
    int getViewCount(const std::string& videoId, sqlite3* db) const;
    int getCommentCount(const std::string& videoId, sqlite3* db) const;
    
};

#endif