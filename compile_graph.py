# compile_graph.py
import os
import subprocess
import platform

print("📦 Compiling C++ Project...")

compile_cmd = [
    "g++", "-std=c++17", "-o", "bin/graph.exe",
    "cpp/main.cpp", "cpp/graph.cpp", "visualization/cpp/graph_viz.cpp",
    "-lsqlite3"
]

result = subprocess.run(compile_cmd)
if result.returncode != 0:
    print("❌ Compilation failed.")
    exit(1)

print("✅ Compilation complete.")
print("🚀 Running traversal program...\n")

subprocess.run(["bin/graph.exe"])

print("🌐 Starting local web server (http://localhost:5500)...")
os.chdir("visualization")
subprocess.run(["python", "-m", "http.server", "5500"])
