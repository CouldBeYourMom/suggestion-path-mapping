#include "graph.h"
#include <iostream>
#include <sqlite3.h>


int main() {
    Graph g;

    // 📥 Load the graph from the SQLite database
    g.loadGraphFromDB("data/youtube_data.db");

    // 🧪 Run a BFS traversal from a selected starting node
    // Replace this ID with a real one from your DB when testing
    std::string startNode = "ORIGINAL";
    g.bfs(startNode);

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