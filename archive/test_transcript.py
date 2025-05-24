from youtube_transcript_api import YouTubeTranscriptApi

video_id = "O0EShM655h8"  # Replace with any video in your database

try:
    transcript = YouTubeTranscriptApi.get_transcript(video_id)
    transcript_text = " ".join([entry["text"] for entry in transcript])
    print("✅ Transcript retrieved:", transcript_text[:200])  # Print first 200 chars
except Exception as e:
    print(f"⚠ No transcript available ({str(e)}).")