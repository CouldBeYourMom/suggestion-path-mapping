import sqlite3
import googleapiclient.discovery
import os
from dotenv import load_dotenv

# Load API Key from .env
load_dotenv()
API_KEY = os.getenv("YOUTUBE_API_KEY")

# Initialize YouTube API client
youtube = googleapiclient.discovery.build("youtube", "v3", developerKey=API_KEY)

# Database connection
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Ensure tables exist
cursor.execute("""
    CREATE TABLE IF NOT EXISTS videos (
        id TEXT PRIMARY KEY,
        title TEXT,
        description TEXT
    )
""")

cursor.execute("""
    CREATE TABLE IF NOT EXISTS video_links (
        source_video TEXT,
        target_video TEXT,
        PRIMARY KEY (source_video, target_video),
        FOREIGN KEY (source_video) REFERENCES videos(id),
        FOREIGN KEY (target_video) REFERENCES videos(id)
    )
""")

conn.commit()

# Quota tracking
DAILY_QUOTA_LIMIT = 10000
QUOTA_RESERVE = 2000  # Leave 2000 units for transcripts & comments
quota_used = 0

# Processed videos to avoid infinite loops
processed_videos = set()

# 📌 Fetch Video Metadata
def get_video_metadata(video_id):
    global quota_used
    if quota_used >= (DAILY_QUOTA_LIMIT - QUOTA_RESERVE):
        print("🚨 Quota limit reached! Stopping execution.")
        return None

    try:
        request = youtube.videos().list(part="snippet", id=video_id)
        response = request.execute()
        quota_used += 1  # videos.list costs 1 unit

        if "items" not in response or len(response["items"]) == 0:
            print(f"❌ Video {video_id} not found.")
            return None

        video = response["items"][0]["snippet"]
        return {
            "id": video_id,
            "title": video["title"],
            "description": video["description"]
        }
    except Exception as e:
        print(f"⚠ Error fetching metadata for {video_id}: {e}")
        return None

# 📌 Fetch Suggested Videos (Recursive with Quota Limit)
def get_suggested_videos(video_id, depth=0, max_depth=3):
    """Fetch suggested videos and recursively collect them up to a max depth."""
    global quota_used
    if quota_used >= (DAILY_QUOTA_LIMIT - QUOTA_RESERVE):
        print("🚨 Quota limit reached! Stopping execution.")
        return

    if depth > max_depth:
        return  # Stop recursion after reaching max depth

    try:
        request = youtube.search().list(
            part="snippet",
            relatedToVideoId=video_id,
            type="video",
            maxResults=10
        )
        response = request.execute()
        quota_used += 100  # search.list costs 100 units

        for item in response.get("items", []):
            suggested_video_id = item["id"]["videoId"]

            # Store the parent-child relationship in video_links
            save_video_link(video_id, suggested_video_id)

            if suggested_video_id not in processed_videos:
                processed_videos.add(suggested_video_id)

                video_data = get_video_metadata(suggested_video_id)
                if video_data:
                    save_video_data(video_data)

                # Recursively fetch suggested videos
                get_suggested_videos(suggested_video_id, depth + 1, max_depth)

    except Exception as e:
        print(f"⚠ Error fetching suggested videos for {video_id}: {e}")

# 📌 Save Video Data to Database (Nodes)
def save_video_data(video_data):
    """Saves video data to SQLite database."""
    cursor.execute("""
        INSERT OR IGNORE INTO videos (id, title, description)
        VALUES (?, ?, ?)
    """, (video_data["id"], video_data["title"], video_data["description"]))
    conn.commit()
    print(f"✅ Video saved: {video_data['title']} (ID: {video_data['id']})")

# 📌 Save Video Links to Database (Edges)
def save_video_link(source_id, target_id):
    """Saves the relationship between videos in the video_links table."""
    cursor.execute("""
        INSERT OR IGNORE INTO video_links (source_video, target_video)
        VALUES (?, ?)
    """, (source_id, target_id))
    conn.commit()
    print(f"🔗 Link saved: {source_id} ➝ {target_id}")

# 📌 Main Execution
if __name__ == "__main__":
    START_VIDEO_ID = "INSERT_A_CHILD_SAFE_VIDEO_ID_HERE"  # Example: '1d6LEnCW99g'

    # Start collecting videos recursively
    video_data = get_video_metadata(START_VIDEO_ID)
    if video_data:
        save_video_data(video_data)
        get_suggested_videos(START_VIDEO_ID)

    conn.close()
    print("🎉 Data collection complete!")
