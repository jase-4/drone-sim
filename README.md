# Drone Simulator

**Blog Page:** [For more depth and video demos](https://jasebranch.com/posts/drone_sim/)


<img width="500" height="300" alt="drone_sim" src="https://github.com/user-attachments/assets/ed87d09f-4967-4257-8f55-32405e91b5e4" />

A real-time drone simulator integrating **physics**, **graphics**, and **networking**, built with **C++**, **Bullet Physics**, **OpenGL**, and **Boost.Asio**.
A virtual flight controller written in **Ada** and **Python** communicates over **UDP** to command the drone.

---

## Features

- **Physics:** Realistic rigid-body dynamics and collisions (Bullet Physics).
- **Rendering:** Real-time OpenGL visualization.
- **Sensors:** IMU, GPS, and other sensor emulation via Boost.Asio.
- **Control:** Ada-based PID flight controller over UDP.
- **Integration:** Unified pipeline linking physics, sensors, and control logic.

---

## System Diagram

<img width="791" height="452" alt="drone_diagram" src="https://github.com/user-attachments/assets/1e1c2ea7-0696-44e2-ada8-ba8cd52415c3" />

---

## Running the Simulator

The `drone_sim.sh` Bash script manages the build process and handles the simultaneous launch and graceful shutdown of the required C++ simulator and the external flight controller processes (Python or Ada).

**Note:** This script is designed to be run from a shell environment like **Git Bash (MinGW)** on Windows.

### Usage

The script accepts two arguments: the **mode** (`build` or `run`) and the **controller focus** (`cpp`, `python`, or `ada`).

```bash
# Build the C++ simulator executable
./drone_sim.sh build

# Run the simulator using the Python flight controller
./drone_sim.sh run python

# Run the simulator using the Ada flight controller
./drone_sim.sh run ada

# Run the C++ simulator only (no external flight control logic)
./drone_sim.sh run cpp
