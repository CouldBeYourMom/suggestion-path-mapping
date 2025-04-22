#include "graph.h"
#include "search_astar.h"
#include "search_dijkstra.h"
#include "search_random_walk.h"
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sqlite3.h>
#include <stack>
#include <unordered_set>
#include "../visualization/cpp/graph_viz.h"
#include "../visualization/cpp/external/json.hpp"

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
                  << "B - Standard BFS (testing only)\n"
                  << "S - Standard DFS (testing only)\n> ";
        std::getline(std::cin, searchType);
        std::transform(searchType.begin(), searchType.end(), searchType.begin(), ::toupper);
        if (searchType == "A" || searchType == "D" || searchType == "R" || searchType == "B" || searchType == "S") break;
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

    std::string stat;
    if (weightType == "1") stat = "flag";
    else if (weightType == "2") stat = "like";
    else if (weightType == "3") stat = "view";
    else if (weightType == "4") stat = "comment";

    std::cout << "🧠 Building graph using stat: " << stat << std::endl;
    createGraphFromDB(dbPath, stat);
    runSelectedSearch(searchType, stat);
    exportFullGraphToJSON(*this, "visualization/graph.json");
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
    int maxNodes = 1000000; // Adjust or make this configurable later
    std::vector<std::pair<std::string, double>> result;
    
    if (searchType == "S") {
        auto dfsResults = dfs(startNode);
        std::vector<std::pair<std::string, double>> flatResults;
        for (const auto& [node, parent, time] : dfsResults) {
            flatResults.emplace_back(node, time);
        }
        exportResults(flatResults, "dfs", stat);
        exportAndPrepareForViewer(*this, dfsResults, "dfs", stat, startNode);
    } else if (searchType == "B") {
        result = bfs(startNode);
        exportResults(result, "bfs", stat);
        exportAndPrepareForViewer(*this, result, "bfs", stat, startNode);
    } else if (searchType == "A") {
        result = runAStar(*this, startNode, maxNodes, stat);
        exportResults(result, "astar", stat);
        exportAndPrepareForViewer(*this, result, "astar", stat, startNode);
    } else if (searchType == "D") {
        auto dijkstraResults = runDijkstra(*this, startNode, maxNodes, stat);
        std::vector<std::pair<std::string, double>> flatResults;
        for (const auto& [node, parent, time] : dijkstraResults) {
            flatResults.emplace_back(node, time);
        }
        exportResults(flatResults, "dijkstra", stat);
        exportAndPrepareForViewer(*this, dijkstraResults, "dijkstra", stat, startNode);
    } else if (searchType == "R") {
        result = runRandomWalk(*this, startNode, maxNodes, stat);
        exportResults(result, "randomwalk", stat);
        exportAndPrepareForViewer(*this, result, "randomwalk", stat, startNode);
    }
}

/*_________________________
    🪄 Vera Verto! 🪄
___________________________*/
/*
Transform raw stat into a prioritizable traversal weight
 - Makes high raw values result in lower traversal cost
*/
double veraVerto(int raw, int maxStat) {
    double x = static_cast<double>(raw) + 1.0; // Avoid log(0)
    double m = static_cast<double>(maxStat) + 1.0;
    return 1000000000*(std::log(x) / m);
}

