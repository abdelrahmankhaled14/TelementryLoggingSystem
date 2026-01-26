# Telemetry Logging System

## Overview

This project implements a **modular, testable telemetry / logging system** written in modern C++ (C++17).
The system is designed with **clean architecture principles**:

- Clear separation of responsibilities
- Interface-based design
- Dependency Injection
- Unit testing with GoogleTest & GoogleMock

The project supports **multiple build modes** using CMake:

- Building the main application
- Building unit tests for individual components

------

## 1. System Architecture & Classes

The system revolves around four core concepts:

```
LogManager  --->  Ilogsink  --->  Concrete Sink (Console / File)
        |
        +--> logmessage
```

Each class has a **single responsibility**, making the system easy to extend and test.

------

### 1.1 `logmessage`

**Purpose:**
Represents a single log entry.

**Responsibilities:**

- Hold log metadata
- Act as a *value object*

**Stored Fields:**

- Logger name
- Timestamp
- Context (module / component)
- Severity level
- Message text

**Key Characteristics:**

- No I/O
- No global state
- Copyable
- Comparable (via `operator==`)

**Why this design?**

- Allows buffering, comparison, and testing
- Keeps log content independent from output method

------

### 1.2 `Ilogsink` (Interface)

**Purpose:**
Defines the abstraction for log output destinations.

**Responsibilities:**

- Provide a common interface for all sinks

**Key Method:**

```cpp
virtual void write(const logmessage& msg) = 0;
```

**Why an interface?**

- Enables dependency injection
- Allows mocking / faking in tests
- Decouples `LogManager` from concrete I/O

------

### 1.3 `consolesink`

**Purpose:**
Concrete implementation of `Ilogsink` that writes logs to standard output.

**Responsibilities:**

- Format log messages
- Output to `std::cout`

**Notes:**

- Used in the main application
- Not used in unit tests

------

### 1.4 `filesink`

**Purpose:**
Concrete implementation of `Ilogsink` that writes logs to a file.

**Responsibilities:**

- Manage file stream
- Persist log messages

**Notes:**

- Demonstrates extensibility of the system
- Can be replaced or extended without touching `LogManager`

------

### 1.5 `LogManager`

**Purpose:**
Central coordinator of the logging system.

**Responsibilities:**

- Accept log messages
- Buffer messages
- Forward messages to the configured sink

**Key Methods:**

```cpp
void log(const logmessage& msg);
void flush();
void clear();
bool isEmpty() const;
size_t getMessageCount() const;
```

**Design Decisions:**

- Uses **dependency injection** (`Ilogsink*`)
- Owns message buffer, not the sink
- No direct knowledge of output mechanism

**Why this matters:**

- Makes the system highly testable
- Allows swapping sinks at runtime

------

## 2. Build System (CMake)

The project uses **CMake ≥ 3.10** and supports **multiple build modes** via a configurable variable.

------

### 2.1 Build Type Variable

A CMake cache variable controls what is built:

```cmake
set(BUILD_TYPE "MAIN" CACHE STRING "Build type: MAIN, TEST_LOGMANAGER, or TEST_LOGMESSAGE")
```

**Valid values:**

- `MAIN`
- `TEST_LOGMANAGER`
- `TEST_LOGMESSAGE`

This allows **one project** to build either:

- the main application
- or a specific test suite

------

### 2.2 Static Library

Core logic is compiled into a reusable static library:

```cmake
add_library(mylib STATIC
    src/logmessage.cpp
    src/filesink.cpp
    src/consolesink.cpp
    src/logmanager.cpp
)
```

**Why a static library?**

- Avoids duplication between main and tests
- Guarantees tests use the same production code

------

### 2.3 Building the Main Application

**Command:**

```bash
cmake -DBUILD_TYPE=MAIN ..
make
```

**Result:**

- Builds `program` using `src/main.cpp`
- Links against `mylib`

------

### 2.4 Building LogManager Tests

**Command:**

```bash
cmake -DBUILD_TYPE=TEST_LOGMANAGER ..
make
```

**Result:**

- Builds unit tests for `LogManager`
- Links GoogleTest and GoogleMock

------

### 2.5 Building LogMessage Tests

**Command:**

```bash
cmake -DBUILD_TYPE=TEST_LOGMESSAGE ..
make
```

**Result:**

- Builds unit tests for `logmessage`

------

### 2.6 Test Discovery

The project uses:

```cmake
gtest_discover_tests(program)
```

**Benefits:**

- Automatic test registration
- No manual `add_test()` needed
- IDE-friendly

------

## 3. Testing Strategy (GoogleTest & GoogleMock)

The project follows **proper unit testing principles**.

------

### 3.1 Why GoogleTest

GoogleTest is used to:

- Verify object state
- Assert correctness of logic
- Ensure regression safety

Examples:

```cpp
EXPECT_EQ(manager.getMessageCount(), 2);
EXPECT_TRUE(manager.isEmpty());
```

------

### 3.2 Why GoogleMock

GoogleMock is used to:

- Verify interactions
- Count method calls
- Assert call order and arguments

This is essential for testing `LogManager::flush()`.

------

### 3.3 Mocking the Sink

A mock sink replaces real I/O:

```cpp
class MockSink : public Ilogsink {
public:
    MOCK_METHOD(void, write, (const logmessage& msg), (override));
};
```

------

### 3.4 Interaction Testing Example

```cpp
EXPECT_CALL(sink, write(::testing::_))
    .Times(2);

manager.flush();
```

This verifies:

- Exact call count
- No extra or missing calls

------

### 3.5 What Is Tested

- Constructor behavior
- Buffer management
- Flush behavior
- Clear behavior
- Edge cases (empty buffer)

------

## Summary

This project demonstrates:

- Clean C++ architecture
- Interface-driven design
- Proper dependency injection
- Modern CMake workflows
- Professional unit testing with gtest & gmock

It is structured to scale, extend, and maintain with confidence.

