import sqlite3

# Connect to the database
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Ensure 'videos' table exists
cursor.execute("""
    CREATE TABLE IF NOT EXISTS videos (
        id TEXT PRIMARY KEY,
        title TEXT,
        description TEXT
    )
""")

# Create a new table for manually collected related videos
cursor.execute("""
    CREATE TABLE IF NOT EXISTS manual_video_links (
        parent_video TEXT,
        suggested_video TEXT,
        PRIMARY KEY (parent_video, suggested_video),
        FOREIGN KEY (parent_video) REFERENCES videos(id),
        FOREIGN KEY (suggested_video) REFERENCES videos(id)
    )
""")

# Commit changes and close
conn.commit()
conn.close()

print("✅ Database schema updated! Added 'manual_video_links' table.")
