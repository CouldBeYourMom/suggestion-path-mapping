#include "graph.h"
#include "search_astar.h"
#include "search_dijkstra.h"
#include "search_random_walk.h"
#include <iostream>
#include <sqlite3.h>
#include <queue>
#include <stack>
#include <unordered_set>
#include <functional>   // for std::hash
#include <fstream>
#include <chrono>
#include "external/json.hpp"
using json = nlohmann::json;


/*____________________
    User Input Menu
  and Search Routing
______________________*/
/*
Handles user prompt to select:
 - Search algorithm type (DFS, A*, etc.)
 - Weight type (likes, views, etc.)
Builds the graph from DB using selected weight
and dispatches the correct search function.
*/
void Graph::userOptions() {
    std::string dbPath = "data/youtube_data.db";
    std::string searchType;
    std::string weightType;

    while (true) {
        std::cout << "Choose search algorithm:\n"
                  << "A - AStar\n"
                  << "D - Dijkstra\n"
                  << "R - RandomWalk\n"
                  << "S - Standard DFS (testing only)\n> ";
        std::getline(std::cin, searchType);
        std::transform(searchType.begin(), searchType.end(), searchType.begin(), ::toupper);
        if (searchType == "A" || searchType == "D" || searchType == "R" || searchType == "S") break;
        std::cout << "❌ Invalid input. Try again.\n";
    }

    while (true) {
        std::cout << "\nChoose weight type:\n"
                  << "1 - flag_count\n"
                  << "2 - like_count\n"
                  << "3 - view_count\n"
                  << "4 - comment_count\n> ";
        std::getline(std::cin, weightType);
        if (weightType == "1" || weightType == "2" || weightType == "3" || weightType == "4") break;
        std::cout << "❌ Invalid input. Try again.\n";
    }

    std::string stat = (weightType == "1") ? "flag" :
                       (weightType == "2") ? "like" :
                       (weightType == "3") ? "view" : "comment";

    std::cout << "🧠 Building graph using stat: " << stat << std::endl;
    createGraphFromDB(dbPath, stat);
    runSelectedSearch(searchType, stat);
}

/*_________________________
    Dispatch Search Type
     and Export Results
___________________________*/
/*
Executes the selected search algorithm (DFS, A*, etc.)
 - Start node is hardcoded as "ORIGINAL"
 - Max node visit count is capped at 100 (for now)
Results are passed to exportResults() for CSV logging.
*/
void Graph::runSelectedSearch(const std::string& searchType, const std::string& stat) {
    std::string dbPath = "data/youtube_data.db";
    std::string startNode = "ORIGINAL";
    int maxNodes = 100; // Adjust or make this configurable later
    std::vector<std::pair<std::string, double>> result;
    
    if (searchType == "S") {
        result = dfs(startNode);
        exportResults(result, "dfs", stat);
    } else if (searchType == "A") {
        auto astarResults = runAStar(*this, startNode, maxNodes, stat);
        std::vector<std::pair<std::string, double>> flatResults;
        for (const auto& [node, parent, time] : astarResults) {
            flatResults.emplace_back(node, time);
        }
        exportResults(flatResults, "astar", stat);
        exportAndPrepareForViewer(*this, astarResults, "astar", stat, startNode);
    } else if (searchType == "D") {
        auto dijkstraResults = runDijkstra(*this, startNode, maxNodes, stat);
        std::vector<std::pair<std::string, double>> flatResults;
        for (const auto& [node, parent, time] : dijkstraResults) {
            flatResults.emplace_back(node, time);
        }
        exportResults(flatResults, "dijkstra", stat);
        exportAndPrepareForViewer(*this, dijkstraResults, "dijkstra", stat, startNode);
    } else if (searchType == "R") {
        auto randomwalkResults = runRandomWalk(*this, startNode, maxNodes, stat);
        std::vector<std::pair<std::string, double>> flatResults;
        for (const auto& [node, parent, time] : randomwalkResults) {
            flatResults.emplace_back(node, time);
        }
        exportResults(flatResults, "randomwalk", stat);
        exportAndPrepareForViewer(*this, randomwalkResults, "randomwalk", stat, startNode);
    }
}

