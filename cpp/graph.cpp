#include "graph.h"
#include <iostream>
#include <sqlite3.h>

Graph::Graph() {}

void Graph::addEdge(const std::string& from, const std::string& to, double weight) {
    adjList[from].emplace_back(to, weight);
}

void Graph::printGraph() const {
    for (const auto& node : adjList) {
        std::cout << node.first << " -> ";
        for (const auto& edge : node.second) {
            std::cout << "(" << edge.first << ", " << edge.second << ") ";
        }
        std::cout << std::endl;
    }
}

const std::unordered_map<std::string, std::vector<std::pair<std::string, double>>>& Graph::getAdjList() const {
    return adjList;
}

#include <sqlite3.h>
#include "graph.h"
#include <iostream>

void Graph::loadGraphFromDB(const std::string& dbPath) {
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::string sql = "SELECT from_video, to_video, weight FROM video_relationships;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string from = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string to = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double weight = sqlite3_column_double(stmt, 2);

            addEdge(from, to, weight);
        }
    } else {
        std::cerr << "Failed to execute query: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

