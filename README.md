This is the implementation of IO module described in [IO module documentation](doc/IO_module_doc.md). The output are shared io_module libraries.

The current IO module supports these devices:
 - Arduino Opta
 - Arduino Mega R3
 - Arduino Uno R4

# Requirements

 - nlohmann json C++ library

# Build

```bash
mkdir _build && cd _build
cmake .. -DCMLIB_DIR=<path-to-cmakelib-dir>
make
```

> cmakelib directory is absolute path to [this](https://github.com/cmakelib/cmakelib) repo root directory.

Now shared libraries `libio_module_module_manager.so` and `libio_module_external_server.so` should be compiled in current directory. Use `libio_module_module_manager.so` with module gateway and `libio_module_external_server.so` with external server.

# Python server

server.py is minimal external server implemented in Python, which handles communication with module gateway, prints received statuses and sends example command intended for Arduino Opta device, which is connected to module gateway. To use server, change ip address and port and run `python server.py`. For fully featured communication you should use External server instead.
