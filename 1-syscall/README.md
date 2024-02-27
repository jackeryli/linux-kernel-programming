# Syscall & Shell

## Design

- Add syscall `getcpu` to arm syscall_table
- Write a simple shell

## Knowledge

- stderr no buffer
- stdout has buffer, and it will accumulate until `\n`

## Result

```sh
root@ubuntu:~# /mnt/shell
$ getcpu
0
```

## Reference

- https://pdos.csail.mit.edu/6.828/2023/xv6.html
- https://brennan.io/2015/01/16/write-a-shell-in-c/