# Phase 5: CommonAPI vSOME/IP Telemetry Implementation

## Overview

Phase 5 implements a telemetry client-server system using **CommonAPI** with **vSOME/IP** binding. The server generates random load percentages and sends them to the client through CommonAPI's RPC mechanism.

## Architecture

```
┌─────────────────┐                    ┌─────────────────┐
│    SERVER       │                    │    CLIENT       │
├─────────────────┤                    ├─────────────────┤
│ LoggingService  │ ────vSOME/IP───→  │ LoggingProxy    │
│ (StubDefault)   │ ←──Response────    │                 │
│                 │                    │                 │
│ Generates       │                    │ Calls           │
│ Random Load %   │                    │ requestData()   │
│ 0-100           │                    │                 │
└─────────────────┘                    └─────────────────┘
```

## Components

### Server (`src/server.cpp`)
- Implements `loggingStubDefault` interface
- Generates random load percentages (0-100)
- Responds to `requestData()` method calls
- Uses CommonAPI's RPC mechanism

### Client (`src/main.cpp`)
- Builds proxy to `loggingProxy`
- Calls `requestData()` method
- Receives percentage from server
- Logs data using existing logging framework

### Generated Files (`src-gen/v1/logger/methods/`)
- `loggingProxy.hpp` - Client-side proxy
- `loggingStub.hpp` - Server-side stub
- `logging.hpp` - Interface definition
- `loggingSomeIPProxy.hpp` - vSOME/IP proxy implementation
- `loggingSomeIPStubAdapter.hpp` - vSOME/IP stub adapter
- `loggingSomeIPDeployment.hpp` - vSOME/IP configuration

## Prerequisites

### Required Packages
```bash
sudo apt-get install libvsomeip3-dev cmake build-essential
```

### CommonAPI Installation
Build from source:

```bash
# Core Runtime
cd ~/Downloads/capicxx-core-runtime-3.2.4/
mkdir -p build && cd build
cmake ..
make -j4
sudo make install
sudo ldconfig

# SomeIP Runtime
cd ~/Downloads/capicxx-someip-runtime-3.2.4/
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/usr/local
make -j4
sudo make install
sudo ldconfig
```

### Verify Installation
```bash
ldconfig -p | grep CommonAPI-SomeIP
```

Should show:
```
libCommonAPI-SomeIP.so.3 (libc6,x86-64) => /usr/local/lib/libCommonAPI-SomeIP.so.3
```

## Configuration

### 1. CommonAPI Configuration (`/etc/commonapi.ini`)

Verify:
```bash
cat /etc/commonapi.ini
```

Must show blank line between sections:
```
[libcommonapi]
default_binding=someip

[libcommonapi-someip]
default_context_name=someip
default_context_libname=libCommonAPI-SomeIP.so.3

[default]
binding=someip
```

### 2. vSOME/IP Configuration (`vsomeip_config.json`)

Located in project root, contains:
- Service ID: `4666` (0x123A)
- Instance ID: `22136` (0x564A)
- Method ID: `1`
- Unicast Address: `127.0.0.1`
- Ports: `30499` (unreliable)

```json
{
  "unicast" : "127.0.0.1",
  "services" : [
    {
      "service" : 4666,
      "instance" : 22136,
      "unreliable" : "30499"
    }
  ]
}
```

### 3. FIDL Definition (`common-api/x.fidl`)

```fidl
package logger.methods

interface logging {
    version { major 1 minor 0 }
    
    method requestData {
        out {
            Int32 percentage
        }
    }
}
```

### 4. Deployment Definition (`common-api/x.fdepl`)

```
define org.genivi.commonapi.someip.deployment for interface logger.methods.logging {
    SomeIpServiceID = 4666
    method requestData {
        SomeIpMethodID = 1
        SomeIpReliable = false
    }
}

define org.genivi.commonapi.someip.deployment for provider as Service {
    instance logger.methods.logging {
        InstanceId = "logger.methods.justSendHi"
        SomeIpInstanceID = 22136
        SomeIpUnicastAddress = "127.0.0.1"
        SomeIpReliableUnicastPort = 30499
        SomeIpUnreliableUnicastPort = 30499
    }
}
```

## Build Instructions

### Generate Code from FIDL

```bash
cd common-api
commonapi-generator x.fidl -sk vSomeIP
```

This creates files in `src-gen/v1/logger/methods/`

### Build Project

**Terminal 1 - Build Server:**
```bash
cd build
rm -rf *
cmake -DBUILD_TYPE=SERVER ..
make
```

**Terminal 2 - Build Client:**
```bash
cd build
rm -rf *
cmake -DBUILD_TYPE=MAIN ..
make
```

## Running the Application

### Start Server

**Terminal 1:**
```bash
cd ~/projects/linux/C++/project/TelementryLoggingSystem/phase_5/build

# Set configuration
sudo bash -c 'printf "[libcommonapi]\ndefault_binding=someip\n\n[libcommonapi-someip]\ndefault_context_name=someip\ndefault_context_libname=libCommonAPI-SomeIP.so.3\n" > /etc/commonapi.ini'

# Run server
export VSOMEIP_CONFIGURATION=../vsomeip_config.json
./server
```

Expected output:
```
========================================
[Server] Starting...
========================================

[Server] Registering service...
[Server] Service registered!
[Server] Waiting for requests...

[Server] Running...
```

### Start Client

