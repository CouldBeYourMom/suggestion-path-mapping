import json
import re

# Load inappropriate words list
with open("../data/inappropriate_words.json", "r") as f:
    inappropriate_words = json.load(f)

def contains_inappropriate_content(text):
    if not text:
        return False  # Ensure the function always returns a value

    text = text.lower()
    for category, words in inappropriate_words.items():
        for word in words:
            pattern = r"\b" + re.escape(word) + r"\b"
            if re.search(pattern, text):
                print(f"🚨 Matched word: {word}")  # Debugging: Show what triggered the flag
                return True
    return False  # Make sure function doesn't return None

# Test cases
test_texts = [
    "That does sound annoying about struggling to get into those trains and make it inside and just die.",
    "This is a clean comment with no inappropriate words.",
    "This is a terrible crime scene with murder and blood.",
    "OMG, this is an epic 3some party at the club!"
]

for text in test_texts:
    result = contains_inappropriate_content(text)
    print(f"Test: {text}\nResult: {result}\n")
