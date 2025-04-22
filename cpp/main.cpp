#include "graph.h"
#include <iostream>

/*
Entry point of the program.
Instantiates a Graph object and launches user interaction.

userOptions() handles:
 - Search algorithm selection (A*, Dijkstra, Random Walk, DFS, etc.)
 - Weighting stat selection (likes, flags, views, etc.)
 - Graph building from database
 - Search execution and result export
*/

int main() {
    Graph g;
    g.userOptions();
    return 0;
}