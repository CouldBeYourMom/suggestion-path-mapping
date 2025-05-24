import sqlite3

# Path to your database file
DB_PATH = "../data/youtube_data.db"

# SQL commands to create triggers
CREATE_TRIGGERS_SQL = """
-- Ensure a row exists in video_flag_counts when a comment is flagged
CREATE TRIGGER IF NOT EXISTS update_comment_flag_count
AFTER UPDATE OF flagged ON comments
BEGIN
    INSERT INTO video_flag_counts (video_id, flag_count)
    VALUES (NEW.video_id, (
        SELECT COALESCE(SUM(flagged), 0) FROM comments WHERE video_id = NEW.video_id
    ) + (
        SELECT COALESCE(flagged, 0) FROM videos WHERE id = NEW.video_id
    ))
    ON CONFLICT(video_id) DO UPDATE SET flag_count = (
        (SELECT COALESCE(SUM(flagged), 0) FROM comments WHERE video_id = NEW.video_id) +
        (SELECT COALESCE(flagged, 0) FROM videos WHERE id = NEW.video_id)
    );
END;

-- Update flag counts for videos
CREATE TRIGGER IF NOT EXISTS update_video_flag_count
AFTER UPDATE OF flagged ON videos
BEGIN
    INSERT INTO video_flag_counts (video_id, flag_count)
    VALUES (NEW.id, (
        SELECT COALESCE(flagged, 0) FROM videos WHERE id = NEW.id
    ) + (
        SELECT COALESCE(SUM(flagged), 0) FROM comments WHERE video_id = NEW.id
    ))
    ON CONFLICT(video_id) DO UPDATE SET flag_count = (
        (SELECT COALESCE(flagged, 0) FROM videos WHERE id = NEW.id) +
        (SELECT COALESCE(SUM(flagged), 0) FROM comments WHERE video_id = NEW.id)
    );
END;
"""

# SQL commands to remove triggers if rollback is needed
ROLLBACK_TRIGGERS_SQL = """
DROP TRIGGER IF EXISTS ensure_video_flag_exists;
DROP TRIGGER IF EXISTS update_video_flag_count;
"""

def apply_triggers():
    """Applies the new triggers to the database."""
    try:
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()
        cursor.executescript(CREATE_TRIGGERS_SQL)
        conn.commit()
        conn.close()
        print("✅ Triggers successfully created and applied.")
    except Exception as e:
        print(f"❌ Error applying triggers: {e}")

def rollback_triggers():
    """Removes the triggers in case of errors or rollback."""
    try:
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()
        cursor.executescript(ROLLBACK_TRIGGERS_SQL)
        conn.commit()
        conn.close()
        print("⏪ Triggers successfully removed (rollback complete).")
    except Exception as e:
        print(f"❌ Error during rollback: {e}")

if __name__ == "__main__":
    print("⚡ Applying triggers to database...")
    apply_triggers()

    # Uncomment the next line to run a rollback instead
    # rollback_triggers()
