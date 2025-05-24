import sqlite3
import googleapiclient.discovery
import googleapiclient.errors

# 🔑 Replace with your actual YouTube API key
API_KEY = "YOUTUBE_API_KEY"

# YouTube API setup
youtube = googleapiclient.discovery.build("youtube", "v3", developerKey=API_KEY)

def get_video_comments(video_id, max_comments=20):
    """Fetches comments for a given video ID."""
    comments = []
    
    try:
        request = youtube.commentThreads().list(
            part="snippet",
            videoId=video_id,
            maxResults=max_comments,
            textFormat="plainText"
        )
        response = request.execute()

        for item in response.get("items", []):
            comment_text = item["snippet"]["topLevelComment"]["snippet"]["textDisplay"]
            comments.append(comment_text)

    except googleapiclient.errors.HttpError as e:
        if "disabled comments" in str(e).lower():
            print(f"⚠ Comments are disabled for video {video_id}.")
        else:
            print(f"⚠ Error fetching comments for video {video_id}: {e}")
    
    return comments

def save_comments_to_db():
    """Fetches comments for videos and stores them in the database."""
    conn = sqlite3.connect("../data/youtube_data.db")
    cursor = conn.cursor()

    # Get videos that don't have comments stored
    cursor.execute("SELECT id FROM videos WHERE id NOT IN (SELECT DISTINCT video_id FROM comments)")
    videos = cursor.fetchall()

    if not videos:
        print("✅ All videos already have comments.")
        conn.close()
        return

    print(f"📥 Fetching comments for {len(videos)} videos...")

    for (video_id,) in videos:
        comments = get_video_comments(video_id)
        for idx, comment in enumerate(comments):
            comment_id = f"{video_id}_cmt{idx+1}"
            cursor.execute("""
                INSERT OR IGNORE INTO comments (id, video_id, comment, flagged)
                VALUES (?, ?, ?, ?)
            """, (comment_id, video_id, comment, 0))

        conn.commit()
        print(f"✅ Comments saved for {video_id} ({len(comments)} total)")

    conn.close()
    print("🎉 Comment fetching complete!")

if __name__ == "__main__":
    save_comments_to_db()
