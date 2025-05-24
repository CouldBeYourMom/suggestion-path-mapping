import sqlite3
import googleapiclient.discovery
import os
import time
from dotenv import load_dotenv

# Load API Key
load_dotenv()
API_KEY = os.getenv("YOUTUBE_API_KEY")

# Initialize YouTube API client
youtube = googleapiclient.discovery.build("youtube", "v3", developerKey=API_KEY)

# Connect to SQLite database
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Fetch videos missing metadata
cursor.execute("""
    SELECT DISTINCT m.suggested_video, MIN(m.parent_video) as parent_id 
    FROM manual_video_links m
    LEFT JOIN videos v ON m.suggested_video = v.id
    WHERE v.id IS NULL OR v.title IS NULL OR v.description IS NULL
    GROUP BY m.suggested_video
""")
video_data = cursor.fetchall()

print(f"📥 Fetching metadata for {len(video_data)} videos...")

# Start transaction
conn.execute("BEGIN")

try:
    for video_id, parent_id in video_data:
        try:
            # Fetch video metadata (title & description)
            request = youtube.videos().list(
                part="snippet",
                id=video_id
            )
            response = request.execute()

            if response["items"]:
                video_info = response["items"][0]["snippet"]
                title = video_info["title"]
                description = video_info["description"]

                # Insert metadata & parent_id into videos table
                cursor.execute("""
                    INSERT INTO videos (id, title, description, transcript, flagged, parent_id)
                    VALUES (?, ?, ?, NULL, 0, ?)
                    ON CONFLICT(id) DO UPDATE SET 
                        title = COALESCE(excluded.title, title),
                        description = COALESCE(excluded.description, description),
                        parent_id = COALESCE(excluded.parent_id, parent_id)
                """, (video_id, title, description, parent_id))

                print(f"✅ Metadata saved for {video_id}, Parent ID: {parent_id}")
                time.sleep(2)  # Avoid hitting API limits

            else:
                print(f"⏭️ Skipping {video_id} (already in DB)")

        except Exception as e:
            print(f"⚠ Failed to fetch metadata for {video_id} ({str(e)}).")

    # Commit transaction
    conn.commit()
    print("🎉 Transaction committed successfully!")

except Exception as e:
    conn.rollback()
    print(f"🚨 Error occurred: {str(e)} — Rolling back changes!")

finally:
    conn.close()
