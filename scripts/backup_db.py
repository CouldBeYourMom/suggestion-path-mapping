import sqlite3
import os
import zipfile
from datetime import datetime

# ___ CONFIGURATION ___
MAX_BACKUPS = 100              # Set to None to keep everything
COMPRESS_BACKUPS = True        # Set to False to skip .zip
LOG_BACKUPS = True             # Set to False to skip logging
BACKUP_FILENAME_PREFIX = "youtube_data_backup"

# ___ PATH SETUP ___
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))

DB_PATH = os.path.join(PROJECT_ROOT, "data", "youtube_data.db")
BACKUP_DIR = os.path.join(PROJECT_ROOT, "backups")
LOG_PATH = os.path.join(BACKUP_DIR, "backup.log")

def ensure_dirs():
    if not os.path.exists(BACKUP_DIR):
        os.makedirs(BACKUP_DIR)

def rotate_backups():
    if MAX_BACKUPS is None:
        return
    backups = sorted([
        f for f in os.listdir(BACKUP_DIR)
        if f.startswith(BACKUP_FILENAME_PREFIX)
    ])
    while len(backups) > MAX_BACKUPS:
        to_delete = backups.pop(0)
        os.remove(os.path.join(BACKUP_DIR, to_delete))
        if LOG_BACKUPS:
            log_entry(f"Deleted old backup: {to_delete}")

def log_entry(message):
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(LOG_PATH, "a", encoding="utf-8") as log_file:
        log_file.write(f"[{timestamp}] {message}\n")

def backup_database():
    ensure_dirs()
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    raw_backup_filename = f"{BACKUP_FILENAME_PREFIX}_{timestamp}.db"
    raw_backup_path = os.path.join(BACKUP_DIR, raw_backup_filename)

    print("Looking for DB at:", DB_PATH)
    try:
        conn = sqlite3.connect(DB_PATH)
        backup_conn = sqlite3.connect(raw_backup_path)
        conn.backup(backup_conn)
        backup_conn.close()
        conn.close()
        print(f"Raw backup created: {raw_backup_filename}")
        if LOG_BACKUPS:
            log_entry(f"Raw backup created: {raw_backup_filename}")
    except Exception as e:
        error_msg = f"Backup failed: {e}"
        print(error_msg)
        if LOG_BACKUPS:
            log_entry(error_msg)
        return

    if COMPRESS_BACKUPS:
        zip_path = raw_backup_path.replace(".db", ".zip")
        with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
            zipf.write(raw_backup_path, arcname=os.path.basename(raw_backup_path))
        os.remove(raw_backup_path)
        print(f"Compressed to ZIP: {os.path.basename(zip_path)}")
        if LOG_BACKUPS:
            log_entry(f"Compressed to ZIP: {os.path.basename(zip_path)}")

    rotate_backups()

if __name__ == "__main__":
    backup_database()
