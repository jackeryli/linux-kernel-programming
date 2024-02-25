# Read from kernel & write from user

## Design

- Trasfer data with `copy_to_user` and `copy_from_user`
- Use `kmalloc` to allocate memory at kernel
- Memory should be protected by `mutex` 

## Result

```sh
root@ubuntu:~# insmod /mnt/jackdriver
root@ubuntu:~# dmesg | tail
root@ubuntu:~# mknod /dev/jackdriver c [MAJORNUMFROMDMESG] 0
root@ubuntu:~# /mnt/test_rw
Enter 'w' to write, 'r' to read, or 'e' to exit: r
Data read from device: Hello Jack!
Enter 'w' to write, 'r' to read, or 'e' to exit: w
Enter string to write: jackisme
Enter 'w' to write, 'r' to read, or 'e' to exit: r
Data read from device: jackisme

```

## Reference

- https://embetronicx.com/tutorials/linux/device-drivers/linux-device-driver-tutorial-programming/