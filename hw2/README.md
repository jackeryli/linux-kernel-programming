# HW2

## Compile Kernel

### Method 1: kprobe


### Method 2: Modify kernel

After `kernel 5.7`, it does not export `kallsyms_lookup_name` by default.
Add this line to `kernel/kallsyms.c:268`

```sh
EXPORT_SYMBOL_GPL(kallsyms_lookup_name);
```