/*_________________________
  Graph Construction from 
     Database (SQLite)
___________________________*/
/*
Queries the database to:
 - Read parent → suggested relationships
 - Assign weights based on chosen stat
 - Populate in-memory adjacency list
Also calls loadVideoStatsMap() to preload all stats for each video to use in outout.
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

    // Get the highest value for the chosen stat from the DB to use for transformation
    int maxStat = 0;
    std::string maxQuery = R"(
        SELECT MAX(CAST(
            CASE
                WHEN ? = 'flag' THEN flag_count
                WHEN ? = 'like' THEN like_count
                WHEN ? = 'view' THEN view_count
                WHEN ? = 'comment' THEN comment_count
                ELSE 0
            END
        AS INTEGER)) FROM video_edges_full;
    )";

    sqlite3_stmt* maxStmt;
    if (sqlite3_prepare_v2(db, maxQuery.c_str(), -1, &maxStmt, nullptr) == SQLITE_OK) {
        for (int i = 1; i <= 4; ++i) {
            sqlite3_bind_text(maxStmt, i, stat.c_str(), -1, SQLITE_STATIC);
        }

        if (sqlite3_step(maxStmt) == SQLITE_ROW) {
            maxStat = sqlite3_column_int(maxStmt, 0);
            std::cout << "🌟 Max stat value for \"" << stat << "\": " << maxStat << std::endl;
        }

        sqlite3_finalize(maxStmt);
    } else {
        std::cerr << "[ERROR] Failed to prepare maxStat query: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

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
            std::string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            int flagCount      = sqlite3_column_int(stmt, 3);
            int likeCount      = sqlite3_column_int(stmt, 4);
            int viewCount      = sqlite3_column_int(stmt, 5);
            int commentCount   = sqlite3_column_int(stmt, 6);

            int weight = -1; // fallback to detect failure

            if (stat == "flag") {
                weight = flagCount;
            } else if (stat == "like") {
                weight = likeCount;
            } else if (stat == "view") {
                weight = viewCount;
            } else if (stat == "comment") {
                weight = commentCount;
            } else {
                std::cerr << "❌ Unmatched stat string: [" << stat << "]" << std::endl;
            }

            double transformedWeight = veraVerto(weight, maxStat);

            adjList[parent].emplace_back(suggested, transformedWeight);
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

            if (videoStatsMap.count("ORIGINAL") == 0) {
                videoStatsMap["ORIGINAL"] = {
                    .title = "Start Node",
                    .flagCount = 0,
                    .likeCount = 0,
                    .viewCount = 0,
                    .commentCount = 0
                };
            }
            
            videoStatsMap[id] = { title, flag, like, view, comment };
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "[ERROR] Failed to prepare stat query: " << sqlite3_errmsg(db) << std::endl;
    }
}

/*_____________________
   DFS Path Traversal
  with Revisit Logic
_______________________*/
/*
Performs a modified DFS traversal:
 - Allows revisits if parent path changes
 - Records visit time (ms since DFS start)
Returns a vector (node, parent, timestamp)
*/
std::vector<std::tuple<std::string, std::string, double>> Graph::dfs(const std::string& startNode) const {
    std::vector<std::tuple<std::string, std::string, double>> visited;
    auto startTime = std::chrono::steady_clock::now();

    std::stack<std::pair<std::string, std::string>> s;  // current, parent
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
        visited.emplace_back(current, parent, elapsed);

        if (adjList.find(current) != adjList.end()) {
            for (auto it = adjList.at(current).rbegin(); it != adjList.at(current).rend(); ++it) {
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
Performs a standard unweighted BFS traversal
*/
std::vector<std::pair<std::string, double>> Graph::bfs(const std::string& startNode) const {
    std::vector<std::pair<std::string, double>> visited;

    if (adjList.find(startNode) == adjList.end()) {
        std::cerr << "[ERROR] Start node not found: " << startNode << std::endl;
        return visited;
    }

    auto startTime = std::chrono::steady_clock::now();

    std::queue<std::pair<std::string, std::string>> q; // {current, parent}
    q.push({startNode, ""});

    // Track visited (node, parent) pairs
    std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs;

    while (!q.empty()) {
        auto [current, parent] = q.front();
        q.pop();

        if (visitedPairs.count({current, parent})) continue;
        visitedPairs.insert({current, parent});

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(current, elapsed);

        if (adjList.find(current) != adjList.end()) {
            const auto& neighbors = adjList.at(current);
            for (const auto& neighbor : neighbors) {
                q.push({neighbor.first, current});
            }
        }
    }

    return visited;
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

/*_________________________
     TRAVERSAL Helper 
        Functions
___________________________*/
/* 
 Retrieves a list of adjacent nodes and edge weights for the given node ID
 - Used by external search algorithms (e.g., A*, Dijkstra, RandomWalk)
 - Returns an empty vector if node ID is not found in the graph
*/
const std::vector<std::pair<std::string, double>>& Graph::getNeighbors(const std::string& nodeId) const {
    static const std::vector<std::pair<std::string, double>> empty; // Fallback
    auto it = adjList.find(nodeId);
    return (it != adjList.end()) ? it->second : empty;
}

/*_____________________
      Accessors
_______________________*/
const std::unordered_map<std::string, Graph::VideoStats>& Graph::getVideoStatsMap() const {
    return videoStatsMap;
}

const std::unordered_map<std::string, std::vector<std::pair<std::string, double>>>& Graph::getAdjList() const {
    return adjList;
}
/*_____________________
     Graph Destructor
_______________________*/
/*
Ensures graph cleanup by clearing:
 - Adjacency list
Optional since program exits quickly, but good for hygiene.
*/
Graph::~Graph() {
    adjList.clear();
}