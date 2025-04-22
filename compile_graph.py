import os
import subprocess
import webbrowser
import time
import signal

print("📦 Compiling C++ Project...\n")

compile_cmd = [
    "g++", "-std=c++17", "-Icpp", "-Ivisualization/cpp",
    "-o", "bin/graph.exe",
    "cpp/main.cpp", "cpp/graph.cpp",
    "cpp/search_dijkstra.cpp", "cpp/search_astar.cpp", "cpp/search_random_walk.cpp",
    "visualization/cpp/graph_viz.cpp",
    "-lsqlite3"
]

result = subprocess.run(compile_cmd)
if result.returncode != 0:
    print("❌ Compilation failed.")
    input("Press [Enter] to exit...")
    exit(1)

print("✅ Compilation complete.")
print("🚀 Running traversal program...\n")

run_result = subprocess.run(["bin/graph.exe"])
if run_result.returncode != 0:
    print("❌ Traversal failed.")
    input("Press [Enter] to exit...")
    exit(1)

print("🌐 Starting local web server at http://localhost:5500...\n")
os.chdir("visualization")
webbrowser.open("http://localhost:5500")
time.sleep(1)

# Launch server in background
server_proc = subprocess.Popen(["python", "-m", "http.server", "5500"])

try:
    print("📝 When you're done viewing the graph:")
    print("   1. Close the browser tab")
    print("   2. Return to this window and press [Enter] to exit")
    input()
finally:
    print("🛑 Shutting down local server...")
    server_proc.terminate()
    server_proc.wait()