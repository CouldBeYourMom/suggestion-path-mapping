import sqlite3

# Connect to (or create) the SQLite database
conn = sqlite3.connect("../data/youtube_data.db")  # Store DB in 'data/' folder
cursor = conn.cursor()

# Create 'videos' table
cursor.execute("""
    CREATE TABLE IF NOT EXISTS videos (
        id TEXT PRIMARY KEY,
        title TEXT,
        description TEXT,
        transcript TEXT,
        flagged INTEGER DEFAULT 0,
        parent_id TEXT
    )
""")

# Create 'comments' table
cursor.execute("""
    CREATE TABLE IF NOT EXISTS comments (
        id TEXT PRIMARY KEY,
        video_id TEXT,
        comment TEXT,
        flagged INTEGER DEFAULT 0,
        FOREIGN KEY(video_id) REFERENCES videos(id)
    )
""")

# Insert test data into 'videos'
cursor.execute("""
    INSERT INTO videos (id, title, description, transcript, flagged, parent_id)
    VALUES ('abc123', 'Test Video', 'This is a test description.', 'Test transcript text.', 0, NULL)
""")

# Insert test data into 'comments'
cursor.execute("""
    INSERT INTO comments (id, video_id, comment, flagged)
    VALUES ('cmt001', 'abc123', 'This is a test comment.', 0)
""")

# Commit changes and close connection
conn.commit()
conn.close()

print("✅ Database setup complete! Test data inserted.")