/*_________________________
  Load Adjacency List from 
       Database (SQLite)
___________________________*/
/*
Queries the database to:
 - Read parent → suggested relationships
 - Assign weights based on chosen stat
 - Populate in-memory adjacency list
Also calls loadVideoStatsMap() to preload stats for each video.
*/
void Graph::createGraphFromDB(const std::string& dbPath, const std::string& stat) {
    std::ofstream log("run_log.txt", std::ios::out);
    std::cout << "📂 Loading graph from: " << dbPath << std::endl;

    sqlite3* db;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    loadVideoStatsMap(db);

    sqlite3_stmt* stmt;
    std::string query = R"(
        SELECT 
            parent_video,
            suggested_video,
            title,
            flag_count,
            like_count,
            view_count,
            comment_count
        FROM video_edges_full
    )";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string parent    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string suggested = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double flagCount      = sqlite3_column_double(stmt, 2);
            double likeCount      = sqlite3_column_double(stmt, 3);
            double viewCount      = sqlite3_column_double(stmt, 4);
            double commentCount   = sqlite3_column_double(stmt, 5);

            double weight = 1.0; // fallback default
            if (stat == "flag") weight = flagCount;
            else if (stat == "like") weight = likeCount;
            else if (stat == "view") weight = viewCount;
            else if (stat == "comment") weight = commentCount;

            adjList[parent].emplace_back(suggested, weight);
            log << "Adding edge: " << parent << " -> " << suggested
                << " (weight = " << weight << ")" << std::endl;
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }

    log.close();
    sqlite3_close(db);
}

/*____________________________
  Populate VideoStats Map from 
        Database (SQLite)
______________________________*/
/*
Loads title and stat info for each unique video into
an in-memory map (`videoStatsMap`) to avoid redundant DB access.
Used by exportResults() and search functions.
*/
void Graph::loadVideoStatsMap(sqlite3* db) {
    sqlite3_stmt* stmt;
    std::string query = R"(
        SELECT DISTINCT suggested_video, title, flag_count, like_count, view_count, comment_count
        FROM video_edges_full
    )";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string id    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int flag          = sqlite3_column_int(stmt, 2);
            int like          = sqlite3_column_int(stmt, 3);
            int view          = sqlite3_column_int(stmt, 4);
            int comment       = sqlite3_column_int(stmt, 5);

            videoStatsMap[id] = { title, flag, like, view, comment };
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "[ERROR] Failed to prepare stat query: " << sqlite3_errmsg(db) << std::endl;
    }
}

/*_________________________
  Export Graph for 3D JSON 
     Visualization Format
___________________________*/
/*
Builds a JSON file compatible with ForceGraph3D:
 - Nodes include ID, title, flag count, and safety label
 - Links pulled from manual_video_links
Used for rendering full suggestion graphs in the browser.
*/
void Graph::vizGraphFromDB(const std::string& filename, const std::string& dbPath) {
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

/*_____________________
   Print Adjacency List
   with Live DB Lookups
_______________________*/
/*
Prints the adjacency list:
 - For each edge, retrieves live title and flag info from DB
 - Outputs formatted line with weights, safety labels, and video titles
Used for debugging and quick DB verification.
*/
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

/*_________________________
   Export Search Results to 
   CSV with Stats and Time
___________________________*/
/*
Saves search results to a CSV:
 - Includes timestamp (ms), video ID, stat info, and title
 - Filenames begin with search type and are time-stamped
 - Appends total elapsed time at bottom
Used for logging path traversal and benchmarking.
*/
void Graph::exportResults(
    const std::vector<std::pair<std::string, double>>& result,
    const std::string& searchType,
    const std::string& stat) const
{
    // Generate timestamp for file name
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::localtime(&timeNow);

    // Filename
    char timestamp[50];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", timeInfo);
    std::string outFile = "data/" + searchType + "_logs/" + searchType + "_" + timestamp + "_" + stat + ".csv";

    std::ofstream file(outFile);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not write to file: " << outFile << std::endl;
        return;
    }

    // CSV Header
    file << "order,timestamp_ms,video_id,flag_count,like_count,view_count,comment_count,title\n";

    auto start = std::chrono::steady_clock::now();

    for (size_t i = 0; i < result.size(); ++i) {
        const auto& [videoId, timestampMs] = result[i];
        std::string title = "[Unknown]";
        int flagCount = 0, likeCount = 0, viewCount = 0, commentCount = 0;

        // Lookup video stats from preloaded map
        auto it = videoStatsMap.find(videoId);
        if (it != videoStatsMap.end()) {
            const auto& stats = it->second;
            title = stats.title;
            flagCount = stats.flagCount;
            likeCount = stats.likeCount;
            viewCount = stats.viewCount;
            commentCount = stats.commentCount;
        }

        file << i + 1 << ","
            << timestampMs << ","
            << videoId << ","
            << flagCount << ","
            << likeCount << ","
            << viewCount << ","
            << commentCount << ","
            << title << "\n";
    }

    // Add total time elapsed at bottom of file
    if (!result.empty()) {
        double totalElapsed = result.back().second - result.front().second;
        file << ","
             << totalElapsed << ","
             << "[TOTAL TIME ELAPSED]" << ",,,,,\n";
    }

    file.close();
    std::cout << "✅ Search results exported to: " << outFile << std::endl;
}

