# myfs

- A simple in-memory file system

## Useful Library

- fs/libfs.c: `simple_dir_inode_operations`, `simple_dir_operations`

## Result

```sh
root@ubuntu:~# mount -t myfs none /home/myfs
root@ubuntu:~# ls -al /home/myfs
total 4
dr-xr-xr-x 2 root root    0 Feb 26 00:43 .
drwxr-xr-x 3 root root 4096 Feb 25 22:46 ..
```

## Reference

- ramfs
- debugfs
- procfs
