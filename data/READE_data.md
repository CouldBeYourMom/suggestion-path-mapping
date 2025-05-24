# 📊 YouTube Suggestion Graph Dataset

This file documents the schema and usage of the `youtube_data.db` SQLite database used in the Suggestion Path Mapping project. This database models metadata, relationships, and risk analysis of recommended videos on YouTube, collected for academic research purposes.

---

## 📦 Database Schema

### Tables

#### `videos`
Stores video-level metadata.

| Column        | Type   | Description |
|---------------|--------|-------------|
| `id`          | TEXT (PK) | YouTube video ID |
| `title`       | TEXT   | Video title |
| `description` | TEXT   | Video description |
| `transcript`  | TEXT   | Full transcript text (if available) |

---

#### `video_stats`
Holds per-video statistics.

| Column         | Type    | Description |
|----------------|---------|-------------|
| `video_id`     | TEXT (PK) | Foreign key to `videos.id` |
| `view_count`   | INTEGER | View count |
| `like_count`   | INTEGER | Like count |
| `comment_count`| INTEGER | Comment count |

---

#### `video_flag_counts`
Stores flag detection data based on keyword analysis.

| Column       | Type    | Description |
|--------------|---------|-------------|
| `video_id`   | TEXT (PK) | Foreign key to `videos.id` |
| `age_flags`  | INTEGER | Number of age-inappropriate content flags detected |
| `danger_flags` | INTEGER | Number of danger/harm-related terms detected |
| `total_flags` | INTEGER | Sum of all matched flag keywords |

---

#### `comments`
Stores user comment text per video.

| Column     | Type    | Description |
|------------|---------|-------------|
| `id`       | INTEGER (PK) | Internal DB row ID |
| `video_id` | TEXT   | Foreign key to `videos.id` |
| `comment`  | TEXT   | Comment text (author info removed) |

---

#### `manual_video_links`
Tracks explicit video-to-video suggestion links (parent → child), either collected manually or inferred.

| Column           | Type | Description |
|------------------|------|-------------|
| `parent_video`   | TEXT | Source video (suggesting) |
| `suggested_video`| TEXT | Destination video (suggested) |
| `priority`       | INTEGER | Optional weight for sorting or grouping links |

---

#### `sqlite_sequence`
Standard SQLite autoincrement table (used internally for `comments.id`)

---

### View

#### `video_edges_full`
This view joins `manual_video_links` with `videos` to provide a full edge list with titles.

| Column          | Description |
|-----------------|-------------|
| `parent_video`  | Source video ID |
| `suggested_video` | Destination video ID |
| `v.title`       | Destination video title |

---

> 📌 Note: The schema was designed to allow for flexible joins and filtered graph exports, including node-level statistics and directional edge weight transformations.

---

### `bfs_logs/`, `dfs_logs/`, `astar_logs/`, `dijkstra_logs/`, `randomwalk_logs/`
Subdirectories that contain CSV logs of traversal output:
- Each CSV contains: `order`, `timestamp_ms`, `video_id`, `flag_count`, `like_count`, `view_count`, `comment_count`, `title`
- Filenames are timestamped for reproducibility and batch analysis.

---

## 🛡️ Data Ethics & Usage Disclaimer

This database contains publicly accessible metadata retrieved via the YouTube API and curated playlist navigation. It is for academic and research purposes only. No personal user data was stored or shared.

- No private user information was accessed or stored.
- Comments and transcripts were stripped of author IDs.
- No full videos were downloaded or redistributed.
- Data was obtained using the YouTube API and custom logic for educational use.

This dataset is shared to support **non-commercial algorithmic research**, particularly in media recommendation safety, visualization, and path mapping.

If you are a researcher or platform representative with concerns or questions, please contact:  
📧 [carrieruble@ufl.edu](mailto:carrieruble@ufl.edu)

---

## 💡 Suggested Use Cases

- Graph traversal and recommendation system simulation
- Keyword flagging benchmark design
- Network visualization and structure analysis
- Transcript/metadata NLP and flag-rate modeling
- Algorithmic content safety audits
- Visualization tools for educational walkthroughs
- Natural language analysis on titles, transcripts, descriptions, and comments

---

© Carrie Ruble, University of Florida, 2025  
See repository [LICENSE](../LICENSE) for terms.
