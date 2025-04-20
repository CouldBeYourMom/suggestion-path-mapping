#include "search_random_walk.h"
#include <chrono>
#include <iostream>

std::vector<std::pair<std::string, double>> runRandomWalk(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
) {
    // Keep these following 3 lines here
    std::vector<std::pair<std::string, double>> visited; // visited : vector of (video_id, time_since_start_in_ms)
    auto startTime = std::chrono::steady_clock::now(); // Start the clock  ****UNCOMMENT THIS LINE
    int visitCount = 0;     

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

    // Implementation: random walk logic
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::string current = startNode;
    visited.emplace_back(current, 0.0);
    for (int i = 1; i < maxNodesToVisit; ++i) {
        const auto& neighbors = g.getNeighbors(current);
        if (neighbors.empty()) break;
        int idx = std::rand() % neighbors.size();
        current = neighbors[idx].first;
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(current, elapsed);
    }

    return visited; 
}
