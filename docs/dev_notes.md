# 🛠️ Development Notes & Strategy Log

This document tracks decisions, iterations, and insights made throughout the YouTube Graph Analysis project. It’s part journal, part reference, and part scratchpad for design thinking.

---

## 📅 Table of Contents

- [Overview](#overview)
- [Video Collection Strategy](#video-collection-strategy)
- [Profanity Dictionary & Flagging System](#profanity-dictionary--flagging-system)
- [Database Schema Decisions](#database-schema-decisions)
- [Graph Structure & Algorithms](#graph-structure--algorithms)
- [Language Model Possibilities](#language-model-possibilities)
- [Backup & Logging System](#backup--logging-system)
- [Roadblocks & Pivots](#roadblocks--pivots)
- [Future Ideas](#future-ideas)
- [Scratchpad](#scratchpad)

---

## 📌 Overview

Originally inspired by concerns about how kids might encounter inappropriate content through YouTube's recommendation system, this project aims to model that experience and analyze it using graph traversal techniques. While we're working with simple detection methods now, the architecture is designed to support more advanced analysis in the future.

---

## 🎥 Video Collection Strategy

- **Initial Approach**: Planned to use autoplay tracking with Selenium but switched due to YouTube's automation detection.
- **Current Approach**: Manually curated seed playlists. Store video IDs + their recommendations in `manual_video_links`.
- Notes:
  - Ensures data collection is repeatable and doesn't violate TOS
  - Seed videos are marked "Made for Kids" for cleaner analysis baseline
- ✅ Works with transcripts, comments, and metadata consistently

---

## 🚩 Profanity Dictionary & Flagging System

- Built as a `JSON` file with words grouped by category
- Includes flexible matching with regular expressions (e.g. boundary checking for "ass")
- Applied to:
  - Video descriptions
  - Transcripts
  - Comments (first 10 per video)
- Design Decision:
  - `videos.flagged` and `comments.flagged` store count of matched terms
  - `video_flag_counts` summarizes total flags per video (via script, not trigger)

---

## 🗃️ Database Schema Decisions

- All data is stored in SQLite (`youtube_data.db`)
- Key tables:
  - `videos`, `comments`, `manual_video_links`, `video_stats`, `video_flag_counts`
- Dropped triggers due to inconsistent behavior with recursive updates
- Added `backup_db.py` script with:
  - Zip compression
  - Log rotation
  - Max backup retention (10 backups)

---

## 🧠 Graph Structure & Algorithms

- Implemented in C++ with adjacency list
- Each node = video, edge = recommendation
- Traversals:
  - `A*`: Ideal for simulating direct paths to flagged content
  - `Dijkstra`: Helps visualize risk spread / clustering
  - `Random Walk`: Simulates casual browsing
- Integration pending after data collection is complete

---

## 🧠 Language Model Possibilities

- One teammate proposed using a small language model to assess:
  - Video sentiment
  - Transcript readability
  - Semantic tone
- We may not implement this before the final presentation, but the project is designed to grow in that direction

---

## 💾 Backup & Logging System

- Runs via `scripts/backup_db.py`
- Features:
  - Safe SQLite `.backup()` call
  - Timestamped `.zip` output
  - Auto deletes oldest backups after 10
  - Logs stored in `backups/backup.log`

---

## 🔄 Roadblocks & Pivots

- ❌ Selenium autoplay blocked by YouTube → ✅ switched to manual playlist strategy
- ❌ Triggers caused inconsistent counts → ✅ replaced with end-of-script summary update
- ❌ Comment scraping initially too aggressive → ✅ limited to top 10 per video

---

## 🧭 Future Ideas

- Use thumbnails / metadata for clickbait detection
- Add YouTube Kids API (if accessible)
- Introduce user simulation profiles (e.g., cautious vs. curious user)
- Connect graph metrics to time-based trends
- Implement dashboard-style visualizations
- Train profanity model or LLM-based detector

---

## ✍️ Scratchpad

Things that don’t fit neatly elsewhere, random ideas, things to revisit:

- Idea: Graph color-coding based on content category?
- Revisit: Should we use timestamps in transcripts to flag *when* inappropriate content appears?
- Potential Bug: Dijkstra sometimes returns tie paths — should we randomize or pick shortest ID?

---

