import sqlite3
import googleapiclient.discovery
import os
import time
import random
from dotenv import load_dotenv

# Load API Key
load_dotenv()
API_KEY = os.getenv("YOUTUBE_API_KEY")

# Initialize YouTube API client
youtube = googleapiclient.discovery.build("youtube", "v3", developerKey=API_KEY)

# Connect to SQLite database
DB_PATH = "../data/youtube_data.db"
conn = sqlite3.connect(DB_PATH)
cursor = conn.cursor()

# Fetch videos that need stats (skip ones already updated)
cursor.execute("""
    SELECT v.id FROM videos v
    WHERE NOT EXISTS (
        SELECT 1 FROM video_stats vs WHERE vs.video_id = v.id
    )
""")
videos_to_fetch = [row[0] for row in cursor.fetchall()]

print(f"📊 Fetching stats for {len(videos_to_fetch)} videos...")

# Start transaction
conn.execute("BEGIN")

try:
    for video_id in videos_to_fetch:
        try:
            # Fetch video statistics
            request = youtube.videos().list(
                part="statistics",
                id=video_id
            )
            response = request.execute()

            if response["items"]:
                stats = response["items"][0]["statistics"]
                view_count = int(stats.get("viewCount", 0))
                like_count = int(stats.get("likeCount", 0))
                comment_count = int(stats.get("commentCount", 0))

                # Insert or update video stats in the database
                cursor.execute("""
                    INSERT INTO video_stats (video_id, view_count, like_count, comment_count)
                    VALUES (?, ?, ?, ?)
                    ON CONFLICT(video_id) DO UPDATE SET 
                        view_count=excluded.view_count,
                        like_count=excluded.like_count,
                        comment_count=excluded.comment_count,
                        last_updated=CURRENT_TIMESTAMP;
                """, (video_id, view_count, like_count, comment_count))

                print(f"✅ Stats saved for {video_id} - Views: {view_count}, Likes: {like_count}, Comments: {comment_count}")

            # Avoid hitting API rate limits (random delay between 2-5 seconds)
            time.sleep(random.uniform(2, 5))

        except Exception as e:
            print(f"⚠ Could not fetch stats for {video_id} ({str(e)})")

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
