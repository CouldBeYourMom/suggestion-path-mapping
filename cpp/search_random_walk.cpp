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
    auto startTime = std::chrono::steady_clock::now(); // Start the clock
    (void)startTime;

    // Random walk logic with visitedPairs to prevent revisits
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::string current = startNode;
    visited.emplace_back(current, 0.0);

    // Track edges already taken to prevent revisits
    std::unordered_set<std::pair<std::string, std::string>, pair_hash> visitedPairs;
    std::vector<std::string> backtrackStack;           // keep the path for backtracking
    backtrackStack.push_back(current);

    while (static_cast<int>(visited.size()) < maxNodesToVisit) {
        const auto& neighbors = g.getNeighbors(current);
        std::vector<std::pair<std::string, int>> candidates;
        for (const auto& p : neighbors) {
            if (!visitedPairs.count({current, p.first})) {
                candidates.push_back(p);
            }
        }

        if (candidates.empty()) {
            // backtrack until we find a node with an unused edge
            bool found = false;
            while (!backtrackStack.empty() && !found) {
                backtrackStack.pop_back(); 
                if (backtrackStack.empty()) break;
                current = backtrackStack.back();
                const auto& nbrs = g.getNeighbors(current);
                for (const auto& p : nbrs) {
                    if (!visitedPairs.count({current, p.first})) {
                        found = true;
                        break;
                    }
                }
            }
            if (!found) break; // nowhere else to go
            continue;          // re‑evaluate
        }

        int idx = std::rand() % candidates.size();
        std::string next = candidates[idx].first;
        visitedPairs.insert({current, next});
        current = next;
        backtrackStack.push_back(current);

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(now - startTime).count();
        visited.emplace_back(current, elapsed);
    }

    return visited;
}
