# Introduction

IO module covers input and output devices. Input and output states are included in Statuses and outputs can be changed using Commands.

For detailed documentation see [IO Module documentation](./doc/IO_module_doc.md).

# Identification and supported devices

The module number/ID: `2`.

## Devices

| **Device Name**  | **Device Type** | **Device Roles** | Comment                                  |
| --------------   | --------------- | ---------------- | ---------------------------------------- |
| Arduino Opta     | 1               | test             | I/O device for Arduino Opta              |
| Arduino Mega     | 2               | test             | I/O device for Arduino Mega (deprecated) |
| Arduino Uno      | 3               | test2            | I/O device for Arduino Uno               |
| Button           | 4               | test2            | Device with 1 input for button           |

# Messages

## Status

Status messages contain these informations about components:
- array of inputs (of size different for each device)
  - number of input
  - state of input (true when input is high, else false)
- array of outputs (of size different for each device)
  - number of output
  - bool value, which determines if error occurred on output
  - state of output (true when output is high, else false)
- number of button presses

### Example

``` json
[
  [
    { "inNum": 1, "inSt": false },
    { "inNum": 2, "inSt": false },
    { "inNum": 3, "inSt": true },
    { "inNum": 4, "inSt": false },
    { "inNum": 5, "inSt": true },
    { "inNum": 6, "inSt": false }
  ],
  [
    { "outNum": 1, "outErr": false, "outSt": false },
    { "outNum": 2, "outErr": false, "outSt": true },
    { "outNum": 3, "outErr": false, "outSt": true },
    { "outNum": 4, "outErr": false, "outSt": false },
  ],
  { "butPr": 2 },
]
```
## Status Error

Status errors are not defined for this module.

## Command

Command messages contain:
- list of outputAction messages, every outputAction message contains:
  - number of output, which this action will affect
  - ActionType enumeration, which could be SET_ON, SET_OFF, TOGGLE

### Example

``` json
[
    {"outNum": 1, "actType": 1},
    {"outNum": 2, "actType": 1},
    {"outNum": 3, "actType": 2}
]
```

# Requirements

 - [CMakeLib](https://github.com/cmakelib/cmakelib)

# Build

```bash
mkdir _build && cd _build
cmake .. -DCMLIB_DIR=<path-to-cmakelib-dir> -DCMAKE_BUILD_TYPE=Release [-DBRINGAUTO_INSTALL=ON] [-DBRINGAUTO_PACKAGE=ON]
make
```

> CMLIB_DIR directory is an absolute path to [cmlib](https://github.com/cmakelib/cmakelib) repo root directory.

Shared libraries `libio-module-gateway-shared.so` and `libio-external-server-shared.so` should now be compiled in the current directory. Use `libio-module-gateway-shared.so` with module gateway and `libio-external-server-shared.so` with external server.

# Python server

server.py is a minimal external server implemented in Python, which handles communication with module gateway, prints received statuses and sends example command intended for Arduino Opta device, which is connected to module gateway. To use the server, set the mqtt ip address and port on line 149 and run `python server.py`. For fully featured communication you should use [External server](https://github.com/bringauto/external-server) instead.
