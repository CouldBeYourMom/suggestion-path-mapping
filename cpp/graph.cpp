#include "graph.h"
#include <iostream>
#include <sqlite3.h>
#include <queue>
#include <unordered_set>

/*
Loads existing graph from database using standard STL containers. 
Video relationships are stored in the manual_video_links table in the DB.
loadGraphFromDB() simply translates it into an in-memory structure 
(the adjacency list) when the program runs.
*/
void Graph::loadGraphFromDB(const std::string& dbPath) {
    std::cout << "📡 Loading graph from: " << dbPath << std::endl;
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_stmt* stmt;
    std::string query = "SELECT parent_video, suggested_video FROM manual_video_links";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        // Iterate over each row in the manual_video_links table and add directed edges to the adjacency list
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string parent = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string suggested = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::cout << "Adding edge: " << parent << " -> " << suggested << std::endl;
            adjList[parent].emplace_back(suggested, 1.0);  // Default weight = 1.0
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to prepare statement." << std::endl;
    }

    // Load node flag weights
    query = "SELECT video_id, flag_count FROM video_flag_counts";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string videoId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int flagCount = sqlite3_column_int(stmt, 1);
            flagCounts[videoId] = flagCount;
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

/*_____________
     Print 
    Function
_______________*/

void Graph::printGraph(sqlite3* db) const {
    for (const auto& pair : adjList) {
        std::cout << pair.first << " -> ";
        for (const auto& neighbor : pair.second) {
            std::string targetId = neighbor.first;
            int weight = neighbor.second;

            int flags = getFlagCount(targetId, db);
            std::string title = getTitle(targetId, db);
            std::string level = classifyFlagLevel(flags);

            std::cout << targetId << " (w=" << weight << ", " << level << ", \"" << title << "\")  ";
        }
        std::cout << std::endl;
    }
}

/*_____________
     Safety 
 Classifications
   Of Videos
_______________*/

std::string Graph::classifyFlagLevel(int flags) const{
    if (flags == 0) return "🟢 Safe";
    if (flags <= 5) return "🟡 Mild";
    if (flags <= 15) return "🟠 Borderline";
    if (flags <= 30) return "🔴 Risky";
    if (flags <= 100) return "🚨 Avoid";
    return "☠️ Extreme";
}

/*_____________
  Video Stat
Helper Functions
_______________*/

int Graph::getFlagCount(const std::string& videoId, sqlite3* db) const {
    int flags = 0;
    sqlite3_stmt* stmt;
    std::string query = "SELECT flags FROM videos WHERE id = ?;";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, videoId.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            flags = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return flags;
}

std::string Graph::getTitle(const std::string& videoId, sqlite3* db) const {
    std::string title = "Unknown";
    sqlite3_stmt* stmt;
    std::string query = "SELECT title FROM videos WHERE id = ?;";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, videoId.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* rawTitle = sqlite3_column_text(stmt, 0);
            if (rawTitle) {
                title = reinterpret_cast<const char*>(rawTitle);
            }
        }
        sqlite3_finalize(stmt);
    }
    return title;
}

/*_____________
    Standard
  BFS Traversal
_______________*/

void Graph::bfs(const std::string& startNode) const {
    if (adjList.find(startNode) == adjList.end()) {
        std::cout << "Start node not found: " << startNode << std::endl;
        return;
    }

    std::queue<std::string> q;
    std::unordered_set<std::string> visited;

    q.push(startNode);
    visited.insert(startNode);

    std::cout << "BFS Traversal (ignoring edge weights):\n";

    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        std::cout << "Visited: " << current << std::endl;
    
        // Check if current has any neighbors before accessing adjList
        if (adjList.find(current) != adjList.end()) {
            for (const auto& neighbor : adjList.at(current)) {
                if (!visited.count(neighbor.first)) {
                    visited.insert(neighbor.first);
                    q.push(neighbor.first);
                }
            }
        }
    }

    std::cout << "\nTraversal complete.\n";
}

/*_____________
   Destructor
_______________*/

Graph::~Graph() {
    adjList.clear();
    flagCounts.clear();
}