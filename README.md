This is the implementation of IO module as described in [IO module documentation](doc/IO_module_doc.md). The output are shared io_module libraries.

The current IO module supports these devices:
 - Arduino Opta
 - Arduino Mega R3
 - Arduino Uno R4

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
