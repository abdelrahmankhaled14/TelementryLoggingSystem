# Telemetry Dashboard 

This project contains three components:

- **Server** – collects real system metrics from Linux
- **Client** – forwards telemetry
- **GUI** – Qt5/QML car-style dashboard

## What Was Changed

### ✔ Server: Now Reads Real System Data

The server no longer generates random numbers.
 It now samples real metrics from Linux:

- **CPU usage** → `/proc/stat`
- **RAM usage** → `/proc/meminfo`
- **CPU temperature** → `/sys/class/thermal/.../temp`

Sampling is done every ~200 ms to provide smooth, responsive data.

### ✔ GUI: Completely Redesigned Into a Car Dashboard

The old basic progress bars were replaced with:

- **Three animated circular gauges** (CPU, RAM, TEMP)
  - Smooth needle movement
  - Tick marks, colored arcs, warning indicators
  - Speedometer-style sweep angles
- **Scrolling history graph** showing the last N samples
- **Continuous updates** (UI refresh ~10 FPS)

The dashboard feels like a real instrument cluster.

### ✔ Faster Log Parsing

`LogParser` now updates every 200 ms instead of 1 second → gauges animate smoothly and follow live changes more closely.

### ✔ Startup Script Delay

`./launch.sh` now waits **5 seconds** before opening the GUI, ensuring the server/client produce telemetry first.

------

## How to Build

```
cd gui
mkdir build && cd build
cmake ..
make
```

## How to Run

```
./launch.sh 
```

This starts:

1. Server
2. Client
3. GUI 
