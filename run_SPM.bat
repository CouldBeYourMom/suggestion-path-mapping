@echo off
echo ===============================
echo Compiling Suggestion Path Mapping...
echo ===============================

g++ -std=c++17 -Icpp -Ivisualization/cpp ^
    -o bin/graph.exe ^
    cpp/main.cpp ^
    cpp/graph.cpp ^
    cpp/search_astar.cpp ^
    cpp/search_dijkstra.cpp ^
    cpp/search_random_walk.cpp ^
    visualization/cpp/graph_viz.cpp ^
    -lsqlite3

if errorlevel 1 (
    echo ❌ Compilation failed.
    pause
    exit /b
)

echo ✅ Compilation successful.
echo ===============================
echo 🚀 Running program...
echo ===============================

bin\graph.exe

echo ===============================
echo 🌐 Starting local server at localhost:5500 ...
echo ===============================
cd visualization
start "" http://localhost:5500
start "" python -m http.server 5500

echo ===============================
echo 📝 When you're done viewing the graph:
echo   1. Close the browser tab
echo   2. Return to this window and press [Enter] to stop the server
echo ===============================
pause