**Terminal 2:**
```bash
cd ~/projects/linux/C++/project/TelementryLoggingSystem/phase_5/build

export VSOMEIP_CONFIGURATION=../vsomeip_config.json
./program
```

Expected output:
```
========================================
[Client] Starting...
========================================

[Client] Waiting for service...
[Client] Service available!

[Client] Iteration 1: Got 45%
[Client] Iteration 2: Got 72%
[Client] Iteration 3: Got 38%
...
```

## How It Works

### Server Flow
1. Create `LoggingService` instance (implements stub)
2. Get CommonAPI runtime
3. Register service with domain="local", instance="logger.methods.justSendHi"
4. Loop forever, waiting for client requests
5. When client calls `requestData()`:
   - Generate random percentage
   - Return via callback `_reply(percentage)`

### Client Flow
1. Get CommonAPI runtime
2. Build proxy for domain="local", instance="logger.methods.justSendHi"
3. Wait for service to be available
4. Loop forever:
   - Call `proxy->requestData(status, percentage)`
   - Check `status == CallStatus::SUCCESS`
   - Log the percentage
   - Sleep 1 second

## Troubleshooting

### "Using default binding 'dbus'"

**Problem:** CommonAPI is using DBus instead of vSOME/IP

**Solution:** 
1. Check `/etc/commonapi.ini` has blank line between sections
2. Run config command exactly as shown above
3. Verify with: `cat /etc/commonapi.ini | od -c | head -20`
4. Should show `\n \n` between sections

### "Service not available"

**Problem:** Client can't find server

**Solutions:**
- Make sure server is running first
- Check vSOME/IP configuration matches
- Verify `VSOMEIP_CONFIGURATION` environment variable is set
- Check ports 30499 are not blocked

### "Failed to register"

**Problem:** Server can't register service

**Solutions:**
- Check `/etc/commonapi.ini` is correct
- Verify CommonAPI-SomeIP is installed: `ldconfig -p | grep CommonAPI-SomeIP`
- Check vSOME/IP daemon is running
- Try different port numbers in config

### Build Errors

**Problem:** CMake can't find CommonAPI headers

**Solutions:**
```bash
# Rebuild CMake cache
cd build
rm -rf *
cmake ..
make
```

**Problem:** Linker errors for CommonAPI-SomeIP

**Solution:** Reinstall CommonAPI-SomeIP
```bash
cd ~/Downloads/luck/commanapi/capicxx-someip-runtime-3.2.4/
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/usr/local
make -j4
sudo make install
sudo ldconfig
```

## File Structure

```
phase_5/
├── CMakeLists.txt                    # Build configuration
├── vsomeip_config.json              # vSOME/IP configuration
├── common-api/
│   ├── x.fidl                       # Interface definition
│   └── x.fdepl                      # Deployment configuration
├── include/
│   ├── ITelemetrySource.hpp         # Base interface
│   └── CommonAPITelemetrySourceImpl.hpp
├── src/
│   ├── main.cpp                     # Client code
│   ├── server.cpp                   # Server code
│   └── CommonAPITelemetrySourceImpl.cpp
├── src-gen/                          # Generated code (do not edit!)
│   └── v1/logger/methods/
│       ├── loggingProxy.hpp
│       ├── loggingStub.hpp
│       ├── loggingSomeIPProxy.hpp
│       └── loggingSomeIPStubAdapter.hpp
└── build/                            # Build output
    ├── server                        # Server executable
    └── program                       # Client executable
```

## Key Concepts

### CommonAPI
- **Abstraction layer** for remote procedure calls (RPC)
- Supports multiple bindings: DBus, SomeIP, etc.
- Generated code from FIDL definitions

### vSOME/IP (Scalable service-Oriented MiddlE ware over IP)
- **Automotive standard** for inter-process communication
- **Lightweight** and **fast**
- Used in vehicle ECU networks

### FIDL (Franca IDL)
- **Interface definition language** for CommonAPI
- Define data types, methods, signals
- Generator creates proxy/stub code

### Proxy (Client Side)
- **Proxy object** on client side
- Calls look like local method calls
- Actually sends RPC over network

### Stub (Server Side)
- **Stub object** on server side
- Implements actual business logic
- Receives RPC calls and executes methods

## Testing

### Manual Test
```bash
# Terminal 1: Start server
cd build && ./server

# Terminal 2: Start client  
cd build && ./program

# Should see iterations printing with random percentages
```

### What to Check
- Server prints "Service registered!"
- Client prints "Service available!"
- Client iterations show random numbers 0-100
- No errors or warnings

## Next Steps

After Phase 5 works:

1. **Integrate with your logging system**
   - Replace simple percentage output with full logging

2. **Add more methods**
   - Create additional RPC methods for different data types

3. **Handle multiple clients**
   - CommonAPI supports multiple concurrent clients

4. **Add events/signals**
   - Server can broadcast events to subscribed clients

5. **Optimize for production**
   - Add error handling
   - Implement timeouts
   - Add security policies

## References

- **CommonAPI Documentation:** https://github.com/COVESA/commonapi_core_runtime
- **vSOME/IP:** https://github.com/COVESA/vsomeip
- **FIDL Specification:** https://github.com/COVESA/capicxx-core-tools

## Summary

Phase 5 successfully implements:
- ✅ vSOME/IP communication between client and server
- ✅ CommonAPI RPC mechanism
- ✅ Code generation from FIDL
- ✅ Random data generation on server
- ✅ Synchronous request-response pattern
- ✅ Integration with existing logging framework

