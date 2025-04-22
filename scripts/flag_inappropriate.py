import sqlite3
import json
import re

# Load inappropriate words list
with open("../data/inappropriate_words.json", "r") as f:
    inappropriate_words = json.load(f)

def count_inappropriate_words(text):
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

# Connect to DB
conn = sqlite3.connect("../data/youtube_data.db")
cursor = conn.cursor()

# Prepare data store
flag_data = {}

# 🧠 Step 1: Age restriction keyword check
cursor.execute("SELECT id, description FROM videos")
for video_id, description in cursor.fetchall():
    age_flags = 0
    if description and any(keyword in description.lower() for keyword in ["under the age of 18", "parental permission", "18+"]):
        age_flags = 1
        print(f"🚨 Age restriction warning detected in {video_id}")
    flag_data[video_id] = {"age_flags": age_flags, "desc_flags": 0, "transcript_flags": 0, "comment_flags": 0}

# 🧠 Step 2: Description and transcript flagging
cursor.execute("SELECT id, description, transcript FROM videos")
for video_id, description, transcript in cursor.fetchall():
    desc_flags = count_inappropriate_words(description)
    trans_flags = count_inappropriate_words(transcript)

    if video_id not in flag_data:
        flag_data[video_id] = {"age_flags": 0, "desc_flags": 0, "transcript_flags": 0, "comment_flags": 0}

    flag_data[video_id]["desc_flags"] = desc_flags
    flag_data[video_id]["transcript_flags"] = trans_flags

# 🧠 Step 3: Comment flagging (summed per video)
cursor.execute("SELECT video_id, comment FROM comments")
for video_id, comment in cursor.fetchall():
    comment_flags = count_inappropriate_words(comment)

    if video_id not in flag_data:
        flag_data[video_id] = {"age_flags": 0, "desc_flags": 0, "transcript_flags": 0, "comment_flags": 0}

    flag_data[video_id]["comment_flags"] += comment_flags

# 💥 Step 4: Rebuild video_flag_counts
cursor.execute("DELETE FROM video_flag_counts")

for video_id, flags in flag_data.items():
    total = flags["age_flags"] + flags["desc_flags"] + flags["transcript_flags"] + flags["comment_flags"]
    cursor.execute("""
        INSERT INTO video_flag_counts (video_id, age_flags, desc_flags, transcript_flags, comment_flags, total_flags)
        VALUES (?, ?, ?, ?, ?, ?)
    """, (video_id, flags["age_flags"], flags["desc_flags"], flags["transcript_flags"], flags["comment_flags"], total))
    if total > 0:
        print(f"✅ {video_id} flagged — Total: {total} (Age: {flags['age_flags']}, Desc: {flags['desc_flags']}, Trans: {flags['transcript_flags']}, Comments: {flags['comment_flags']})")

conn.commit()
conn.close()

print("🎉 Done! video_flag_counts has been updated.")