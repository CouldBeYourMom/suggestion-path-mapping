import sqlite3
from youtube_transcript_api import YouTubeTranscriptApi
from youtube_transcript_api._errors import TranscriptsDisabled, VideoUnavailable

def get_transcript(video_id):
    """Fetches the transcript of a given video ID (if available)."""
    try:
        transcript_list = YouTubeTranscriptApi.get_transcript(video_id)
        transcript_text = " ".join([entry["text"] for entry in transcript_list])
        return transcript_text
    except TranscriptsDisabled:
        print(f"⚠ No transcript available for video {video_id} (Disabled).")
        return None
    except VideoUnavailable:
        print(f"❌ Video {video_id} is unavailable.")
        return None
    except Exception as e:
        print(f"⚠ Error fetching transcript for {video_id}: {e}")
        return None

def update_transcripts():
    """Fetches transcripts for videos in the database and updates them."""
    conn = sqlite3.connect("../data/youtube_data.db")
    cursor = conn.cursor()

    # Get videos that don't have transcripts yet
    cursor.execute("SELECT id FROM videos WHERE transcript IS NULL")
    videos = cursor.fetchall()

    if not videos:
        print("✅ All videos already have transcripts.")
        conn.close()
        return

    print(f"📥 Fetching transcripts for {len(videos)} videos...")

    for (video_id,) in videos:
        transcript = get_transcript(video_id)
        if transcript:
            cursor.execute("UPDATE videos SET transcript = ? WHERE id = ?", (transcript, video_id))
            conn.commit()
            print(f"✅ Transcript saved for {video_id}")

    conn.close()
    print("🎉 Transcript fetching complete!")

if __name__ == "__main__":
    update_transcripts()