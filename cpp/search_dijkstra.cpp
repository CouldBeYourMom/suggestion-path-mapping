#include <chrono>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <tuple>
#include <string>
#include <vector>
#include <utility>
#include "search_dijkstra.h"

// This replaces your old SearchDijkstra::dijkstra() and gives you the final format for team integration
std::vector<std::pair<std::string, double>> runDijkstra(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
) {
    // Setting up return vector and starting clock -- Provided by Carrie
    std::vector<std::pair<std::string, double>> visited; // visited : vector of (video_id, time_since_start_in_ms)
    auto startTime = std::chrono::steady_clock::now(); // Start the clock
    int visitCount = 0;

    // Check to see if our starting video has neighbors to visit
    if (g.getNeighbors(startNode).empty())
      {
        std::cerr << "[ERROR] Start node not found or has no neighbors: " << startNode << std::endl;
        return visited;
      }

    // Hash-map to ensure we don't visit the same Vi->Vj edge -- Provided by Carrie
    std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs;

    // Setting up Heap with tuple corresponding to distance, name of currentNode, name of parentNode
    std::priority_queue<std::tuple<double, std::string, std::string>> pq;

    // Setting the starting node distance to 0
    std::unordered_map<std::string, double> distance;
    distance[startNode] = 0.0;

    // Negating the distance, so max-heap will behave like a min-heap
    pq.push(std::make_tuple(-0.0, startNode, ""));

    // Loop until no more videos, or we reach maximum number for visit count
    while (!pq.empty() && visitCount < maxNodesToVisit)
    {
        // Pop the top of the heap and setup variables for its data
        auto [negDist, currentNode, parentNode] = pq.top();
        pq.pop();

        // Flipping sign back to regular distance
        double currentDist = -negDist;

        // Check if Vi->Vj edge has been done before
        if (visitedPairs.count({currentNode, parentNode}))
          continue;
        visitedPairs.insert({currentNode, parentNode});

        // Record the time and update the visit count -- Provided by Carrie
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(currentNode, elapsed);
        ++visitCount;

        // Get neighbors for currentNode and push them onto the Heap
        const auto& neighbors = g.getNeighbors(currentNode);
        for (size_t i = 0; i < neighbors.size(); i++)
        {
            std::string neighborId = neighbors[i].first;
            int weight = neighbors[i].second;
            double newDist = currentDist + static_cast<double>(weight);
            if (!distance.count(neighborId) || newDist < distance[neighborId])
            {
                distance[neighborId] = newDist;

                // Push negative distance to keep min-heap behavior
                pq.push(std::make_tuple(-newDist, neighborId, currentNode));
            }
        }
    }
    return visited;
}