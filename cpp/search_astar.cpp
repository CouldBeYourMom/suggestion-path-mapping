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

    // Potential heuristic: number of views in the node's video
    return 0.0;
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
    std::vector<std::string> shortestPath;
    // Define lambda function for comparing values in PQ.
    // Returns true if a comes before b. However, b is the first to come out.
    // f_a > f_b: b will come out first if it has smaller f value
    auto cmp = [](std::string firstNode, std::string secondNode){
        return fValues[firstNode] > fValues[secondNode];
    };
    // Define priority queue
    std::priority_queue<std::string, std::vector<std::string>, decltype(cmp)> unprocessed(cmp);
    fValues[startNode] = 0;
    gValues[startNode] = 0;


    //
    while (!unprocessed.empty()) {
        std::string currentNode = unprocessed.top();
        unprocessed.pop();

        // Retrieve neighbors of currentNode
        std::vector<std::string> neighbors = graph.adjList.at(currentNode);
        // For each neighbor of currentNode.
        for (auto n : neighbors) {
            // Calculate tentative g value for neighbor. g(neighbor) = g(currentNode) + weight(currentNode to neighbor)
            float edgeWeight = 0.0; // Get edge weight from current node to the neighbor.
            float tentativeG = gValues[currentNode] + edgeWeight; // tentativeG = g(currentNode) + edge weight

            // if tentative g is less than current g value of neighbor, perform updates.
            if (tentativeG < gValues[n]) {
                // Perform updates: update g(neighbor) and f(neighbor). Update parent(neighbor)
                gValues[n] = tentativeG;
                fValues[n] = gValues[n] + heuristic(n);
            }

            // Add neighbor to priority queue.
            unprocessed.push(n);
        }
    }

    return result;
}