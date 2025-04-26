✅ Random Walk Algorithm: Embracing Exploration Over Optimization

In our graph, a random walk mimics the way a curious viewer might follow suggestions without a specific destination — moving from one video to the next without a clear strategy, and potentially circling back when stuck. By default, out implementation is stat-weighted. A weight is derived from flags, likes, views, or comments. The lower an edge's transformed weight the more likely it is to be chosen. 

Unlike Dijkstra’s algorithm, which follows the shortest path to a goal, or DFS/BFS, which follow strict structural orders, the random walk meanders — selecting among options at random and backtracking only when no other moves are available.

🧩 How It Works

Let’s say we start at node D, which has 5 outbound suggestions (with example weights in parentheses):

D ➝ L  (0.52)
D ➝ M  (1.11)
D ➝ N  (0.07)
D ➝ O  (1.66)
D ➝ P  (0.01)

Draw r ∈ [0, T] and walk through the list, summing weights until the running total ≥ r.

Traverse that edge *e.g., a small r is likely to pick D ➝ P because of its tiny weight*

Repeat from

Repeat from the new node, backtracking only when every outgoing edge has already been used.

This continues until either:

- The walk reaches the `maxNodesToVisit`, **or**
- It reaches a node where **all outbound edges have already been used**.

In that case, the algorithm backtracks to the last node with unused edges and continues walking from there.

🔁 Key Features of the Algorithm

| Feature                    | Behavior                                                                |
|----------------------------|-------------------------------------------------------------------------|
| Stat‑weighted selection    | The lower transformed weight the higher pick probability                |
| Edge revisit prevention    | Tracks visited edges (not just nodes) to avoid duplicate exploration    |
| Backtracking               | Returns to prior nodes only when stuck (no available forward moves)     |
| Non-deterministic output   | Different results on each run due to randomness                         |

⚠️ Not Your Typical Search Algorithm

This algorithm **does not guarantee** reaching a particular goal, nor does it ensure all reachable nodes will be visited. That’s intentional — it prioritizes organic exploration and models spontaneous user behavior more than exhaustive traversal.

✅ What We Actually Get

The behavior is ideal for simulating viewer paths or testing traversal diversity:

Edge   | Traversal Allowed?  | Reason
-------|---------------------|--------
D ➝ N |         ✅          | Chosen if random draw falls in N's weight range
