# Telemetry Logging System – Phase 4

**Asynchronous, Multi-Sink, Threaded Logging Framework**

## 📌 Overview

This phase implements a **multithreaded telemetry logging system** that reads telemetry data from a file, formats it according to a policy, and logs it asynchronously to multiple sinks (console + files).

The design focuses on:

- Producer / Consumer separation
- Thread safety
- Proper object lifetime management
- Clean shutdown without deadlocks or undefined behavior

This phase is intentionally **systems-level**, dealing with concurrency, synchronization, and virtual dispatch safety.

## 🧱 Architecture

```
Telemetry Source (File)
        |
        v
   Formatter Task
        |
        v
 RingBuffer<logmessage>
        |
        v
   Logger Task
        |
        v
  LogManager
        |
        v
 +-------------+
 | ConsoleSink |
 | FileSink(s) |
 +-------------+
```

## 🧵 Threading Model

- **ThreadPool**
  - Fixed number of worker threads
  - Task queue backed by a ring buffer
  - Clean shutdown with join semantics
- **Producer (Formatter Task)**
  - Reads raw telemetry from `telemetry.txt`
  - Converts raw data → `logmessage` using a policy
  - Pushes formatted messages into a shared ring buffer
- **Consumer (Logger Task)**
  - Waits on a condition variable
  - Pops formatted messages
  - Logs to all registered sinks
  - Flushes output deterministically

## 🔄 Synchronization Strategy

- `RingBuffer<logmessage>`
   Used as a bounded queue between producer and consumer.
- `std::mutex + std::condition_variable`
  - Producer notifies when new data is available
  - Consumer blocks when queue is empty
  - `done` flag signals end-of-stream cleanly

This avoids:

- Busy waiting
- Data races
- Lost notifications

------

## 🧩 Core Components

### 1️⃣ Telemetry Source

**`FileTelemetrySourceImpl`**

- Reads telemetry data line-by-line from a file
- Acts as a real-world input abstraction

------

### 2️⃣ Formatter

**`LogFormatter<Policy>`**

- Converts raw telemetry into structured `logmessage`

- Policy-based design allows:

  - CPU telemetry

  - RAM telemetry

  - Future extensions without modifying core logic

  - ### 3️⃣ LogManager

    - Central logging coordinator
    - Fan-out logging to multiple sinks
    - Owns no sinks (non-owning references)
    - Thread-safe logging interface

    ------

    ### 4️⃣ Sinks

    - **ConsoleSink** → stdout
    - **FileSink** → persistent logs
    - Virtual interface with concrete implementations

    ------

    ### 5️⃣ ThreadPool

    - Task-based execution
    - Safe shutdown:
      - Workers exit first
      - Threads join
      - No tasks outlive their dependencies

    ------

    ## ⚠️ Important Design Lesson (Key Takeaway)

    > **Threads must never outlive the objects they use.**

    This phase explicitly handles:

    - Virtual function calls across threads
    - Proper destruction order
    - Avoiding `pure virtual method called` crashes

    ThreadPool destruction is guaranteed **before**:

    - `LogManager`
    - `ConsoleSink`
    - `FileSink`

## 🛑 Shutdown Flow

1. Telemetry source finishes reading
2. Producer sets `done = true`
3. Consumer drains remaining messages
4. Worker threads exit
5. ThreadPool joins all threads
6. Logger and sinks are destroyed safely

No sleeps. No races. No UB.

------

## ▶️ How to Run

1. Prepare telemetry file:

```
telemetry.txt
```

Example content:

```
1
2
3
4
5
6
```

## ▶️ Build & Run

To build and run the telemetry logging system, follow these steps:

```
# Generate build files in a separate directory
cmake . -B build -DBUILD_TYPE=MAIN

# Go to the build directory
cd build

# Compile the program
make

# Run the executable
./program
```
