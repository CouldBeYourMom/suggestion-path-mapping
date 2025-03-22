import sqlite3
from youtube_transcript_api import YouTubeTranscriptApi
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
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Fetch only videos that have NOT been checked (transcript is NULL)
cursor.execute("""
    SELECT id FROM videos 
    WHERE transcript IS NULL
""")
videos_to_fetch = cursor.fetchall()

print(f"📥 Fetching transcripts for {len(videos_to_fetch)} new videos...")

# Start transaction
conn.execute("BEGIN")

try:
    for (video_id,) in videos_to_fetch:
        try:
            # Fetch transcript
            transcript = YouTubeTranscriptApi.get_transcript(video_id)
            transcript_text = " ".join([entry["text"] for entry in transcript])

            # Update transcript in the videos table
            cursor.execute("UPDATE videos SET transcript = ? WHERE id = ?", (transcript_text, video_id))

            print(f"✅ Transcript saved for {video_id}")

            # Avoid hitting API rate limits (random delay between 10-30 seconds)
            time.sleep(random.randint(30, 55))

        except Exception as e:
            print(f"⚠ No transcript available for video {video_id} ({str(e)}). Storing '0' to prevent rechecking.")

            # Store '0' in transcript column so we don't check it again
            cursor.execute("UPDATE videos SET transcript = '0' WHERE id = ?", (video_id,))

    # Commit the transaction **only if all updates succeed**
    conn.commit()
    print("🎉 Transaction committed successfully!")

except Exception as e:
    # Rollback if anything goes wrong
    conn.rollback()
    print(f"🚨 Error occurred: {str(e)} — Rolling back changes!")

finally:
    # Close database connection
    conn.close()
