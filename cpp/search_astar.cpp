#include "search_astar.h"
#include <chrono>
#include <iostream>

// Comparison class for use in priority queue.
class Compare {
public:
    bool operator() (const std::pair<std::string, double> node1, const std::pair<std::string, double> node2){
        return node1.second > node2.second;
    }
};

// Heuristic function for calculating priorities.
double calculateHeuristic(const Graph& g, std::string& node, const std::string& weightType) {
    // Heuristic estimate will extract edge weight.
    std::vector<std::pair<std::string, double>> neighbors = g.getNeighbors(node);
    // If no neighbors, return heuristic of 0.
    if (neighbors.size() == 0) {
        return 0.0;
    }
    // Find minimum transformed edge weight.
    // Multiplication by 1.0 to convert to double.
    double minWeight = 1.0 * neighbors[0].second;
    for (auto neighbor : neighbors) {
        if (neighbor.second < minWeight) {
            minWeight = 1.0 * neighbor.second;
        }
    }
    return minWeight;
}

// Returns vector of tuples: video, parent, and time
std::vector<std::tuple<std::string, std::string, double>>  runAStar(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
) {
    // List of things to return and start time.
    std::vector<std::tuple<std::string, std::string, double>> visited; // Returning this vector: visited : vector of (video_id, time_since_start_in_ms)
    auto startTime = std::chrono::steady_clock::now(); // Start the clock
    int visitCount = 0;

    // Check if start node is valid.
    if (g.getNeighbors(startNode).size() == 0) {
        std::cerr << "[ERROR] Start node not found or has no neighbors: " << startNode << std::endl;
        return visited;
    }

    // Maps for tracking parent, f values (f = distance + heuristic), and distances
    std::unordered_map<std::string, std::string> parents;
    std::unordered_map<std::string, double> fValues;
    std::unordered_map<std::string, double> distances;
    std::unordered_set<std::string> visitedNodes;

    // Create custom priority queue that acts as min heap and compares nodes.
    std::priority_queue<std::pair<std::string, double>, std::vector<std::pair<std::string, double>>, Compare>
        unprocessed;

    // Insert the start node
    fValues[startNode] = 0;
    distances[startNode] = 0;
    parents[startNode] = "";
    // Insert start node into priority queue as pair of node and f value (0 for the start)
    std::pair<std::string, double> startPair{startNode, 0.0};
    unprocessed.push(startPair);

    // Begin iterating through data
    for (int i = 0; i < maxNodesToVisit; i++) {
        if (unprocessed.size() == 0) {
            std::cout << "Ran out of nodes to process." << std::endl;
            break;
        }

        // Get current node
        std::pair<std::string, double> pqTop = unprocessed.top();
        std::string curr = pqTop.first;
        unprocessed.pop();

        // Check if current node was visited; mark as visited if it hasn't
        if (visitedNodes.find(curr) != visitedNodes.end()) {
            // This node has already been visited. Do not visit it again.
            continue;
        }
        visitedNodes.insert(curr);
        visitCount++;

        // Retrieve the time stamp and time elapsed
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        if (parents.find(curr) == parents.end()) {
            visited.emplace_back(curr, "", elapsed);
        } else {
            visited.emplace_back(curr, parents[curr], elapsed);
        }
        auto neighbors = g.getNeighbors(curr);
        if (neighbors.size() == 0) {
            continue;
        }

        // Now go through neighbors and add neighbors into priority queue.
        for (std::pair<std::string, double> neighborWeight : neighbors) {
            std::string neighbor = neighborWeight.first;
            double weight = neighborWeight.second;

            if (parents.find(neighbor) == parents.end() || distances.find(neighbor) == distances.end() || fValues.find(neighbor) == fValues.end()) {
                // Neighbor does not yet have an assigned distance or f value
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
    } // Finish processing data
    return visited;
}