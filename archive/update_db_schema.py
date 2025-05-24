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

# Create 'video_links' table to store parent-child relationships
cursor.execute("""
    CREATE TABLE IF NOT EXISTS video_links (
        source_video TEXT,
        target_video TEXT,
        PRIMARY KEY (source_video, target_video),
        FOREIGN KEY (source_video) REFERENCES videos(id),
        FOREIGN KEY (target_video) REFERENCES videos(id)
    )
""")

# Commit changes and close
conn.commit()
conn.close()

print("✅ Database schema updated successfully!")
