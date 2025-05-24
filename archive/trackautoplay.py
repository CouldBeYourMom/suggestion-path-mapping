from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from webdriver_manager.chrome import ChromeDriverManager
import time
import re
import random

# Configure Chrome to run headless
options = Options()
#options.add_argument("--headless")  # Runs in the background (no window)
options.add_argument("--mute-audio")  # Mute audio for efficiency

# Initialize WebDriver
driver = webdriver.Chrome(service=Service(ChromeDriverManager().install()), options=options)

def get_video_id(url):
    """Extract YouTube video ID from URL"""
    match = re.search(r"v=([\w-]+)", url)
    return match.group(1) if match else None

def track_autoplay(start_video_url, max_depth=10):
    """Simulate autoplay behavior by tracking the next video YouTube selects"""
    driver.get(start_video_url)
    # Initial wait before tracking starts (random delay)
    time.sleep(random.randint(10, 30))

    video_sequence = []

    for i in range(max_depth):
        current_url = driver.current_url
        video_id = get_video_id(current_url)
        
        if video_id:
            video_sequence.append(video_id)
            print(f"🎥 {i+1}. Video ID: {video_id}")

        # Wait for autoplay (give it time to switch)
        time.sleep(random.randint(10, 30))  # Adjust if necessary
        
        # Debugging: Check what URL Selenium is seeing
        print(f"DEBUG: Current URL - {driver.current_url}")

    driver.quit()
    return video_sequence

# Example Usage
start_video = "https://www.youtube.com/watch?v=26M-bU3wMBI"  # Stampylonghead video
autoplay_videos = track_autoplay(start_video, max_depth=5)

print("\n🔗 Autoplay Sequence:")
for vid in autoplay_videos:
    print(f"https://www.youtube.com/watch?v={vid}")
