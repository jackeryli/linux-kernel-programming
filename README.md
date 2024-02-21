# Linux Kernel Programming

All code has been run on a machine with the following settings:

- Ubuntu 22.04
- Linux kernel 6.5
- qemu-system-aarch64 6.2.0
- glibc 2.36

Follow this [guide](installation.md) to prepare environment.

## Part 1: Add new system call and write a shell

- Add new syscall `getcpu`

## Part 2: Rootkit

- Hide/Unhide rootkit from `lsmod`
- Hook syscalls `execve` and `reboot`

## Reference

This project is modified by CSIE5374. I tried to run experiments on ubuntu 22.04 and kernel 6.5.