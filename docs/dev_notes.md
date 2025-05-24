# 🛠️ Developer Notes

This document serves as an evolving log of development progress, key decisions, and strategy notes for the YouTube Suggestion Path Mapping project.  
Sections below reflect major functional areas with short, dated updates to track growth over time.

---

🧊 **Repo Status:** Finalized for archival and portfolio use.  
All collaborators have been removed, and `main` is now a protected branch.  
For questions, contact [carrieruble@ufl.edu](mailto:carrieruble@ufl.edu)

---


## 📚 Table of Contents
- [📦 Data Collection Pipeline](#-data-collection-pipeline)
- [🔍 Flagging & Dictionary Strategy](#-flagging--dictionary-strategy)
- [🧠 Graph Structure & Algorithms](#-graph-structure--algorithms)
- [🌐 Visualization](#-visualization)
- [🧪 Testing & Structure](#-testing--structure)
- [🚀 Next Steps](#-next-steps)
- [🧠 Observations](#-observations)

---

## 📦 Data Collection Pipeline

**Purpose**: Recursively gather metadata, transcripts, and comments for videos recommended by YouTube starting from a known safe video.

**Strategy**:
- Initially gathered parent → child pairs manually
- Replaced autoplay scraping with curated playlists
- Used YouTube API to extract full lists of video IDs from playlist links
- Later modified the system to support inputting **multiple playlists at once**
- Stored all data in SQLite for flexibility and easy joins

---

**🗓️ Updates**
- *Mar 14*: Stopped Selenium/autoplay scraping due to detection risk and fragility
- *Mar 18*: Manually collected video IDs by clicking through suggestions and pasting IDs
- *Mar 22*: Switched to using **YouTube Playlist IDs** with the API to pull video IDs
- *Mar 25*: Updated playlist fetch script to handle **multiple playlists at once**
- *Apr 8*: Created `run_all.py` to combine metadata fetch, transcript, comment, and flagging steps into a single command

---

## 🔍 Flagging & Dictionary Strategy

**Purpose**: Identify potentially inappropriate videos using a flexible keyword-matching system based on title, description, transcript, and comments.

**Strategy**:
- Dictionary of flagged phrases (including emoji and leetspeak variants)
- Python scripts search each text field and count total matches per video
- Designed for extensibility if using classifier or LLM in future

---

**🗓️ Updates**
- *Mar 17*: Converted initial word list into JSON structure
- *Mar 21*: Added `flag_inappropriate.py` to update `video_flag_counts` in DB
- *Mar 23*: Re-analyzed entire dataset with revised word list (~300 phrases)

---

## 🧠 Graph Structure & Algorithms

**Core Logic**:
- Graph implemented in C++ using adjacency list
- Nodes = video IDs, Edges = parent → suggested links
- Traversals: DFS, A*, Dijkstra, Random Walk
- Edge weights computed via `veraVerto()` transformation for fair traversal scaling

**Traversals**:
- 🔍 A*: Simulates most direct route to high-risk content
- 🧭 Dijkstra: Calculates shortest safe/unsafe paths for risk mapping
- 🎲 Random Walk: Mimics a casual user clicking freely

---

**🗓️ Updates**
- *Mar 23*: Added DFS with stack and refactored `loadGraphFromDB()` to support node weight annotations
- *Mar 25*: Standardized all `getStat()` helper functions for views, likes, flags, comments
- *Mar 26*: Fully implemented `exportFullGraphFromDB()` using SQL JOINs and JSON serialization
- *Apr 9*: Cleaned graph export for 3D visualization compatibility; final testing and inline documentation completed
- *Apr 13–14*: Refactored `graph.cpp` and `graph.h` to finalize traversal logic:
  - Added `visitedPairs` (unordered_set with custom `pair_hash`) to track `(child, parent)` visits. This ensures we allow revisits to nodes through different paths (as needed for YouTube structure), while preventing infinite loops from repeated edge traversals.
  - Updated all `search_*.cpp` stubs with final integration format for team members. Each includes `visitedPairs` scaffolding, elapsed time tracking, and support for `maxNodesToVisit` (optional).
  - Added `userOptions()` and `runSelectedSearch()` for cleaner CLI flow — now supports stat selection and dispatches to the correct traversal with logging built-in.
- *Apr 15*: A*, Dijkstra, and Random Walk implementations completed and tested by team members
- *Apr 16*: Added `veraVerto()` for normalized edge weighting
- *Apr 21*: All teammates added individual algorithm summaries in `docs/`
- *Apr 22*: ✅ Fully integrated all traversal outputs with shared export format and graph logic

---

## 🌐 Visualization

**Purpose**: Visually represent search paths through YouTube’s recommendation network using 3D force-directed rendering.

**Stack**:
- Data: `graph.json` (all nodes/edges) and `search.json` (DFS traversal)
- Frontend: `3d-force-graph` and Three.js via CDN
- Tools: Python's `http.server` or VSCode Live Server

---

**Current Status:**
- ✅ DFS traversal visualized with directional edges
- ✅ Start node marked; visited nodes styled
- ✅ Nodes sized and colored by group (Safe → Extreme)
- ✅ All search types export correctly in a unified format
- ✅ DFS, A*, Dijkstra, and Random Walk all visualize correctly via exportTraversalToJSON()
- ✅ Graph uses directional links and cache-busted fetches to ensure freshness

---

**🗓️ Updates**
- *Apr 9*: Initial rendering pipeline created
- *Apr 14*: Refined export format to track (node, parent)
- *Apr 20*: Styling finalized, node hover info added
- *Apr 21*: Prototype locked and documented for DFS
- *Apr 22*: Visualization documented as DFS-only with future expansion noted
- *Apr 23*: Added cache-busting to all `fetch()` calls to resolve browser refresh issues
- *Apr 23*: Applied cooldown tuning for faster initial graph layout and better responsiveness
- *Apr 23*: Finalized support for all traversals to export `(node, parent, timestamp)` for directional edge rendering

---

## 🧪 Testing & Structure

**Strategy**:
- Modularity: collection, flagging, export, and viz can be run independently
- CLI prompts streamline testing by stat and algorithm
- Each traversal logs CSV output to `data/<searchType>_logs/`

---

**🗓️ Updates**
- *Mar 23*: Added `"export"` mode to `main.cpp` to skip traversal
- *Mar 26*: Added `.gitkeep` to preserve empty folders in repo
- *Apr 9*: Confirmed `.gitignore` behavior for visualization output, DB, and backups
- *Apr 14*: Implemented CSV export functionality for traversal results via `exportResults()`. Output includes:
  - `order`, `timestamp_ms`, `video_id`, `flag_count`, `like_count`, `view_count`, `comment_count`, `title`
  - Total time elapsed recorded at the bottom
  - Filename format: `[searchType]_[timestamp]_[stat].csv`
- *Apr 14*: Created per-search output folders under `data/` (e.g., `dfs_logs/`, `astar_logs/`) and added `.gitkeep` to preserve them in Git.
- *Apr 14*: Added CSV export from `exportResults()`
- *Apr 14–15*: Organized all traversal output by folder and timestamp
- *Apr 21*: Finalized graph cleanup, removed legacy logic, tagged visualization status

---

## 🚀 Next Steps

**Short-Term**:
- [x] Added overlay support for A*, Dijkstra, and Random Walk paths
- [x] Extended traversal export to support `(node, parent)` for all algorithms (DFS, A*, Dijkstra, RW)
- [ ] Add controls to toggle visibility of unvisited nodes or untraversed edges
- [ ] Highlight overlapping paths and cycles in a future visualization pass

**Mid-Term**:
- [ ] Allow interactive comparison of multiple search outputs
- [ ] Add support for weighted path visualizations (e.g., path thickness = comment count)
- [ ] Save/share visualization snapshots as static images or embeddable views

---

## 🧠 Observations

- Playlist-based discovery is fast and avoids automation detection
- Visualization helped identify high-flag clusters and bridge nodes
- Many flagged videos are multi-linked and could serve as pivot points for comparison

---

## 📁 Repo Organization Overview

- `data/youtube_data.db`: Final SQLite dataset (scrubbed, flagged, export-ready)
- `data/README_data.md`: Table schema and ethics/disclaimer documentation
- `docs/SPM_final_report.pdf`: Full project proposal and formal report
- `visualization/`: Graph JSON exports, 3D renderer
- `archive/`: Development-stage scripts and manually gathered seed files
- `compile_graph.py`: Unified compile/run/test pipeline

---

## 🏁 Final Commit Note

This repo is now frozen as a showcase artifact.  
All traversal logic, data exports, and documentation are complete and verified.  
Future improvements or explorations will be forked and built elsewhere.