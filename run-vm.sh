#!/bin/bash

# Default values
CMDLINE="earlycon=pl011,0x09000000"
DUMPDTB=""
DTB=""
SHARED_DIR=./shared

usage() {
    echo "Usage: $0 -k <path_to_kernel_image> -i <path_to_cloud_image>"
    exit 1
}

# Parse command-line arguments
while getopts ":k:i:" opt; do
    case ${opt} in
        k)
            KERNEL_IMAGE=$OPTARG
            ;;
        i)
            CLOUD_IMAGE=$OPTARG
            ;;
        \?)
            echo "Invalid option: $OPTARG" 1>&2
            usage
            ;;
        :)
            echo "Option -$OPTARG requires an argument." 1>&2
            usage
            ;;
    esac
done
shift $((OPTIND -1))

# Check if required arguments are provided
if [ -z "$KERNEL_IMAGE" ] || [ -z "$CLOUD_IMAGE" ]; then
    echo "Error: Both kernel image and cloud image paths are required."
    usage
fi

# Run QEMU
qemu-system-aarch64 -nographic \
    -machine virt,gic-version=2 \
    -m 1024 \
    -cpu cortex-a72 \
    -append "console=ttyAMA0 root=/dev/vda rw $CMDLINE" \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -device virtio-net-pci,netdev=net0,mac=de:ad:be:ef:41:49 \
    -kernel "$KERNEL_IMAGE" \
    -drive file="$CLOUD_IMAGE",format=raw \
    -virtfs local,path=$SHARED_DIR,mount_tag=shared,security_model=passthrough,readonly

exit 0
