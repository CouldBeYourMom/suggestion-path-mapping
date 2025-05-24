# 🗃️ Archive Folder

This folder contains scripts that are no longer actively used but are preserved for transparency, reproducibility, or historical context. Many of these were stepping stones in the development of the current pipeline, and some may be re-used or referenced in future extensions.

---

## 📦 Script Index

| Script Name               | Description |
|---------------------------|-------------|
| `fetch_videos.py`         | Early recursive fetcher using `relatedToVideoId` (now deprecated). Saved relationships in the `video_links` table. |
| `fetch_video_metadata.py` | Predecessor to `fetch_video_stats.py`. Focused on title/description only. |
| `insert_manual_links.py`  | One-time script to manually populate `manual_video_links` from collected video ID pairs. |
| `setup_db.py`             | Prototyped initial schema and test data. Not used in current workflows. May overwrite tables. |
| `test_flagging.py`        | Local testbed to verify word-flagging accuracy against known input. |
| `test_transcript.py`      | Used to troubleshoot transcript fetching before implementing rate limiting. |
| `trackautoplay.py`        | Selenium-based script to simulate user autoplay behavior. Deprecated due to YouTube's bot detection. |
| `old_fetch_comments.py`   | Initial comment collector with basic logic. Replaced by `fetch_comments.py` with better deduping and recursion support. |
| `old_fetch_transcripts.py`| Basic transcript fetcher without request delays or tracking. Replaced by `fetch_transcripts.py`. |
| `old_fetch_videos.py`     | Deprecated fetcher that built connections using same-category searches. |
| `update_db_schema.py`     | Early schema version with `videos` and `video_links`. Now replaced by `manual_video_links`. |
| `update2_db_schema.py`    | Added `manual_video_links` and ensured `videos` table existed. |
| `update3_db_schema.py`    | Removed `video_links` table. Populated `videos.parent_id` from `manual_video_links`. |
| `update4_db_schema.py`    | Added `video_stats` table to store view/like/comment counts and timestamps. |
| `update_db_triggers.py`   | Created triggers to sync `video_flag_counts`. Replaced by script-based summary logic for accuracy. |
| `verify_suggestions.py`   | Compared saved suggestions to YouTube API results to validate earlier relationship data. No longer used since API method was deprecated. |

---

> ✅ These files are not part of the active data pipeline but are preserved to support traceability and potential future reuse.

