# Telemetry Logging System — Phase Two

## Data Sources & Smart Resource Management (Modern C++ / RAII)

## Overview

Phase Two extends the telemetry logging system into **real data acquisition and resource ownership**.

Instead of simulated or abstract inputs, the system now introduces **actual operating-system–backed data sources** such as files and Unix Domain Sockets.
 The core objective of this phase is to demonstrate **correct, deterministic resource management** using **modern C++ RAII principles**.

This phase deliberately focuses on **lifetime, ownership, and safety**, rather than features or performance tricks.

------

### Key Goals

- Model **real telemetry sources** (files / sockets)
- Enforce **correct ownership semantics**
- Eliminate resource leaks by construction
- Apply **Rule of 3 / 5 / 0** intentionally
- Separate *resource management* from *business logic*

------

## Core C++ Concepts Used

- References and ownership semantics
- Move vs copy semantics
- Rule of 3 / 5 / 0
- Special member functions
- Interface-based design
- RAII (Resource Acquisition Is Initialization)
- Zero-cost abstractions over POSIX APIs

------

## Design Philosophy

This phase enforces a simple rule:

> **If your object exists, the resource must be valid.
>  If the object is destroyed, the resource must be released.**

No manual cleanup paths.
 No “remember to call close()”.
 No duplicated ownership.

------

## High-Level Architecture

```
OS Resource (File / UDS)
        |
        v
RAII Wrapper (SafeFile / SafeSocket)
        |
        v
Telemetry Source Implementation
        |
        v
ITelemetrySource Interface
        |
        v
Logging System (Phase 1)
```

Each layer has **exactly one responsibility**.

------

## Telemetry Source Abstraction

### `ITelemetrySource`

An interface representing any continuous telemetry data provider.

```
class ITelemetrySource {
public:
    virtual ~ITelemetrySource() = default;

    virtual bool openSource() = 0;
    virtual bool readSource(std::string& out) = 0;
};
```

### Responsibilities

- Establish a connection or open a resource
- Read the *latest available* telemetry value
- Abstract away *how* the data is obtained

This allows the logging system to remain **completely agnostic** to:

- Files
- Sockets
- Future hardware-backed sources

------

## RAII Resource Wrappers

Raw POSIX handles are **never exposed directly**.

Instead, they are wrapped in **move-only RAII types**.

------

### `SafeFile`

A safe wrapper around a POSIX file descriptor.

**Characteristics:**

- Owns exactly one file descriptor
- Automatically closes the file in its destructor
- Copy semantics are **explicitly deleted**
- Move semantics are **explicitly enabled**

```
SafeFile(const SafeFile&) = delete;
SafeFile& operator=(const SafeFile&) = delete;

SafeFile(SafeFile&&) noexcept;
SafeFile& operator=(SafeFile&&) noexcept;
```

### Why Move-Only?

- File descriptors represent **unique ownership**
- Copying would introduce double-close bugs
- Move semantics cleanly transfer responsibility

------

### `SafeSocket`

A safe wrapper around Unix Domain Socket operations.

**Characteristics:**

- Encapsulates socket creation, binding, and cleanup
- Guarantees `close()` on destruction
- Copy semantics are **forbidden**
- Move semantics allow ownership transfer

This enables sockets to be:

- Returned from functions
- Stored in containers
- Passed between components safely

------

## Rule of Zero Telemetry Sources

Telemetry source implementations **do not manage raw resources directly**.

Instead, they rely entirely on:

- RAII members
- Standard library types

This allows them to follow the **Rule of Zero**.

------

### `FileTelemetrySourceImpl`

A concrete implementation of `ITelemetrySource`.

**Responsibilities:**

- Own a `SafeFile`
- Open a telemetry file
- Read values line-by-line
- Forward raw data to the logging system

The data source is a shell script that continuously writes floating-point values into a file.

No manual cleanup code exists in this class.

------

### `SocketTelemetrySourceImpl`

A socket-backed telemetry source.

**Responsibilities:**

- Own a `SafeSocket`
- Connect to a Unix Domain Socket endpoint
- Receive telemetry data from another process

This enables:

- Inter-process telemetry
- Decoupled producers and consumers
- Realistic system-level IPC

------

## Refactoring `LogMessage`

As part of Phase Two, `LogMessage` is refactored to:

- Default **all special member functions**
- Remove unnecessary custom implementations

### Result

- Trivially copyable / movable
- Clear value semantics
- No ownership concerns
- Fully compliant with Rule of Zero

This reinforces when **not** to write special member functions.

------

## Extra: Unix Domain Sockets (UDS)

Unix Domain Sockets provide:

- High-performance IPC
- No network stack overhead
- Strong OS-level guarantees

They are widely used in:

- Linux system services
- Daemons
- Logging and monitoring pipelines

This phase introduces them as a **first-class telemetry source**.

------

## Bonus: Real Linux Telemetry

Linux exposes real system metrics via `/proc`.

### Available Sources

- `/proc/stat` → CPU usage
- `/proc/meminfo` → Memory usage

### Bonus Requirements

- Parse real CPU and memory data
- Feed it through:
  - A file-based telemetry source **or**
  - A socket-based telemetry source
- Integrate seamlessly with the logging system

This guarantees the telemetry pipeline processes **meaningful, real-world data**, not random values.
