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