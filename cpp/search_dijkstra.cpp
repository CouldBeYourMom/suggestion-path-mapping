#include "search_dijkstra.h"
#include <chrono>
#include <iostream>

// This replaces your old SearchDijkstra::dijkstra() and gives you the final format for team integration
std::vector<std::pair<std::string, double>> runDijkstra(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
) {
    // Keep these following 3 lines here
    std::vector<std::pair<std::string, double>> visited; // visited : vector of (video_id, time_since_start_in_ms)
    //auto startTime = std::chrono::steady_clock::now(); // Start the clock  ****UNCOMMENT THIS LINE
    //int visitCount = 0;           ****UNCOMMENT THIS LINE


    /* ========================================================
        See the dfs() function in graph.cpp for reference
    
        Before you start looping through adjacent vectors add the following line
        std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs; // Hashmap to keep track of parent-child visits
    
        Inside your loop add the following:
        
        after you open.pop(), before you find neighbors put the following 3 lines to capture a timestamp and the video ID for the visit of each node
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(currentNode, elapsed);

     * ======================================================== */

     return {};       // REMOVE THIS LINE
    //return visited; // Replace your current return with this   ****UNCOMMENT THIS LINE
}