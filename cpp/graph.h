#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

class Graph {
public:
    void loadGraphFromDB(const std::string& dbPath);
    void printGraph(sqlite3* db) const;
    void bfs(const std::string& startNode) const;
    ~Graph(); // Destructor to ensure cleanup

private:
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> adjList;
    std::unordered_map<std::string, int> flagCounts;
    int getFlagCount(const std::string& videoId, sqlite3* db) const;
    std::string getTitle(const std::string& videoId, sqlite3* db) const;
};

#endif