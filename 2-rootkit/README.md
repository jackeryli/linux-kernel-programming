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

- kill: do not kill.
- execve: record paths executed by `execve` to `dmesg`.
- reboot: do not reboot.

### How to hook original syscall?

#### Method 1: kprobe

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

#### Method 2: Modify kernel

After `kernel 5.7`, it does not export `kallsyms_lookup_name` by default.
Add this line to `kernel/kallsyms.c:268`

```sh
EXPORT_SYMBOL_GPL(kallsyms_lookup_name);
```

## Links

- https://xcellerator.github.io/posts/linux_rootkits_05/
- https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html
- https://xcellerator.github.io/posts/linux_rootkits_01/
- https://xcellerator.github.io/posts/linux_rootkits_02/
- https://xcellerator.github.io/posts/linux_rootkits_11/