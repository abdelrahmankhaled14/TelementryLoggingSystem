# Telemetry Logging System (Modern C++ / Policy-Based Design)

## Overview

This project implements a **modular telemetry logging system** written in **modern C++ (C++17)**.
 It is designed to demonstrate **core language features**, **type safety**, and **classic design patterns** applied to a realistic systems-level problem.

The system reads raw telemetry data (e.g. CPU / RAM readings), **infers severity at compile time**, formats structured log messages, buffers them, and finally dispatches them to one or more log sinks.

### Key Goals

- Zero runtime ambiguity
- Compile-time correctness where possible
- Clean separation of responsibilities
- Easy extensibility without modifying existing code

------

## Core C++ Concepts Used

- `enum class` (strongly typed enums)
- Templates & policy-based design
- `std::optional` as a type-safe union
- `constexpr` and `if constexpr`
- Compile-time configuration
- Factory & Builder design patterns
- RAII & move semantics
- Interface-based design (dependency injection)

------

## High-Level Architecture

```
Telemetry Source (File / Future Sources)
        |
        v
LogFormatter<Policy>
        |
        v
LogMessage
        |
        v
LogManager
        |
        v
LogSink(s) (Console / File / Socket)
```

Each layer is **fully decoupled** and replaceable.

------

## Strongly Typed Enumerations

To avoid implicit conversions and logic errors, the system exclusively uses `enum class`.

### Defined Enums

- `LogSinkType`
  - `CONSOLE`
  - `FILE`
  - `SOCKET`
- `SeverityLvl`
  - `CRITICAL`
  - `WARNING`
  - `INFO`
- `TelemetrySrc`
  - `CPU`
  - `RAM`

### Why This Matters

- Prevents mixing unrelated values
- Improves API clarity
- Enforces correctness at compile time

------

## Telemetry Sources

### `ITelemetrySource`

An abstract interface representing any telemetry input.

**Responsibilities:**

- Open the source
- Read telemetry data line-by-line

This allows swapping:

- File-based telemetry
- Socket-based telemetry
- Hardware / driver-backed telemetry

### `FileTelemetrySourceImpl`

A concrete implementation that reads telemetry from a file.

This is used by `main()` to simulate incoming telemetry streams.

------

## Policy-Based Log Formatting

### Motivation

Telemetry rules (thresholds, units, severity mapping) **do not change at runtime**.
 Encoding them dynamically would be unnecessary and error-prone.

Instead, the system uses **compile-time policies**.

------

### Formatter Template

```
template<typename Policy>
class LogFormatter;
```

The policy defines:

- Telemetry source
- Units
- Severity thresholds
- Severity inference logic

There is **no inheritance** and **no virtual dispatch**.

------

### Example Policy: RAM

```
struct CpuPolicy
{
    static constexpr float WARNING = 75.0f;
    static constexpr float CRITICAL = 90.0f;
    static constexpr std::string_view unit = "%";
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;

    static constexpr SeverityLvl_enum inferSeverity(float val)
    {
        if (val > CRITICAL)
            return SeverityLvl_enum::CRITICAL;
        if (val > WARNING)
            return SeverityLvl_enum::WARNING;
        return SeverityLvl_enum::INFO;
    }
};
struct RamPolicy
{
    static constexpr int WARNING = 8 ;
    static constexpr float CRITICAL = 12;
    static constexpr std::string_view unit = "GB";
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::RAM;
    static constexpr SeverityLvl_enum inferSeverity(float val)
    {
        if (val > CRITICAL)
            return SeverityLvl_enum::CRITICAL;
        if (val > WARNING)
            return SeverityLvl_enum::WARNING;
        return SeverityLvl_enum::INFO;
    }
```

Other policies (`CpuPolicy`, `RamPolicy`) follow the same structure.

The formatter **expects** the policy to provide `inferSeverity`.
 This is enforced at compile time.

------

## `LogFormatter` Responsibilities

### `formatDataToLogMsg`

- Accepts raw telemetry input as `std::string`
- Parses the value
- Infers severity using the policy
- Generates a timestamp
- Constructs a `LogMessage`

Returns:

```
std::optional<LogMessage>
```

This explicitly represents **success or failure**, without exceptions.

------

### `msgDescription`

Generates a human-readable message describing:

- The telemetry source
- The measured value
- The unit
- The severity meaning

------

### `currentTimeStamp`

Generates the timestamp **at message construction time**, not at dispatch time.

------

## `LogMessage`

A value-type object representing a single log entry.

**Characteristics:**

- No I/O
- No global state
- Fully copyable
- Easy to test
- Independent of output mechanism

**Contains:**

- Source name
- Timestamp
- Telemetry context
- Severity level
- Message payload

------

## Log Sinks

### `Ilogsink`

An interface that defines a single responsibility:

> “Consume a `LogMessage`.”

Concrete implementations include:

- `consolesink`
- `filesink`

This allows:

- Multiple sinks at once
- Easy mocking for tests
- Zero coupling with `LogManager`

------

## `LogManager`

The central coordinator of the system.

**Responsibilities:**

- Accept log messages
- Buffer them internally
- Dispatch them to all configured sinks

### Design Choices

- Knows nothing about telemetry rules
- Knows nothing about formatting
- Only coordinates message flow

This makes it **simple, testable, and reusable**.

------

## Builder Design Pattern (LogManager)

### Why Builder?

`LogManager` requires:

- Buffer capacity
- One or more sinks
- Valid configuration before use

A factory would hide too much.
 A builder **guides construction step-by-step**.

------

### Builder Characteristics

- User must create a builder
- Supports method chaining
- `build()` returns a fully valid `LogManager`

Example:

```
logmanagerbuilder builder(10);
builder.addSink(consoleSink.get())
       .addSink(fileSink.get());

LogManager logManager = builder.build();
```



------

## Main Application Flow

The `main()` function acts as a **composition root** only.

**Responsibilities:**

- Wire components together
- Select policies and sinks
- Run the telemetry pipeline

### Runtime Flow

1. Open telemetry source
2. Build `LogManager` using builder
3. Read telemetry line-by-line
4. Format using `LogFormatter<RamPolicy>`
5. Log valid messages
6. Handle parsing failures
7. Flush logs and print statistics

There is **no business logic** inside `main()`.

------

## Bonus: Template Ring Buffer

To replace `std::vector` buffering, a fixed-size ring buffer is provided.

### Design

- Template-based
- Wraps `std::vector<std::optional<T>>`
- FIFO semantics
- Non-copyable
- Move-enabled

### API

```
bool tryPush(T&& value);
std::optional<T> tryPop();
```

### Benefits

- Bounded memory
- No reallocations
- Explicit failure handling
- Ideal for telemetry systems

------

## Libraries Used

### `magic_enum`

Used for:

- Enum-to-string conversion
- Debug output
- Cleaner logs

**Advantages:**

- Header-only
- Compile-time
- No manual mapping tables

------

## Summary

This project demonstrates:

- Practical modern C++ design
- Compile-time safety without runtime cost
- Proper use of templates and policies
- Correct application of Factory & Builder patterns
- Clean, testable, extensible architecture
