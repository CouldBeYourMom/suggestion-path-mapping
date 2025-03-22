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

# Fetch videos that need comments
cursor.execute("""
    SELECT DISTINCT v.id, v.parent_id FROM videos v
    JOIN manual_video_links mvl ON v.id = mvl.suggested_video
    WHERE NOT EXISTS (
        SELECT 1 FROM comments c WHERE c.video_id = v.id
    )
""")
videos_to_fetch = cursor.fetchall()

print(f"📥 Fetching comments for {len(videos_to_fetch)} videos...")

# Start transaction
conn.execute("BEGIN")

try:
    for video_id, parent_id in videos_to_fetch:
        try:
            # Fetch video metadata (title & description)
            request = youtube.videos().list(
                part="snippet",
                id=video_id
            )
            response = request.execute()

            if response["items"]:
                video_data = response["items"][0]["snippet"]
                title = video_data["title"]
                description = video_data["description"]

                # Insert video info into videos table if not exists
                cursor.execute("""
                    INSERT OR IGNORE INTO videos (id, parent_id, title, description)
                    VALUES (?, ?, ?, ?)
                """, (video_id, parent_id, title, description))

            # Fetch comments
            request = youtube.commentThreads().list(
                part="snippet",
                videoId=video_id,
                maxResults=10  # Limit to first 10 comments
            )
            response = request.execute()

            comment_count = 0
            for item in response.get("items", []):
                comment = item["snippet"]["topLevelComment"]["snippet"]["textOriginal"]
                cursor.execute("INSERT INTO comments (video_id, comment) VALUES (?, ?)", (video_id, comment))
                comment_count += 1

            print(f"✅ Comments saved for {video_id} ({comment_count} total)")

            # Avoid hitting API rate limits
            time.sleep(2)

        except Exception as e:
            print(f"⚠ Comments are disabled for video {video_id} ({str(e)}).")

    # Commit the transaction **only if all operations succeed**
    conn.commit()
    print("🎉 Transaction committed successfully!")

except Exception as e:
    # Rollback if anything goes wrong
    conn.rollback()
    print(f"🚨 Error occurred: {str(e)} — Rolling back changes!")

finally:
    # Close database connection
    conn.close()
