#include "graph.h"
#include<iostream>

/*
 * Entry point of the program.
 * Instantiates a Graph object and prompts user interaction.
 * The userOptions() function handles:
 *  - Search algorithm selection (A*, Dijkstra, Random Walk, DFS, etc.)
 *  - Weighting stat selection (likes, flags, views, etc.)
 *  - Graph building and traversal
 */
int main() {
    Graph g;
    g.userOptions();  // Launch menu for graph setup and search
    return 0;
}