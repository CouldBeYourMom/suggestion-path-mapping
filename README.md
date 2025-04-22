# 📺 Suggestion Path Mapping

## 🎯 Project Overview

This project explores how YouTube’s suggestion system can lead viewers — particularly younger users — from **family-friendly content** to videos with potentially inappropriate language or themes. We start from a video known to be child-appropriate and recursively collect metadata, transcripts, and comments from suggested videos.

Each video is evaluated using a dictionary of inappropriate words/phrases curated with input from mental health professionals and concerned parents. We flag matches found in titles, descriptions, transcripts, and comments, then use a graph structure to map how videos are connected through YouTube’s recommendations.

## 🛠️ How It Works

- 🧠 **Graph Structure (C++)**:  
  Videos are nodes; suggested links are directed edges.

- 🐍 **Data Collection (Python)**:  
  Gathers metadata, comments, and transcripts using the YouTube API and stores them in a local SQLite database.

- 🚩 **Flagging System**:  
  Compares video content against a growing dictionary of inappropriate language. Re-analysis is supported when the dictionary is updated.

- 🌌 **3D Visualization**:  
  The graph is exported as `graph.json` and rendered with [3d-force-graph](https://github.com/vasturiano/3d-force-graph).

- 🔧 **Technologies Used**:  
  Python · SQLite · C++ · VSCode · Git · JavaScript + Three.js

## 🧪 Algorithms (In Progress)

We're designing traversal algorithms to simulate how a viewer might progress from safe to unsafe content, in order to **compare their behavior** and **evaluate risk** within the graph structure:

- 🔍 **A\***: Finds the shortest "path to corruption" — from a known-safe video to one flagged for inappropriate content.
- 🧭 **Dijkstra’s**: Maps broader clusters of video suggestions with increasing risk.
- 🎲 **Random Walks**: Mimics a naive user (like a child) clicking randomly through recommendations.

We’re also designing the structure so that future semantic analysis (via LLMs or classifiers) can be plugged in easily.

## 📂 Project Structure

```plaintext
data/           ← SQLite DB, flag dictionary
scripts/        ← Python scripts for collection & flagging
cpp/            ← Graph + algorithm implementations (C++)
visualization/  ← 3D rendering using ForceGraph3D + JSON export
backups/        ← Daily zipped backups of the DB
archive/        ← Deprecated or legacy scripts
docs/           ← Dev notes, algorithm summaries, and HowToRun instructions
```

## 🔍 Key Features

- ✅ Modular, recursive data collection
- ✅ Real-time flagging with an updatable dictionary
- ✅ Multiple traversal algorithms (testable + swappable)
- ✅ JSON graph export for visualization
- ✅ Auto-rotation of DB backups
- ✅ Clean Git structure for collaboration

## 📝 Extended Notes

For design decisions, collection strategies, and team-specific workflows, see `docs/dev_notes.md`. This file serves as a running log of project milestones and pivots.

## 🚀 Getting Started

### ✅ Prerequisites

- Python 3.10+  
- C++17 or later  
- SQLite3  
- YouTube Data API v3 key  
- VSCode with Python & C++ extensions recommended

### 🔧 Setup

```bash
$ git clone https://github.com/YOUR_USERNAME/suggestion-path-mapping
$ cd suggestion-path-mapping
$ cp .env.example .env      # Add your YouTube API key
$ python scripts/run_all.py # Populate the database (see dev_notes for options)
```

### 🚦 Full Run Instructions

To compile and run the graph, launch the interactive CLI, and view results in the 3D visualization:

```bash
$ python compile_graph.py      # Compiles C++ files and launches graph.exe
$ python -m http.server 5500   # Launches visualization server (or use Live Server)
```

## 🙌🏻 Acknowledgements

- Built for **COP3530: Data Structures & Algorithms**
- Mental health professionals who contributed to the inappropriate word list
- YouTube API for enabling this kind of research
- Our teammates for bringing their skills and interests to this project:
  - Yepeth Berhie (@Y-Berhie)  
  - Carrie Ruble (@CouldBeYourMom)  
  - Adam Schwartz (@schwartza-afs)  
  - Kevin Yu (@kevinyu0)  
- Visualization powered by [3d-force-graph](https://github.com/vasturiano/3d-force-graph)
