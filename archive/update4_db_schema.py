import sqlite3

# Database path
DB_PATH = "../data/youtube_data.db"

# Connect to SQLite database
conn = sqlite3.connect(DB_PATH)
cursor = conn.cursor()

# Create video_stats table (if it doesn't exist)
cursor.execute("""
CREATE TABLE IF NOT EXISTS video_stats (
    video_id TEXT PRIMARY KEY,
    view_count INTEGER,
    like_count INTEGER,
    comment_count INTEGER,
    last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (video_id) REFERENCES videos(id)
);
""")

# Commit and close
conn.commit()
conn.close()

print("✅ Database schema updated successfully!")
