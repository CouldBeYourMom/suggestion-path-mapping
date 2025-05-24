import sqlite3
import os
from dotenv import load_dotenv
from googleapiclient.discovery import build

# Load API Key from .env
load_dotenv()
API_KEY = os.getenv("YOUTUBE_API_KEY")

# Ensure API Key is loaded
if not API_KEY:
    raise ValueError("❌ API Key Not Found! Check your .env file.")

# Initialize YouTube API client
youtube = build("youtube", "v3", developerKey=API_KEY)

# Connect to SQLite database
DB_PATH = "C:/Users/carri/Documents/UF/Spring25/COP3530 - DSA/YouTube-Graph-Analysis/data/youtube_data.db"
conn = sqlite3.connect(DB_PATH)
cursor = conn.cursor()

# Debugging: Confirm script is running
print("🚀 Running verify_suggestions.py...")

# 🔍 Fetch Suggested Videos from YouTube API
def get_suggested_videos(video_id):
    """Fetch suggested videos from YouTube API"""
    try:
        request = youtube.search().list(
            part="snippet",
            relatedToVideoId=video_id,
            type="video",
            maxResults=10
        )
        response = request.execute()

        suggested_videos = []
        for item in response.get("items", []):
            video_id = item["id"]["videoId"]
            title = item["snippet"]["title"]
            suggested_videos.append((video_id, title))

        print(f"🎥 API returned {len(suggested_videos)} suggested videos.")  # Debugging
        return suggested_videos
    except Exception as e:
        print(f"⚠ Error fetching suggestions for {video_id}: {e}")
        return []

# 🔍 Fetch Saved Suggested Videos from DB
def get_saved_suggestions(video_id):
    """Retrieve saved suggested videos from the SQLite database"""
    cursor.execute("""
        SELECT target_video FROM video_links WHERE source_video = ?
    """, (video_id,))
    saved_videos = [row[0] for row in cursor.fetchall()]

    # Fetch video titles
    saved_video_titles = {}
    for vid in saved_videos:
        cursor.execute("SELECT title FROM videos WHERE id = ?", (vid,))
        row = cursor.fetchone()
        if row:
            saved_video_titles[vid] = row[0]

    print(f"💾 Database returned {len(saved_video_titles)} saved suggested videos.")  # Debugging
    return saved_video_titles

# 🔍 Compare API vs. Database
def compare_suggestions(video_id):
    print(f"\n🔎 **Verifying Suggestions for Video ID: {video_id}**")

    # Get YouTube API suggestions
    api_suggestions = get_suggested_videos(video_id)

    # Get saved database suggestions
    saved_suggestions = get_saved_suggestions(video_id)

    # Compare results
    api_video_ids = {vid for vid, _ in api_suggestions}
    saved_video_ids = set(saved_suggestions.keys())

    print("\n✅ **Matches (Found in both API & Database):**")
    for vid, title in api_suggestions:
        if vid in saved_video_ids:
            print(f"   - {title} ({vid})")

    print("\n❌ **Missing in Database (API suggests but not saved):**")
    for vid, title in api_suggestions:
        if vid not in saved_video_ids:
            print(f"   - {title} ({vid})")

    print("\n❓ **Unexpected in Database (Saved but NOT in API results):**")
    for vid, title in saved_suggestions.items():
        if vid not in api_video_ids:
            print(f"   - {title} ({vid})")

# 🔥 Run Verification
if __name__ == "__main__":
    print("🚀 Running verify_suggestions.py...")  # Debugging

    # Change this to a **known video ID from your database**
    START_VIDEO_ID = "Qi4AV2S4xA0"  # Ms. Rachel's video

    compare_suggestions(START_VIDEO_ID)

    # Close DB connection
    conn.close()
