#include "search_astar.h"
#include <chrono>
#include <iostream>

/*
 * Input: A node in the graph.
 * Output: The heuristic value for this node.
 * This function should calculate the heuristic value for a node using the heuristic function.
 *
 */
float SearchAStar::heuristic(const std::string& startNode) {
    return 0;
}

/*
 *
 */
std::vector<std::string> reconstructPath() {
 	int x;
}

std::vector<std::string> SearchAStar::astar(const Graph& graph, const std::string& startNode, const std::string& endNode) {
    // TODO: Implement A* Search Algorithm
    std::vector<std::pair<std::string, double>> result;
    std::priority_queue<std::string> open;
    fValues[startNode] = 0;
    gValues[startNode] = 0;


    //
    while (!open.empty()) {
        std::string currentNode = open.top();
        open.pop();

        // Retrieve neighbors of currentNode
        std::vector<std::string> neighbors = getNeighbors(currentNode);
        // For each neighbor of currentNode.
        for (auto n : neighbors) {
            // Calculate tentative g value for neighbor. g(neighbor) = g(currentNode) + weight(currentNode to neighbor)
            float edgeWeight = 0.0; // Update this with true edge weights.
            float tentativeG = gValues[currentNode] + edgeWeight; // tentativeG = g(currentNode) + edge weight

            // if tentative g is less than current g value of neighbor, perform updates.
            if (tentativeG < gValues[n]) {
                // Perform updates: update g(neighbor) and f(neighbor). Update parent(neighbor)
                gValues[n] = tentativeG;
                fValues[n] = gValues[n] + heuristic(n);
            }

            // Add neighbor to priority queue.
            open.push(n);
        }
    }

    return result;
}