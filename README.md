# le_disasm

## Dependencies

- binutils-dev package

## Overview

libopcodes-based linear executable (MZ/LE/LX DOS EXEs) disassembler.
Outputs AT&amp;T syntax by default (switch can be made in code).

This is a continuation of work on
[Syndicate Wars Disassembler 1.0 by Vexillium group](http://swars.vexillium.org/files/swdisasm-1.0.tar.bz2).

## Building

The tool should build correctly on any platform with a working port of libopcodes.

Below there are tested ways of building.

### General building instructions

To build **LE Disassembler**, you will need the following:

* GNU Autotools
* GNU C++ compiler
* libOpcodes (part of GNU Binutils) dev package

Once you've made sure you have the above, proceed with the following steps:

1. go into the directory with `le_disasm` source release (containing `src` `res` etc.)
2. do `autoreconf -if` to create build scripts from templates
3. do `./configure` to make the build scripts find required toolchain and libraries
4. do `make` to compile the executable file

You should now have a working `src/le_disasm` executable file.

#### Build example - 32-bit toolchain of MSYS2 updated 2023-07 on Windows

Using Minimal System and the MinGW toolchain available within, it is possible
to build the executable using the same way as for UNIX systems, with bash and autotools.

First install the dependencies - mingw32, since we aim for 32-bit build:

```
pacman -S mingw-w64-i686-binutils mingw-w64-i686-pkgconf mingw-w64-i686-make mingw-w64-i686-gcc
```

Now as our host is ready, we can start working on the actual `le_disasm` sources.
We will still have to provide paths to 32-bit configuration - MSYS will prefer
folders with data for 64-bit building.
Go to the `le_disasm` folder, and generate build scripts from templates using autotools:

```
autoreconf -ivf --include=/mingw32/share/aclocal/
```

Next, proceed with the build steps; we will do that in a separate folder.
Note how we are modifying PATH environment variable to make shell search for mingw32
binaries before the default mingw64:

```
mkdir -p release; cd release
PATH="/mingw32/bin:$PATH" CFLAGS="-m32" CXXFLAGS="-m32" LDFLAGS="-m32" ../configure
PATH="/mingw32/bin:$PATH" make V=1
```

On success, this will create `release/src/le_disasm.exe` executable file.
Note that you need a pack of 32-bit DLL files from `mingw32` folder to corretly run
the 32-bit executable on 64-bit Windows.

In case you want a debug version of the binary (required for tracing bugs within the
tool), the commands should include optimizations disable and debug symbols enable.
To get such build, go to the `le_disasm` folder; then it would be prudent to create
a separate build directory for debug version, and compile the tool there:

```
mkdir -p debug; cd debug
PATH="/mingw32/bin:$PATH" CPPFLAGS="-DDEBUG" CFLAGS="-m32 -g -O0" CXXFLAGS="-m32 -g -O0" LDFLAGS="-m32 -g -O0" ../configure
PATH="/mingw32/bin:$PATH" make V=1
```
On success, this will create `debug/src/le_disasm.exe` executable file.

## Done

That's all. Have fun using the tool!
