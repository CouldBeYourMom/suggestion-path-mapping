import sqlite3
import json
import re

# Load inappropriate words list
with open("../data/inappropriate_words.json", "r") as f:
    inappropriate_words = json.load(f)

def count_inappropriate_words(text):
    """Returns the number of inappropriate words found in the text."""
    if not text:
        return 0
    text = text.lower()
    total_flags = 0
    for category, words in inappropriate_words.items():
        for word in words:
            if word == "ass":
                pattern = r"\b" + re.escape(word) + r"\b(?!\w)"
            else:
                pattern = r"\b" + re.escape(word) + r"\b"
            matches = re.findall(pattern, text)
            total_flags += len(matches)
    return total_flags

# Connect to database
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

flagged_videos = 0
flagged_comments = 0

# ✅ Flag videos with age restriction warnings
cursor.execute("SELECT id, description FROM videos WHERE flagged = 0")
for video_id, description in cursor.fetchall():
    if description and any(keyword in description.lower() for keyword in ["under the age of 18", "parental permission", "18+"]):
        cursor.execute("UPDATE videos SET flagged = 1 WHERE id = ?", (video_id,))
        flagged_videos += 1
        print(f"🚨 Flagged video {video_id} for age restriction warning.")

# ✅ Flag videos based on transcript and description
cursor.execute("SELECT id, description, transcript FROM videos")
for video_id, description, transcript in cursor.fetchall():
    desc_flags = count_inappropriate_words(description)
    trans_flags = count_inappropriate_words(transcript)
    total_flags = desc_flags + trans_flags

    if total_flags > 0:
        cursor.execute("UPDATE videos SET flagged = ? WHERE id = ?", (total_flags, video_id))
        flagged_videos += 1
        print(f"🚨 Flagged video {video_id} with {total_flags} content violations.")

# ✅ Flag inappropriate comments
cursor.execute("SELECT id, video_id, comment FROM comments WHERE flagged = 0")
for comment_id, video_id, comment in cursor.fetchall():
    comment_flags = count_inappropriate_words(comment)
    if comment_flags > 0:
        cursor.execute("UPDATE comments SET flagged = ? WHERE id = ?", (comment_flags, comment_id))
        flagged_comments += 1
        print(f"🚨 Flagged comment {comment_id} on video {video_id} with {comment_flags} issues.")

# ✅ Recalculate and update video_flag_counts (replaces triggers)
print("📊 Recalculating total flags for each video...")
cursor.execute("DELETE FROM video_flag_counts")
cursor.execute("""
    INSERT INTO video_flag_counts (video_id, flag_count)
    SELECT
        v.id,
        COALESCE(v.flagged, 0) + COALESCE(SUM(c.flagged), 0)
    FROM videos v
    LEFT JOIN comments c ON v.id = c.video_id
    GROUP BY v.id
""")
print("✅ video_flag_counts table updated with combined flags.")

conn.commit()

# ✅ Final counts
cursor.execute("SELECT COUNT(*) FROM videos WHERE flagged > 0")
final_videos_flagged = cursor.fetchone()[0]

cursor.execute("SELECT COUNT(*) FROM comments WHERE flagged > 0")
final_comments_flagged = cursor.fetchone()[0]

conn.close()

print(f"🎉 Flagging complete! {final_videos_flagged} videos and {final_comments_flagged} comments flagged.")
