#!/bin/bash
set -e

MODE=$1      # build | run
FOCUS=$2     # cpp | python | both

cleanup() {
    echo -e "\nCaught SIGINT, killing child processes..."
    kill $PYTHON_PID $CPP_PID 2>/dev/null || true
    wait $PYTHON_PID $CPP_PID 2>/dev/null || true
    exit 1
}

trap cleanup SIGINT

case "$MODE" in
    build)
        echo "=== BUILD PHASE ==="
        cd build
        cmake --build .
        ;;
    run)
        echo "=== RUN PHASE ==="

        # Start Python server with optional silencing
        if [ "$FOCUS" = "cpp" ]; then
            python -u "C:/Users/Jase/Desktop/drone_sim2/py/main.py" > /dev/null 2>&1 &
           # python -u " /mnt/c/Users/Jase/Documents/Projects/Graphics/BranchEngine/fc.py" > /dev/null 2>&1 &
        else
            python -u "C:/Users/Jase/Desktop/drone_sim2/py/main.py" &
           # python -u "/mnt/c/Users/Jase/Documents/Projects/Graphics/BranchEngine/fc.py" &
        fi
        PYTHON_PID=$!

        cd build

        # Run C++ program with optional silencing
        if [ "$FOCUS" = "python" ]; then
            ./Debug/drone_sim.exe > /dev/null 2>&1 &
        else
            ./Debug/drone_sim.exe &
        fi
        CPP_PID=$!

        # Wait for C++ program to exit
        wait $CPP_PID

        echo "C++ program exited, killing Python server..."
        kill $PYTHON_PID 2>/dev/null || true
        wait $PYTHON_PID 2>/dev/null || true
        ;;
    *)
        echo "Usage: $0 {build|run} [cpp|python|both]"
        exit 1
        ;;
esac
