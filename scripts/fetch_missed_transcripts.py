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

# Fetch only videos where transcript = '0' (indicating a previous failed attempt)
cursor.execute("""
    SELECT id FROM videos 
    WHERE transcript = '0'
""")
videos_to_retry = cursor.fetchall()

print(f"🔁 Retrying transcript fetch for {len(videos_to_retry)} videos...")

# Start transaction
conn.execute("BEGIN")

try:
    for (video_id,) in videos_to_retry:
        try:
            # Attempt to fetch transcript again
            transcript = YouTubeTranscriptApi.get_transcript(video_id)
            transcript_text = " ".join([entry["text"] for entry in transcript])

            # Update transcript in the videos table
            cursor.execute("UPDATE videos SET transcript = ? WHERE id = ?", (transcript_text, video_id))

            print(f"✅ Successfully fetched transcript for {video_id}")

        except Exception as e:
            print(f"❌ Still no transcript for {video_id} ({str(e)})")

            # Leave '0' as-is so you can retry later if needed
            # You could also log these to a file if you want

        # Optional: Add a safe delay to avoid rate limiting
        time.sleep(random.randint(30, 55))

    # Commit transaction
    conn.commit()
    print("🎉 Transcript retry transaction committed.")

except Exception as e:
    conn.rollback()
    print(f"🚨 Error occurred during retry: {str(e)} — Rolling back changes!")

finally:
    conn.close()
