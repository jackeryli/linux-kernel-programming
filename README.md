# Linux Kernel Programming

All code has been run on a machine with the following settings:

- Ubuntu 22.04
- Linux kernel 6.5
- qemu-system-aarch64 6.2.0
- glibc 2.36

Follow this [guide](installation.md) to prepare environment.

## Part 1: Add new system call and write a shell

- Add new syscall `getcpu`
- Write a simple shell and call `getcpu`

## Part 2: Rootkit

- Hide/Unhide rootkit from `lsmod`
- Hook syscalls `execve` and `reboot`
- Protect module list and `sys_call_table` by `mutex`

## Part 3: Transfer data between user and kernal

- Trasfer data with `copy_to_user` and `copy_from_user`
- Use `kmalloc` to allocate memory at kernel
- Memory should be protected by `mutex` 

## Part 4: procfs

## Part 5: myfs

- A simple in-mermoy file system


## Reference

This project is modified by CSIE5374. I tried to run experiments on ubuntu 22.04 and kernel 6.5.