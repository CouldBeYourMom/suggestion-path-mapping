# 🛠️ Developer Notes

This document serves as an evolving log of development progress, key decisions, and strategy notes for the YouTube Suggestion Path Mapping project.  
Sections below reflect major functional areas with short, dated updates to track growth over time.

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

**Core Logic**  
- Implemented in C++ using adjacency list
- Each node = video, each edge = parent→suggested relationship
- Designed for multiple traversal strategies

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

---

## 🌐 Visualization

**Purpose**: Show node relationships and flag counts using an interactive force-directed graph

**Stack**:
- `graph.json` → exported from C++
- `3d-force-graph` → JavaScript/Three.js wrapper
- Python's http.server or VSCode Live Server

---

**🗓️ Updates**
- *Apr 9*: Created `visualization/` folder and fully implemented 3D rendering of YouTube graph
- *Apr 9*: Added real-time resizing support and custom styling
- *Apr 9*: Confirmed browser compatibility and deployed local server using Python

---

## 🧪 Testing & Structure

**Strategy**:
- Isolate each major step (collection, flagging, export, viz) for modular testing
- Added command-line flag to export JSON independently
- Stored playlist IDs and added notes to track growth over time

---

**🗓️ Updates**
- *Mar 23*: Added `"export"` mode to `main.cpp` to skip traversal
- *Mar 26*: Added `.gitkeep` to preserve empty folders in repo
- *Apr 9*: Confirmed `.gitignore` behavior for visualization output, DB, and backups

---

## 🚀 Next Steps

**Short-Term**:
- Integrate A*, Dijkstra, and Random Walk into CLI for testable comparison
- Allow traversal results to be exported for overlay in visualization
- Add filters to hide/show nodes by group (Safe, Risky, Extreme)

**Mid-Term**:
- Support GitHub Pages deployment or Netlify for demo
- Support hover-based traversal path preview (with reset button)
- Add snapshot mode (save .png of current state)

---

## 🧠 Observations

- Playlist-based discovery is fast and avoids automation detection
- Visualization helped identify high-flag clusters and bridge nodes
- Many flagged videos are multi-linked and could serve as pivot points for comparison
