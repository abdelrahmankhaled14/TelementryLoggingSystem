# 📡 Telemetry Logging System — Phase 6

### **Real-Time Logging • CommonAPI SOME/IP • GUI Dashboard (Qt/QML)**

This project implements a full embedded-style telemetry pipeline:

1. **Telemetry Server**
    Provides CPU usage values via **CommonAPI SOME/IP**.
2. **Telemetry Logger (program)**
    Reads telemetry values, formats them, logs them into `build/log1.txt`.
3. **Qt/QML GUI Dashboard**
    Instead of connecting to CommonAPI, the GUI **parses the log file** and animates the CPU values one-by-one as if playing a real-time stream.

The GUI includes:

- Animated round gauge
- Real-time line graph (history)
- Smooth playback of logged telemetry
- Auto-looping animation
- Clean dark UI

------

# 📂 Project Structure

```
phase_6/
├── src/                      # Logger + program + server sources
├── common-api/               # FIDL + FDEPL
├── src-gen/                  # CommonAPI generated sources
├── gui/                      # GUI application (Qt/QML)
│   ├── main.cpp
│   ├── SystemMonitorBackend.h
│   ├── SystemMonitorBackend.cpp
│   └── qml/
│       └── main.qml
├── vsomeip_config.json
└── build/                    # CMake build output
    ├── server
    ├── program
    ├── libmylib.a
    ├── log1.txt              # Parsed by GUI
```

# ⚙️ Build Instructions

## 1️⃣ Build Server + Logger (program)

From `phase_6/`:

```
mkdir -p build
cd build
cmake ..
make -j
```

This produces:

- `build/server`
- `build/program`
- `build/log1.txt` (runtime generated)

------

## 2️⃣ Build GUI

```
cd gui
mkdir -p build
cd build
cmake ..
make -j
```

This creates:

- `gui/build/telemetry_gui`

## 1️⃣ Start the server

```
cd phase_6/build
./server
```

Expected output:

```
[Server] Starting...
[Server] Running...
```

------

## 2️⃣ Start the logger

```
cd phase_6/build
./program
```

This will generate new telemetry logs:

```
[INFO] TelemetryApp (CPU): CPU usage: 74%
[WARNING] TelemetryApp (CPU): CPU usage: 89%
...
```

All logged to:

```
phase_6/build/log1.txt
```

------

## 3️⃣ Run the GUI (reads log1.txt in real-time simulation)

```
cd phase_6/gui/build
env -u LD_LIBRARY_PATH ./telemetry_gui
```

Why `env -u LD_LIBRARY_PATH`?
 → Prevents Qt from loading broken snap-libs.

Expected output:

```
[GUI] Loaded 250 CPU samples from log.
```

### GUI Behavior

- Reads *all* CPU samples from `log1.txt`
- Animates them every **200 ms**
- Loops automatically
- Updates:
  - CPU gauge
  - Real-time graph
  - Live numeric value
  - Color based on severity

------

# 🏗️ GUI Implementation Details

### Backend class (`SystemMonitorBackend`)

Handles:

- Loading log file (`../../build/log1.txt`)
- Extracting CPU % using regex
- Timer playback engine
- QML signal emission

### QML (`main.qml`)

Contains:

- CPU Gauge with animation
- History Graph (ListModel + Path)
- Dark dashboard theme
- Play / Pause / Reset controls

------

# 🧪 Log Format Expected

The GUI parses lines like:

```
[2026-02-19 15:30:38] [WARNING] TelemetryApp (CPU): CPU usage: 89%
```

Regex used:

```
CPU usage:\s*(\d+)%
```

The GUI automatically clamps values `0–100`.

# 📌 Environment Variables (Optional for SOME/IP)

```
export VSOMEIP_CONFIGURATION=$PWD/vsomeip_config.json
export VSOMEIP_BASE_PATH=/tmp
export VSOMEIP_APPLICATION_NAME=server
```

------

# 🛠️ Dependencies

### Runtime

- Qt 5.15+ (QML, Quick, GUI)
- GCC / g++
- CMake
- CommonAPI Runtime
- CommonAPI SOME/IP runtime
- vsomeip3 runtime

### Build

```
sudo apt install qtdeclarative5-dev qml-module-qtquick-controls2 qtquickcontrols2-5-dev
sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
```

------

# 🧩 How GUI Reads Data

Unlike earlier CommonAPI attempts, the GUI **does not connect** to the active SOME/IP service.

Why?

- Your desktop Qt build cannot link cleanly with your embedded SOME/IP stack.
- GLIBC conflicts from snap libraries.
- Keeps GUI lightweight and independent.

### Instead, it works like this:

```
logger (program) writes into log1.txt → GUI parses and animates it
```

This is **safer**, **faster**, and **perfect for development**.

------

# 🧭 Usage Summary

| Component | Purpose                   | Command           |
| --------- | ------------------------- | ----------------- |
| Server    | Provides CPU values       | `./server`        |
| Logger    | Reads values + writes log | `./program`       |
| GUI       | Visualizes CPU values     | `./telemetry_gui` |
