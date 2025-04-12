#include "search_random_walk.h"
#include <cstdlib>   // For rand() and srand()
#include <ctime>     // For time()

/* Simple implementation of the Random Walk algorithm.
The randomWalk() function starts at the given start node and, for each step,
randomly selects one of the outgoing neighbors to continue the walk.
If the current node has no outgoing edges, the walk terminates early.
(getAdjList()) to its internal adjacency list. */

std::vector<std::string> SearchRandomWalk::randomWalk(const Graph& graph, const std::string& startNode, int steps) {
    // Retrieve the graph's adjacency list using the accessor.
    const auto& adjList = graph.getAdjList();
    
    // Seed the random number generator.
    srand(static_cast<unsigned int>(time(0)));
    
    // Initialize the path with the starting node.
    std::vector<std::string> path;
    path.push_back(startNode);
    std::string current = startNode;
    
    // Perform the random walk for up to 'steps' iterations.
    for (int i = 0; i < steps; ++i) {
        // Look for the current node in the adjacency list.
        auto it = adjList.find(current);
        if (it == adjList.end() || it->second.empty()) {
            // If the current node is not found or has no outgoing edges, terminate the walk.
            break;
        }
        
        // Select a random neighbor from the available edges.
        int index = rand() % it->second.size();
        current = it->second[index].first;
        
        // Add the selected node to the path.
        path.push_back(current);
    }
    
    return path;
}
