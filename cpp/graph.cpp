#include "graph.h"
#include <iostream>
#include <sqlite3.h>
#include <queue>
#include <stack>
#include <unordered_set>
#include <fstream>
#include "external/json.hpp"
using json = nlohmann::json;

/*
Loads existing graph from database using standard STL containers. 
Video relationships are stored in the manual_video_links table in the DB.
loadGraphFromDB() simply translates it into an in-memory structure 
(the adjacency list) when the program runs.
*/
void Graph::loadGraphFromDB(const std::string& dbPath) {
    std::ofstream log("run_log.txt", std::ios::app);
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
            log << "Adding edge: " << parent << " -> " << suggested << std::endl;
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

    log.close();
    sqlite3_close(db);
}

/*_____________
  Visualization 
    Support
_______________*/
/*
Export the full graph to a JSON file in 3D-force-graph format.

This function:
 - Selects all videos from the database (id, title, flag_count).
 - Joins with `video_flag_counts` to include the total flags for each video.
 - Labels each video with a "group" based on how inappropriate it is.
 - Exports each parent-child relationship (suggestion) from `manual_video_links`.

 This output is used in the frontend visualization with ForceGraph3D.
*/
void Graph::exportFullGraphFromDB(const std::string& filename, const std::string& dbPath) {
    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to open database: " << dbPath << std::endl;
        return;
    }

    nlohmann::json j;
    sqlite3_stmt* stmt;
    
    // Export all videos as nodes with LEFT JOIN to get flag count
    const char* nodeQuery = "SELECT videos.id, videos.title, "
                        "COALESCE(video_flag_counts.flag_count, 0) "
                        "FROM videos "
                        "LEFT JOIN video_flag_counts ON videos.id = video_flag_counts.video_id;";

    if (sqlite3_prepare_v2(db, nodeQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* id_cstr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* title_cstr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            std::string id = id_cstr ? id_cstr : "";
            std::string title = title_cstr ? title_cstr : "[Untitled]";
            int flags = sqlite3_column_int(stmt, 2);
            std::string group = classifyFlagLevel(flags);

            j["nodes"].push_back({
                {"id", id},
                {"title", title},
                {"flags", flags},
                {"group", group}
            });
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "[ERROR] Failed to prepare node export query." << std::endl;
    }

    // Export all edges (links) from manual_video_links
    const char* linkQuery = "SELECT parent_video, suggested_video FROM manual_video_links;";
    if (sqlite3_prepare_v2(db, linkQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string source = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string target = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            j["links"].push_back({{"source", source}, {"target", target}});
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "[ERROR] Failed to prepare link export query." << std::endl;
    }

    // Write to file
    std::ofstream out(filename);
    if (out.is_open()) {
        out << j.dump(2);
        std::cout << "✅ Full graph exported to " << filename << std::endl;
    } else {
        std::cerr << "[ERROR] Could not write to: " << filename << std::endl;
    }

    sqlite3_close(db);
}

    // Added accessor for read-only access to the adjacency list.
    const std::unordered_map<std::string, std::vector<std::pair<std::string, double>>>& Graph::getAdjList() const {
        return adjList;

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
    std::string query = "SELECT flag_count FROM video_flag_counts WHERE video_id = ?;";

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

int Graph::getLikeCount(const std::string& videoId, sqlite3* db) const {
    int likes = 0;
    sqlite3_stmt* stmt;
    std::string query = "SELECT like_count FROM video_stats WHERE id = ?;";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, videoId.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            likes = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return likes;
}

int Graph::getViewCount(const std::string& videoId, sqlite3* db) const {
    int views = 0;
    sqlite3_stmt* stmt;
    std::string query = "SELECT view_count FROM video_stats WHERE id = ?;";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, videoId.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            views = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return views;
}

int Graph::getCommentCount(const std::string& videoId, sqlite3* db) const {
    int comments = 0;
    sqlite3_stmt* stmt;
    std::string query = "SELECT comment_count FROM video_stats WHERE id = ?;";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, videoId.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            comments = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return comments;
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
    Standard
  DFS Traversal
_______________*/

void Graph::dfs(const std::string& startNode) const {
    if (adjList.find(startNode) == adjList.end()) {
        std::cout << "Start node not found: " << startNode << std::endl;
        return;
    }

    std::stack<std::string> s;
    std::unordered_set<std::string> visited;

    s.push(startNode);
    std::cout << "DFS Traversal (ignoring edge weights):\n";

    while (!s.empty()) {
        std::string current = s.top();
        s.pop();

        if (visited.count(current)) continue;

        visited.insert(current);
        std::cout << "Visited: " << current << std::endl;

        if (adjList.find(current) != adjList.end()) {
            // Reverse to maintain DFS ordering like recursion
            const auto& neighbors = adjList.at(current);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                if (!visited.count(it->first)) {
                    s.push(it->first);
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