/*_____________________
   DFS Path Traversal
  with Revisit Logic
_______________________*/
/*
Performs a modified DFS traversal:
 - Allows revisits if parent path changes
 - Records visit time (ms since DFS start)
Returns a vector of visited nodes with timestamps.
*/
std::vector<std::pair<std::string, double>> Graph::dfs(const std::string& startNode) const {
    std::vector<std::pair<std::string, double>> visited;
    auto startTime = std::chrono::steady_clock::now();

    if (adjList.find(startNode) == adjList.end()) {
        std::cerr << "[ERROR] Start node not found: " << startNode << std::endl;
        return visited;
    }

    std::stack<std::pair<std::string, std::string>> s;  // {current, parent}
    s.push({startNode, ""});

    // Track visited (node, parent) pairs
    std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs; // Hashmap to keep track of parent-child visits

    while (!s.empty()) {
        auto [current, parent] = s.top();
        s.pop();

        // Only continue if this (node, parent) combo hasn't been visited
        if (visitedPairs.count({current, parent})) continue;
        visitedPairs.insert({current, parent});

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(current, elapsed);

        if (adjList.find(current) != adjList.end()) {
            const auto& neighbors = adjList.at(current);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                s.push({it->first, current});
            }
        }
    }
    return visited;
}

/*_____________________
   BFS Path Traversal
_______________________*/
/*
Performs a standard unweighted BFS traversal:
 - Logs each visited node to std::cout
 - Does not export results or track timestamps
Primarily used for manual inspection.
*/
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

/*_____________________
  Classify Flag Severity
_______________________*/
/*
Maps numeric flag count to a safety category:
 - Returns emoji-coded label (e.g., "🟡 Mild")
Used for classification in graph export and terminal output.
*/
std::string Graph::classifyFlagLevel(int flags) const{
    if (flags == 0) return "🟢 Safe";
    if (flags <= 5) return "🟡 Mild";
    if (flags <= 15) return "🟠 Borderline";
    if (flags <= 30) return "🔴 Risky";
    if (flags <= 100) return "🚨 Avoid";
    return "☠️ Extreme";
}

/*_________________________
  Video Stat Helper Functions
   (Flag, Like, View, Comment)
___________________________*/
/*
Each function pulls a single stat value from the database
 - Used by printGraph() and internal analytics
 - Fallbacks to 0 if not found
*/
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

/*_________________________
     TRAVERSAL Helper 
        Functions
___________________________*/
/* 
 Retrieves a list of adjacent nodes and edge weights for the given node ID
 - Used by external search algorithms (e.g., A*, Dijkstra, RandomWalk)
 - Returns an empty vector if node ID is not found in the graph
*/
const std::vector<std::pair<std::string, int>>& Graph::getNeighbors(const std::string& nodeId) const {
    static const std::vector<std::pair<std::string, int>> empty; // Fallback
    auto it = adjList.find(nodeId);
    return (it != adjList.end()) ? it->second : empty;
}

/*_____________________
     Graph Destructor
_______________________*/
/*
Ensures graph cleanup by clearing:
 - Adjacency list
 - Flag counts
Optional since program exits quickly, but good for hygiene.
*/
Graph::~Graph() {
    adjList.clear();
    flagCounts.clear();
}