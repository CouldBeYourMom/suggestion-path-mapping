import sqlite3
from datetime import datetime

DB_PATH = "../data/youtube_data.db"

TABLES = [
    "manual_video_links",
    "videos",
    "video_stats",
    "comments",
    "video_flag_counts"
]

def summarize():
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    total = 0
    print("📊 Current Row Counts:")
    print(f"🕒 {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("-" * 35)

    for table in TABLES:
        if table == "video_flag_counts":
            cursor.execute("SELECT COUNT(*) FROM video_flag_counts WHERE flag_count > 0")
        else:
            cursor.execute(f"SELECT COUNT(*) FROM {table}")
        count = cursor.fetchone()[0]
        total += count
        print(f"{table.upper():<22}: {count}")

    print("-" * 35)
    print(f"{'TOTAL':<22}: {total}")

    conn.close()

if __name__ == "__main__":
    summarize()
