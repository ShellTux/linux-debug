# Linux Debugging

```sh
git submodule update --init --recursive
```

## Compiling kernel

```sh
cd linux
make defconfig
make menuconfig
# [ ] - turn off
# [x] - turn on

# [ ] Processor type and features >>> Randomize the address of the kernel image (KASLR) -> This is a security feature, we will disable it so we can debug it with gdb
# [ ] Virtualization -> No need for virtualization
# [ ] Enable loadable module support -> No need for loading modules at runtime
# [ ] Networking support -> No need to reach over the network
# [x] Kernel hacking >>> Compile-time checks and compiler options >>> Debug information >>> Rely on the toolchain's implicit default DWARF version
# [x] Kernel hacking >>> Compile-time checks and compiler options >>> Provide GDB scripts for kernel debugging
make --jobs=$(nproc)
```

## Compiling my own utilities to replace GNU coreutils

In `./rootfs/src` directory contains my own simple reimplementations of some
GNU coreutils, so I can interact with the system via text/shell. I made this so
I didn't need to pull the source code of coreutils to this repo and it is
necessary to have static linked executables because dynamic linking is not set
up.

Feel free to read the source code, the most important files are:
- `init.c` -> first program to be called
- `shell.c` -> simple shell

```sh
make --directory=rootfs --jobs=$(nproc)
```

## Running kernel

```sh
./startvm.sh
```

## Debugging the kernel

1. run linux kernel
```sh
./startvm.sh
```
2. Open gdb session
```gdb
(gdb) break load_script
(gdb) target remote :1234
```
