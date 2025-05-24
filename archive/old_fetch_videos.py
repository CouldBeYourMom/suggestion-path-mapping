import sqlite3
import googleapiclient.discovery

# 🔑 Replace with your actual YouTube API key
API_KEY = "YOUTUBE_API_KEY"

# YouTube API setup
youtube = googleapiclient.discovery.build("youtube", "v3", developerKey=API_KEY)

def get_video_metadata(video_id):
    """Fetches metadata for a given video ID."""
    request = youtube.videos().list(
        part="snippet",
        id=video_id
    )
    response = request.execute()
    
    if "items" not in response or len(response["items"]) == 0:
        print(f"❌ Video {video_id} not found.")
        return None

    video = response["items"][0]["snippet"]
    return {
        "id": video_id,
        "title": video["title"],
        "description": video["description"]
    }

def get_suggested_videos(video_id):
    """Fetches the first 10 videos from the same category as the given video ID."""
    request = youtube.videos().list(
        part="snippet",
        id=video_id
    )
    response = request.execute()

    if "items" not in response or len(response["items"]) == 0:
        print(f"❌ Could not retrieve category for video {video_id}.")
        return []

    # Get category ID of the original video
    category_id = response["items"][0]["snippet"]["categoryId"]

    # Fetch other videos from the same category
    request = youtube.search().list(
        part="snippet",
        type="video",
        videoCategoryId=category_id,  # Search within the same category
        maxResults=10,
        regionCode="US"
    )
    response = request.execute()

    suggested_videos = []
    for item in response.get("items", []):
        if "videoId" in item["id"]:
            suggested_videos.append({
                "id": item["id"]["videoId"],
                "title": item["snippet"]["title"],
                "description": item["snippet"]["description"]
            })

    return suggested_videos

def save_to_database(video_data, suggested_videos):
    """Saves video data and suggested videos to SQLite database."""
    conn = sqlite3.connect("../data/youtube_data.db")
    cursor = conn.cursor()

    # Insert main video
    cursor.execute("""
        INSERT OR IGNORE INTO videos (id, title, description)
        VALUES (?, ?, ?)
    """, (video_data["id"], video_data["title"], video_data["description"]))

    # Insert suggested videos
    for video in suggested_videos:
        cursor.execute("""
            INSERT OR IGNORE INTO videos (id, title, description, parent_id)
            VALUES (?, ?, ?, ?)
        """, (video["id"], video["title"], video["description"], video_data["id"]))

    conn.commit()
    conn.close()
    print(f"✅ Data saved for {video_data['title']} and its suggested videos.")

if __name__ == "__main__":
    START_VIDEO_ID = "Qi4AV2S4xA0"  # Example: 'dQw4w9WgXcQ'

    video_data = get_video_metadata(START_VIDEO_ID)
    if video_data:
        suggested_videos = get_suggested_videos(START_VIDEO_ID)
        save_to_database(video_data, suggested_videos)