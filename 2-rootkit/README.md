# Part2: Rootkit

- Hide rootkit from `lsmod`
- Hook syscall

## Hide Rootkit

## Hook syscall

- kill 
- execve
- reboot

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

- https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html
- https://xcellerator.github.io/posts/linux_rootkits_01/
- https://xcellerator.github.io/posts/linux_rootkits_02/
- https://xcellerator.github.io/posts/linux_rootkits_05/
- https://xcellerator.github.io/posts/linux_rootkits_11/