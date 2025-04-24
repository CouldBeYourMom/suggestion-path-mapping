# Dijkstra Traversal – Code Overview

This is a quick walkthrough of our implementation of the `runDijkstra` function inside `search_dijkstra.cpp`.
It uses Dijkstra’s algorithm to explore YouTube’s recommendation graph and figure out how fast users might get to certain videos based on engagement stats.

## Function Signature

```
std::vector<std::tuple<std::string, std::string, double>> runDijkstra(
    const Graph& g,
    const std::string& startNode,
    int maxNodesToVisit,
    const std::string& weightType
);
```


## What It Does

We’re returning a vector of tuples where each tuple has:
- The current video ID
- The parent video it came from
- The time (in milliseconds) since the traversal started when we got to it

This helps us trace the path and timing of the traversal in CSV format.

## Key Parts of the Code

### 1. The `visited` Vector
We build up a vector of visited nodes as we go, recording each one with the parent and timestamp.

### 2. Starting the Clock
We use `std::chrono` to start a timer so we can measure how long it takes to reach each video.

### 3. Priority Queue
We use a min-heap (priority queue) that stores a tuple of:
- Current total weight to reach the node
- Current video
- Parent video

This makes sure the lowest-weight paths get explored first.

### Heap Behavior with `veraVerto` Transformation

To calculate edge weights, we apply a transformation to the raw stat values using a helper function called `veraVerto`.
This compresses extreme values (like millions of views or hundreds of flags) into smaller float-based weights.


```
double veraVerto(int raw, int maxStat) {
    double x = static_cast<double>(raw) + 1.0;
    double m = static_cast<double>(maxStat) + 1.0;
    return 1000000000 * (std::log(x) / m);
}
```


By using a log scale and normalizing with the max stat value, we reduce bias from outliers and make the weights easier to compare.
Since the values are transformed before entering the queue, the min-heap still prioritizes lower cumulative weights correctly — keeping Dijkstra’s behavior intact.

### 4. Distance Map
We track the shortest known distance to each video so we don’t waste time on longer paths.

### 5. Visited Edges
We keep track of visited `(parent → child)` edges using a hash set so we don’t loop back on ourselves.

## Main Traversal Loop

- Start with the initial video at distance 0
- Keep looping while the priority queue isn’t empty, and we haven’t hit our node visit limit
- For each node:
    - If we’ve already visited the same edge, skip it
    - Otherwise, timestamp the visit and add it to our result
    - Then check all the neighbors of that node
    - If we find a new or better path to a neighbor, push it to the queue

## Return Value

Returns a vector of:

```
tuple<video_id, parent_id, timestamp_ms>
```

This gets written to a CSV for analysis later.

## Error Handling

If the start node has no neighbors, we print an error and return an empty result.

## Final Notes

This version is designed to work with different weighting stats like flag count, likes, or views. The `weightType` parameter controls that, and the graph builds the edge weights before calling this function.
