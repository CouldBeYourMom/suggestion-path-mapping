#include "graph.h"
#include "search_dijkstra.h"
#include "search_astar.h"
#include "search_random_walk.h"
#include <iostream>

int main() {
    Graph graph;
    graph.loadGraphFromDB("youtube_graph.db");  // Replace with your actual database path
    
    std::cout << "Graph Structure:\n";
    graph.printGraph();

    std::cout << "\nDijkstra's Shortest Path from A to D:\n";
    auto dijkstraResult = SearchDijkstra::dijkstra(graph, "A", "D");
    for (const auto& node : dijkstraResult) {
        std::cout << node << " ";
    }
    std::cout << "\n";

    std::cout << "\nA* Search Path from A to D:\n";
    auto astarResult = SearchAStar::astar(graph, "A", "D");
    for (const auto& node : astarResult) {
        std::cout << node << " ";
    }
    std::cout << "\n";

    std::cout << "\nRandom Walk from A (5 steps):\n";
    auto randomWalkResult = SearchRandomWalk::randomWalk(graph, "A", 5);
    for (const auto& node : randomWalkResult) {
        std::cout << node << " ";
    }
    std::cout << "\n";

    return 0;
}