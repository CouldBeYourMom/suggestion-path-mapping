#include "search_astar.h"
#include <chrono>
#include <iostream>



double calculateHeuristic(const Graph& g, std::string& node, const std::string& weightType) {
    // Heuristic estimate is just extracting edge weight.
    return 3.0;
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
    std::priority_queue<std::pair<std::string, double>, std::vector<std::pair<std::string, double>>, decltype(cmp)> unprocessed(cmp);

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
        std::pair<std::string, double> pqTop = unprocessed.top();
        std::string curr = pqTop.first;
        unprocessed.pop();
        // Mark node as visited
        // Retrieve the time stamp and time elapsed
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(curr, elapsed);

        auto neighbors = g.getNeighbors(curr);
        if (neighbors.size() == 0) continue;
        // Now go through neighbors and add neighbors into priority queue.
        for (std::pair<std::string, int> neighborWeight : neighbors) {
            std::string neighbor = neighborWeight.first;
            int weight = neighborWeight.second;

            if (distances.find(neighbor) == distances.end() || fValues.find(neighbor) == fValues.end()) {
                // distances has not been visited
                parents[neighbor] = curr;
                distances[neighbor] = distances[curr] + weight;
                fValues[neighbor] = distances[neighbor] + calculateHeuristic(g, neighbor, weightType);
            } else {
                double tentative = distances[neighbor] + calculateHeuristic(g, neighbor, weightType);
                if (tentative < fValues[neighbor]) {
                    fValues[neighbor] = tentative;
                    parents[neighbor] = curr;
                    distances[neighbor] = distances[curr] + weight;
                }
            }
            // Add neighbors into priority queue.
            std::pair<std::string, double> newNeighborPair{neighbor, fValues[neighbor]};
            unprocessed.push(newNeighborPair);

        }
    }
    return visited;
}