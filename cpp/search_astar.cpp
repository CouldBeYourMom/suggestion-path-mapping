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

double calculateHeuristic(std::string& node, const std::string& weightType) {
    // Heuristic estimate is just extracting edge weight.
}


std::vector<std::pair<std::string, double>> runAStar(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
) {
    // List of things to return and start time.
    std::vector<std::pair<std::string, double>> visited; // Returning this vector: visited : vector of (video_id, time_since_start_in_ms)
    auto startTime = std::chrono::steady_clock::now(); // Start the clock
    int visitCount = 0;

    // Maps for tracking parent, f values (f = distance + heuristic), and distances
    std::unordered_map<std::string, std::string> parents;
    std::unordered_map<std::string, double> fValues;
    std::unordered_map<std::string, double> distances;

    // Define lambda function for comparing values in PQ.
    auto cmp = [](std::pair<std::string, double> firstNodeF, std::pair<std::string, double> secondNodeF){
        //return fValues[firstNode] > fValues[secondNode];
        return firstNodeF.second > secondNodeF.second;
    };
    std::priority_queue<std::string, std::vector<std::string>, decltype(cmp)> unprocessed(cmp);

    // Insert the start node
    fValues[startNode] = 0;
    distances[startNode] = 0;
    parents[startNode] = "No parent";

    // Begin iterating through data
    for (int i = 0; i < maxNodesToVisit; i++) {
        if (unprocessed.size() == 0) {
            break;
        }
        // Get current node
        std::string curr = unprocessed.top();
        unprocessed.pop();
        // Mark node as visited
        // Retrieve the time stamp and time elapsed
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(curr, elapsed);

        auto neighbors = g.getNeighbors(curr);
        if (neighbors.size() == 0) continue;
        for (std::pair<std::string, int> neighborWeight : neighbors) {
            std::string neighbor = neighborWeight.first;
            int weight = neighborWeight.second;

            if (distances.find(neighbor) == distances.end() || fValues.find(neighbor) == fValues.end()) {
                // distances has not been visited
            } else {
                double tentative = distances[neighbor] + calculateHeuristic(neighbor, weightType);
                if (tentative < fValues[neighbor]) {
                    fValues[neighbor] = tentative;
                    parents[neighbor] = curr;
                    distances[neighbor] = distances[curr] + weight;
                }

            }

        }
    }
}

std::vector<std::string> SearchAStar::astar(const Graph& graph, const std::string& startNode, const std::string& endNode) {
    // TODOlater: Implement A* Search Algorithm


    // Define priority queue
    std::priority_queue<std::string, std::vector<std::string>, decltype(cmp)> unprocessed(cmp);
    // Create hash maps for tracking distances, f values, and parents.
    //std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs;
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