import subprocess

# Run playlist_videos
print("📊 Fetching playlist videos...")
subprocess.run(["python", "fetch_playlist_videos.py"])

# Run stats
print("🎥 Fetching stats...")
subprocess.run(["python", "fetch_video_stats.py"])

# Run transcripts
print("📄 Fetching transcripts...")
subprocess.run(["python", "fetch_transcripts.py"])

# Run comments
print("💬 Fetching comments...")
subprocess.run(["python", "fetch_comments.py"])

# Run flags
print("🚩 Flagging videos...")
subprocess.run(["python", "flag_inappropriate.py"])

print("✅ All tasks completed!")

# Run summary
print("✅ Here is your summary:")
subprocess.run(["python", "summarize_table_counts.py"])
