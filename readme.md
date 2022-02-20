# CFacts

**A CLI for fetching daily facts.**

Command-line C program for fetching facts from compliant HTTP servers.

Licensed under the MIT license.

## Build instructions

These instructions are designed for Unix systems. There is currently no support
for building on other systems, however PRs to address this are welcome.

### Prerequisites

Make sure you have these tools installed:

- Git
- GNU Make
- Clang
- pkg-config

CFacts also requires these libraries:

- libcurl

Normal system utilities are expected as well.

### Cloning

First, you need to clone the repository locally.

```
git clone -b main "https://github.com/19wintersp/CFacts" cfacts/
cd cfacts/
```

### Building

It is recommended to build CFacts with the provided Makefile. This can simply
be done by running Make with no options.

```
make
```

The output of this should be in the "bin/native/" directory.

To build for another target, you can provide the `TARGET` environment variable.
It should be a valid Clang target triple, and that target should be properly
installed on your system. For example, to build for Windows, you can use the
target `x86_64-pc-windows-gnu` (or the appropriate variant for your
installation).

```
TARGET=x86_64-pc-windows-gnu make -e
```

Compilation for other targets is output into "bin/TARGET/".

Cross-compilation is unstable and likely to break. Specific steps required for
setting up your system so that cross-compilation is possible are not provided
here. Native compilation is recommended where possible.
