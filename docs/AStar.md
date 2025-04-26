# A* Search Algorithm

## Introduction
This document contains information regarding the implementation of 
the A* search algorithm in this project. It briefly describes the logic
behind our implementation, as well as the heuristic function and our implementation's
worst-case runtime.

## Implementation Summary
Our project's implementation of A* takes a start node as an input and visits
a certain number of nodes as specified in the input. One of its inputs is
the graph object as well, containing both video ids and edge weights that are
defined based on likes, view counts, or number of flags. The goal of the algorithm
is to reach a highly flagged video (thereby simulating a child who 
unknowingly stumbles upon a highly inappropriate video).

Our implementation begins by taking the starting node and inserting it into 
a priority queue. The priority queue functions as a min heap, and the priority
of each node is calculated using the following formula: $f(n) = 
g(n) + h(n)$, where $g$ represents the distance to a node $n$ and $h$ is the heuristic 
function (more on this below).

In each iteration of the algorithm's loop, we extract the node at the front of 
the priority queue and add it to a set of visited nodes. Afterwards,
we examine the neighboring nodes and calculate $f(n)$ for each neighbor. If 
the tentative value of $f(n)$ is smaller than whatever value has already been
calculated for this node, then the neighbor's value is updated, as well as 
its parent.


## The heuristic function
The heuristic function in our implementation examines the edges going out of a 
node and selects the smallest edge weight as the heuristic value. The reason
our heuristic function selects the smallest value is to avoid overestimating
the remaining distance to a highly flagged video.

## Runtime
In the worst case scenario, the time complexity is $O(EV*log(V))$. In the worst
case scenario, the algorithm must repeatedly update the priority queue
by removing and/or inserting nodes for every edge in the graph. Moreover,
the heuristic function runs in every iteration as well and has a worst-case runtime
of $O(V)$ due to the fact that it must examine all neighboring vertices.

