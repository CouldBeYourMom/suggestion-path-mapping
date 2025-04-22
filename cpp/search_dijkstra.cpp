#include <chrono>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <tuple>
#include <string>
#include <vector>
#include <utility>
#include "search_dijkstra.h"


/*
 * Perform Dijkstra's algorithm to find the shortest weighted traversal path starting from a given node.
 *
 * Parameters:
 *  - g: The graph to traverse
 *  - startNode: The ID of the starting video node
 *  - maxNodesToVisit: A limit on how many nodes to visit during the traversal
 *  - weightType: Indicates which video stat is being used as edge weight (e.g., flag_count)
 *
 * Returns:
 *  - A vector of (video ID, time elapsed since start) pairs, in order of visit
 */


std::vector<std::pair<std::string, double>> runDijkstra(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
) {

    // Create a vector of visited (video_id, time_since_start_in_ms) and start the clock -- Provided by Carrie
    std::vector<std::pair<std::string, double>> visited;
    auto startTime = std::chrono::steady_clock::now();
    int visitCount = 0;

    // Edge case: If the start node has no neighbors, terminate early
    if (g.getNeighbors(startNode).empty())
      {
        std::cerr << "[ERROR] Start node not found or has no neighbors: " << startNode << std::endl;
        return visited;
      }

    // Setting up hash-map to track which (parent, child) edges have been visited to prevent revisits
    std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs;

    // Setting up Heap with tuple corresponding to distance, name of currentNode, name of parentNode
    std::priority_queue<std::tuple<double, std::string, std::string>> pq;

    // Setting up a distance map to store the shortest known distance from startNode to each node
    std::unordered_map<std::string, double> distance;
    distance[startNode] = 0.0;

    // Begin traversal with the starting node
    pq.push(std::make_tuple(0.0, startNode, ""));


    // Continue traversal until heap is empty or maxNodesToVisit is reached
    while (!pq.empty() && visitCount < maxNodesToVisit)
    {
        // Get the next node with the lowest total distance from the priority queue
        auto [dist, currentNode, parentNode] = pq.top();
        pq.pop();

        // Skip if this (parent → child) edge was already explored
        if (visitedPairs.count({parentNode, currentNode}))
          continue;
        visitedPairs.insert({parentNode, currentNode});

        // Timestamp this visit and record the node -- Provided by Carrie
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(currentNode, elapsed);
        ++visitCount;

        // Look at all neighbors of the current node
        const auto& neighbors = g.getNeighbors(currentNode);
        for (size_t i = 0; i < neighbors.size(); i++)
        {
            std::string neighborId = neighbors[i].first;
            double weight = neighbors[i].second;
            double newDist = dist + weight;

            // If this is a new or shorter path to neighbor, update and push to queue
            if (!distance.count(neighborId) || newDist < distance[neighborId])
            {
                distance[neighborId] = newDist;
                pq.push(std::make_tuple(newDist, neighborId, currentNode));
            }
        }
    }
    return visited;
}