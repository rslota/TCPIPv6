## TCP/IP Stack

### Overview

This project is an implementation of TCP/IPv6 stack. It is intended to be
portable between different hardware and operating systems. The `hw.h` specifies
functions that have to be provided by a target platform. An implementation for
x86 Linux-based platform is provided in `hw.c` file.

The stack was created as an academic research project. It doesn't implement the
whole TCP/IPv6 specification. Particularly no optional IPv6 are supported, and
the TCPv6 implementation is incomplete (although functional). The code is not
intended for production use.

### Compilation

The code relies on some GCC features like `attribute(packed)` and support of
`-std=gnu11` flag. Therefore it can only be compiled with a modern
GCC-compatible compiler. CMake is also a dependency.

To compile the project issue following commands in the terminal:
```sh
$ mkdir build/
$ cd build
$ cmake ..
$ make
```

The compilation will output build/src/libtcpip.a library and build/src/test
executable.

### Usage

The project includes a basic `main.c` which contains a main function to
demonstrate some of the functionality.

The `net.h` header contain main function declarations of the library's
interface. The TCP listen function has no `net.h` wrapper and has to be
accessed directly from the `tcp.h`. While both `tcp.h` and `udp.h functions
can be accessed directly, we recommend not using functions of IP layer and
lower.

The following example code sends a TCP message. The first argument of net_init
contains a string by which the platform-specific implementation selects a
network interface. The Linux implementation connects directly to a raw packet
socket, so the executable needs appropriate system privileges (see
`man 7 packet` for more details).

```C
#include "net.h"

extern uint8_t *src_ip;   // must contain an src IPv6 IP. This doesn't have to
                          // be an address given to the network interface by the
                          // kernel
extern uint16_t src_port; // must contain an src port.

extern uint8_t *dst_ip;   // must contain a destination IPv6 IP.
extern uint16_t dst_port; // must contain an dst port.

const char *buffer = "Hello World!";
session_t *session = net_init("eth0", src_ip, src_port, dst_ip, dst_port, TCP, -1);
net_send(session, 0, 0, (uint8_t*) buffer, strlen(buffer));
net_free(session);
```

### Documentation

The headers are fully documented in-code using Doxygen. The documentation can
be generated using the `make doc` command. `build/doc/Doxyfile.in` will contain
default Doxygen settings for the project. By default HTMLa and LaTeX
documentation is generated.
