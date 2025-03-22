# 📺 Suggestion Path Mapping

## 🎯 Project Overview

This project explores how YouTube's suggestion system can lead viewers — particularly younger users — from safe, kid-friendly content to videos with potentially inappropriate language or themes. We start from a video marked "Made for Kids" and recursively collect metadata, transcripts, and comments from suggested videos.

Each video is evaluated using a dictionary of inappropriate words/phrases curated with input from mental health professionals and concerned parents. We flag matches found in titles, descriptions, transcripts, and comments, then use a graph structure to map how videos are connected through YouTube’s recommendations.

## 🔧 How It Works

- 🧠 **Graph Structure (C++)**: Videos are nodes; suggested links are directed edges.
- 🐍 **Data Collection (Python)**:
  - Pulls metadata, comments, and transcripts using the YouTube API
  - Stores everything in a local SQLite database
- 🚩 **Flagging System**:
  - Compares content against a growing dictionary of inappropriate language
  - Supports re-analysis after dictionary updates
- 🗺️ **Traversal Algorithms**:
  - Designed to simulate how a viewer might progress from safe to unsafe content
  - Implemented in C++ for performance and learning purposes

## 🧪 Algorithms (In Progress)

We're experimenting with:

- 🔍 **A\***: Find the shortest "path to corruption" from a starting video to one flagged for inappropriate content.
- 🧭 **Dijkstra’s**: Map broader clusters of inappropriate video recommendations.
- 🎲 **Random Walks**: Simulate a naive user (like a child) clicking through recommendations.

We’ve also discussed using language models to analyze semantic content — while that feature may not make it into the final demo, we’re designing the project so it can grow into that direction in the future.

## 🗃️ Project Structure

```plaintext
data/               ← SQLite DB, flag dictionary, video ID lists
scripts/            ← Python data collection + processing
cpp/                ← Graph + traversal algorithm implementations
backups/            ← Auto-generated daily database backups (.zip)
archive/            ← Retired scripts preserved for transparency
docs/               ← dev_notes.md and future extended documentation
```

## 🗃️ Key Features

- ✅ Modular data collection
- ✅ Recursion-aware transcript & comment fetching
- ✅ Real-time flagging based on evolving criteria
- ✅ Automated database backups with rotation & logging
- ✅ Clean Git structure for collaboration

## 📝 Extended Notes & Strategy Log

For deeper project decisions, collection strategy changes, and algorithm development logs, see docs/dev_notes.md. This doc complements our final presentation and report but focuses on process over polish.

## 🚀 Getting Started

These instructions will help you set up the project locally for development and testing.

### ✅ Prerequisites

- Python 3.10+  
- C++17 or later (for graph traversal algorithms)  
- SQLite3  
- YouTube Data API v3 key  
- VSCode with Python & C++ extensions recommended

### 🔧 Installation

Clone the repository and install required Python packages.

```bash
$ git clone https://github.com/YOUR_USERNAME/YouTube-Graph-Analysis.git
$ cd YouTube-Graph-Analysis
$ cp .env.example .env
```

### 🙌🏻 Acknowledgements

- Mental health professionals who contributed to the inappropriate word list
- YouTube API for making this dataset feasible
- Our teammates for bringing their own specialities and interests into this collaboration