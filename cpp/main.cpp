#include "graph.h"
#include <iostream>
#include <algorithm>  // for std::remove

int main(int argc, char* argv[]) {
    Graph g;

    // Path to the SQLite database
    std::string dbPath = "data/youtube_data.db";

    // Default starting node for traversal
    std::string startNode = "ORIGINAL";

    // Parse command-line argument
    std::string arg = (argc >= 2) ? argv[1] : "";
    arg.erase(std::remove(arg.begin(), arg.end(), '\r'), arg.end());
    arg.erase(std::remove(arg.begin(), arg.end(), '\n'), arg.end());

    // 🗂️ EXPORT MODE: comment out to test traversal functions
    if (arg == "export") {
        g.exportFullGraphFromDB("data/graph.json", dbPath);
        return 0;  // Exit early after export
    }

    // 🌐 Otherwise, load graph and run default or team search
    g.loadGraphFromDB(dbPath);
    g.bfs(startNode);      // Replace with desired traversal method

    // ---------------------------------------------
    // 🧑‍💻 Teammate References: Keep these here for testing
    //
    // Run Dijkstra's algorithm (teammate 1)
    // runDijkstra(g, "VIDEO_ID_START", "VIDEO_ID_END");
    //
    // Run A* Search (teammate 2)
    // runAStar(g, "VIDEO_ID_START", "VIDEO_ID_END");
    //
    // Run Random Walk (teammate 3)
    // runRandomWalk(g, "VIDEO_ID_START");
    // ---------------------------------------------
    
    return 0;
}