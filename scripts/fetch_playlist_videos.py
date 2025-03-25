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

# ✅ ENTER PLAYLIST ID HERE BEFORE RUNNING
PLAYLIST_ID = "UNIQUE PLAYLIST ID"

# Connect to SQLite database
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Function to fetch videos from a playlist
def fetch_playlist_videos(playlist_id):
    videos = []
    next_page_token = None

    while True:
        request = youtube.playlistItems().list(
            part="contentDetails,snippet",
            playlistId=playlist_id,
            maxResults=50,  # Max per request
            pageToken=next_page_token
        )
        response = request.execute()

        for item in response["items"]:
            video_id = item["contentDetails"]["videoId"]
            title = item["snippet"]["title"]
            description = item["snippet"].get("description", "")

            videos.append((video_id, title, description))

        next_page_token = response.get("nextPageToken")
        if not next_page_token:
            break

    return videos

# Fetch videos from the playlist
video_list = fetch_playlist_videos(PLAYLIST_ID)

if not video_list:
    print("🚨 No videos found in the playlist!")
else:
    parent_video = video_list[0][0]  # First video is the parent
    print(f"📌 Parent video: {parent_video}")

    # Start transaction
    conn.execute("BEGIN")
    
    try:
        # Save video metadata & relationships
        for video_id, title, description in video_list:
            # Insert video metadata
            cursor.execute("""
                INSERT INTO videos (id, title, description, transcript, flagged, parent_id)
                VALUES (?, ?, ?, NULL, 0, ?)
                ON CONFLICT(id) DO UPDATE SET 
                    title = COALESCE(excluded.title, title),
                    description = COALESCE(excluded.description, description),
                    parent_id = COALESCE(excluded.parent_id, ?)
            """, (video_id, title, description, parent_video, parent_video))

            # Insert parent-child relationship (ignore duplicates)
            if video_id != parent_video:
                cursor.execute("""
                    INSERT OR IGNORE INTO manual_video_links (parent_video, suggested_video)
                    VALUES (?, ?)
                """, (parent_video, video_id))

        # Commit changes
        conn.commit()
        print(f"✅ Successfully inserted {len(video_list)} videos into the database!")

        # Save output file
        output_path = "../data/playlist_video_ids.txt"
        with open(output_path, "w", encoding="utf-8") as f:
            for video_id, _, _ in video_list[1:]:  # Skip the first video (parent)
                f.write(f"{parent_video} : {video_id}\n")

        print(f"📄 Parent-child video list saved to {output_path}")

    except Exception as e:
        conn.rollback()
        print(f"🚨 Error: {str(e)} — Rolling back changes!")

    finally:
        conn.close()
