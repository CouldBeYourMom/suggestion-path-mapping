#include "search_random_walk.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <unordered_set>

std::vector<std::pair<std::string, double>> runRandomWalk(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
) {
    // Keep these following 3 lines here
    std::vector<std::pair<std::string, double>> visited; // visited : vector of (video_id, time_since_start_in_ms)
    auto startTime = std::chrono::steady_clock::now(); 
    (void)startTime;

    // Random walk logic with visitedPairs to prevent revisits
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::string current = startNode;
    visited.emplace_back(current, 0.0);

    // Track edges already taken to prevent revisits
    std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs;

    for (int i = 1; i < maxNodesToVisit; ++i) {
        const auto& neighbors = g.getNeighbors(current);
        if (neighbors.empty()) break;
        std::vector<std::pair<std::string, int>> candidates;
        for (const auto& p : neighbors) {
            if (!visitedPairs.count({current, p.first})) {
                candidates.push_back(p);
            }
        }
        if (candidates.empty()) break;

        int idx = std::rand() % candidates.size();
        std::string next = candidates[idx].first;

        visitedPairs.insert({current, next});
        current = next;

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(current, elapsed);
    }

    return visited;
}
