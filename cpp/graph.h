#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>

class Graph {
public:
    Graph();
    void addEdge(const std::string& from, const std::string& to, double weight = 1.0);
    void printGraph() const;
    
    // Accessors for algorithms
    const std::unordered_map<std::string, std::vector<std::pair<std::string, double>>>& getAdjList() const;

    void loadGraphFromDB(const std::string& dbPath);
    
private:
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> adjList;
};

#endif
