#!/bin/bash
set -e

MODE=$1      # build | run
FOCUS=$2     # python | ada | both

# ---------- Cleanup function ----------
cleanup() {
    echo -e "\nCaught SIGINT, killing child processes..."
    kill $PYTHON_PID $ADA_PID 2>/dev/null || true
    wait $PYTHON_PID $ADA_PID 2>/dev/null || true
    exit 1
}

trap cleanup SIGINT

case "$MODE" in
    build)
        echo "=== BUILD PHASE ==="
        # Build Ada project
        alr build
        ;;
    run)
        echo "=== RUN PHASE ==="

        # --- Start Python ---
        if [ "$FOCUS" = "ada" ]; then
            # Hide Python output
            python -u "C:\Users\Jase\Desktop\flight_controll2\flight_controller\dummy.py" > /dev/null 2>&1 &
        else
            # Show Python output
            python -u "C:\Users\Jase\Desktop\flight_controll2\flight_controller\dummy.py" &
        fi
        PYTHON_PID=$!

        # --- Start Ada ---
        if [ "$FOCUS" = "python" ]; then
            # Hide Ada output
            alr run > /dev/null 2>&1 &
        else
            # Show Ada output
            alr run &
        fi
        ADA_PID=$!

        # --- Wait for Ada to exit ---
        wait $ADA_PID

        echo "Ada program exited, killing Python..."
        kill $PYTHON_PID 2>/dev/null || true
        wait $PYTHON_PID 2>/dev/null || true
        ;;
    *)
        echo "Usage: $0 {build|run} [python|ada|both]"
        exit 1
        ;;
esac
