#!/bin/bash
set -e

MODE=$1      # build | run
FOCUS=$2     # cpp | python | ada 

cleanup() {
    echo -e "\nCaught SIGINT, killing child processes..."
    kill $PYTHON_PID $CPP_PID $ADA_PID 2>/dev/null || true
    wait $PYTHON_PID $CPP_PID $ADA_PID 2>/dev/null || true
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

        case "$FOCUS" in
            python)
                echo "Starting Python backend..."
                python -u "C:/Users/Jase/Desktop/drone_sim2/py/main.py" &
                PYTHON_PID=$!
                echo "Python started (PID: $PYTHON_PID)"
                ;;
            ada)
                echo "Starting Ada program..."
                cd ada
                alr run > /dev/null 2>&1 &
                ADA_PID=$!
                cd ..
                echo "Ada started (PID: $ADA_PID)"
                ;;
            *)
                echo "No backend selected, running C++ only."
                ;;
        esac
        # Might Need to adjust this delay based on backend initialization time
        echo "Waiting 5 seconds before launching C++... Letting Backend initialize."
        sleep 5

        cd build
        if [ "$FOCUS" = "python" ] || [ "$FOCUS" = "ada" ]; then
            ./Debug/drone_sim.exe &
        else
            ./Debug/drone_sim.exe &
        fi
        CPP_PID=$!
        echo "C++ started (PID: $CPP_PID)"
        wait $CPP_PID
        echo "C++ program exited."

        if [ "$FOCUS" = "python" ]; then
            echo "Stopping Python..."
            kill $PYTHON_PID 2>/dev/null || true
            wait $PYTHON_PID 2>/dev/null || true
        elif [ "$FOCUS" = "ada" ]; then
            echo "Stopping Ada..."
            kill $ADA_PID 2>/dev/null || true
            wait $ADA_PID 2>/dev/null || true
        fi

        echo "All processes finished cleanly."
        ;;

    *)
        echo "Usage: $0 {build|run} [cpp|python|ada]"
        exit 1
        ;;
esac
