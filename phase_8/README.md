# **📌 Phase 8 — Final System Integration & Runtime Configuration**

## **✨ Overview**

Phase 8 focuses on wrapping the entire telemetry logging pipeline into a **clean, modular, configurable application** using:

- A **Façade Class** (`YourApplicationName`)
- A **runtime JSON configuration file**
- A **simple GUI generator** that produces `config.json`
- Full refactoring of the original `main.cpp` into a clean architecture
- Support for multiple **telemetry sources**
- Support for multiple **log sinks**
- Safe multithreaded producer/consumer pipeline
- Policy-based log formatting for CPU / RAM / Temperature

This phase is all about turning your project into a **real application** rather than a long messy main function.

------

# **🚀 Goals of Phase 8**

### ✔ Replace your giant `main.cpp` with a clean façade

The class:

```
YourApplicationName
```

abstracts **all logic** of:

- threadpool
- telemetry source handling
- parsing
- formatting
- logging
- flushing
- shutting down

into:

```
YourApplicationName app("config.json");
app.start();
```

### ✔ Add runtime configuration (JSON)

Your telemetry logging app no longer needs recompilation to change behavior.

It loads everything from:

```
config.json
```

### ✔ Add a very simple GUI to generate JSON

A local HTML page that creates a valid JSON config file.

### ✔ Support 3 telemetry source types

1. **SOME/IP** → gives 3 values (CPU, RAM, TEMP)
2. **File** → 1 value (policy-defined)
3. **Socket** → 1 value (policy-defined)

### ✔ Support 4 log sinks

- Console
- CPU file
- RAM file
- TEMP file

### ✔ Add mapping for SOME/IP

You can tell the system the order of values:

```
"mapping": ["cpu","temp","ram"]
```

### ✔ Add policy selection for single-value sources

Example:

```
"policy": "temp"
```

# **🧠 System Architecture (Phase 8)**

## **1. Telemetry Source**

There are 3 types:

### 🔵 SOME/IP

Reads a string like:

```
"30;55;60"
```

JSON controls mapping:

```
"mapping": ["cpu","temp","ram"]
```

### 🟢 File Source

Reads a single numeric value → JSON tells what it represents:

```
"policy": "cpu"
```

### 🟣 Socket Source

Same as file — single numeric value → mapped using `"policy"`.

## **2. Policies & Log Formatting**

Each measurement is passed through a formatter based on its category:

- `CpuPolicy`
- `RamPolicy`
- `TempPolicy`

They build a `logmessage` struct containing:

- name ("CPU")
- timestamp
- severity
- context
- text

------

## **3. Producer / Consumer Pipeline**

### 🏭 Producer Thread

- Reads raw values from source
- Parses according to JSON
- Applies correct policy
- Pushes formatted log messages to a lock-free ringbuffer

### 📥 Consumer Thread

- Pops messages from ringbuffer
- Sends them to `LogManager`
- Flushes periodically

This ensures high throughput and non-blocking logging.

## **4. LogManager + Sinks**

JSON file enables/disables sinks:

```
"sinks": {
  "console": true,
  "cpu_file": true,
  "ram_file": true,
  "temp_file": true
}
```

Your `LogManager` broadcasts messages to all sinks, and each sink filters only what it needs.

------

# **📄 JSON Configuration Structure (Phase 8)**

Here is the JSON your system reads at runtime:

```
{
  "source": {
    "type": "someip",
    "mapping": ["cpu", "temp", "ram"]
  },

  "sinks": {
    "console": true,
    "cpu_file": true,
    "ram_file": true,
    "temp_file": true
  },

  "rates": {
    "parse_ms": 0,
    "log_ms": 0
  }
}
```

### For file source:

```
{
  "source": {
    "type": "file",
    "policy": "cpu"
  },
  ...
}
```

------

# **🖥 Simple GUI Generator (HTML)**

A 100% local, self-contained HTML file lets you choose:

- source type
- SOME/IP mapping
- policy for file/socket
- enabled sinks
- rates

Then click **Generate → Download config.json**.

This GUI completely removes the need to hand-write JSON.

# **🏗 The Façade Class — `YourApplicationName`**

Inside this class we wrapped:

- ThreadPool initialization
- LogManager builder setup
- Sink attachment
- Source initialization
- Producer thread
- Consumer thread
- RingBuffer
- JSON parsing
- Shutdown logic

So your real `main.cpp` becomes:

```
int main() {
    YourApplicationName app("config.json");
    app.start();
    return 0;
}
```

# **🧵 Threading Diagram**

```
      +--------------------------+
      |  Telemetry Source        |
      | (SOME/IP | File | Socket)|
      +--------------------------+
                   |
                   v
         +-------------------+
         |   Producer Thread |
         | parse → format    |
         | push → ringbuffer |
         +-------------------+
                   |
                   v
        +---------------------+
        |    RingBuffer       |
        | (lock-free queue)   |
        +---------------------+
                   |
                   v
         +-------------------+
         |  Consumer Thread  |
         | pop → LogManager  |
         | flush periodically|
         +-------------------+
                   |
                   v
     +-------------+--------------+
     |   Sinks (configurable)     |
     | console / cpu.log / ...    |
     +-----------------------------+
```
