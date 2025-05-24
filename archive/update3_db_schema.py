import sqlite3

# Connect to the database
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Start a transaction
conn.execute("BEGIN")

try:
    # Drop redundant table if it exists
    cursor.execute("""
        DROP TABLE IF EXISTS video_links
    """)

    # Populate videos.parent_id based on manual_video_links
    cursor.execute("""
        UPDATE videos
        SET parent_id = (
            SELECT parent_video 
            FROM manual_video_links 
            WHERE suggested_video = videos.id
        )
        WHERE EXISTS (
            SELECT 1 FROM manual_video_links WHERE suggested_video = videos.id
        )
    """)

    # Commit changes if everything succeeds
    conn.commit()
    print("✅ Database schema updated! 'video_links' table dropped and videos.parent_id updated.")

except Exception as e:
    # Rollback changes if an error occurs
    conn.rollback()
    print(f"🚨 Error updating database: {str(e)} — Rolling back changes!")

finally:
    # Close database connection
    conn.close()
