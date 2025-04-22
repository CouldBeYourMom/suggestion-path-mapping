
---

## 🏃‍♂️ How to Run This Project (Required for Grading)

To run this project locally and view both the traversal output and the 3D graph visualization:

---

### 📦 1. Install Requirements

```bash
pip install -r requirements.txt
```

Make sure to create a `.env` file with your YouTube API key (see `.env.example`).

---

### 📁 2. Use the Included Synthetic Database

This repository includes a file named:

```plaintext
data/synthetic_youtube_data.db
```

Before compiling, ensure the correct filename is specified in **two places** inside `graph.cpp`:
1. Inside `userOptions()`, update:
   ```cpp
   std::string dbPath = "data/synthetic_youtube_data.db";
   ```
2. Inside `createGraphFromDB()`, verify that `sqlite3_open()` uses the correct path.

---

### 🧠 3. Populate the Graph from the DB

```bash
python scripts/run_all.py
```

This step will flag the videos and generate outputs based on the test dataset.

---

### ⚙️ 4. Compile & Run the C++ Graph

```bash
bin/graph.exe  # Prompts user to select stat + traversal type
```

✅ DFS, A*, Dijkstra, and Random Walk are fully implemented.  
🔍 All four traversals (DFS, Dijkstra, A, Random Walk) now export correctly. DFS is fully visualized; others are export-ready.*

---

### 🌐 5. Launch the 3D Visualization (DFS Only)

```bash
cd visualization/
python -m http.server 5500       # or use Live Server in VSCode
```

Open your browser and go to:

> http://localhost:5500

✅ `graph.json` = full graph  
✅ `search.json` = DFS traversal path and overlay


---

### 🐍 6. Optional: Use Python Script to Compile and Launch (Cross-Platform)

Just run this:

```bash
python compile_graph.py
```

This script will:
1. Compile the C++ source files
2. Run the traversal program with CLI prompts
3. Launch a local server on port 5500 to visualize the DFS traversal

✅ `compile_graph.py` is located in the root directory.

---
### 🖥️ Terminal Compatibility

`compile_graph.py` has been tested and works in:

- ✅ PowerShell
- ✅ MSYS2 UCRT64 terminal
- ⚠️ Not currently compatible with MinGW64 (graph.exe may not launch properly)