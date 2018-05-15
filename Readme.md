# Witcher
Attempt to write x86 kernel in C++ based on vlog [Write your own Operating System](https://www.youtube.com/channel/UCQdZltW7bh1ta-_nCH7LWYw).

Aim of this project is to learn from first hand how kernel works in order to build a microkernel in Rust for other architectures.

# How to build

To build the mykernel.bin file you need gcc (i386).

```
	make
```

# How to run

To run the kernel you can use QEMU.
```
	make qemu
```

# Progress

* [x] Hello world
* [x] Memory management (GDT)
* [x] Ports
* [ ] Interrupts