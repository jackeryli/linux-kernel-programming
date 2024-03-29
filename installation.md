# Prepare Environment for kernel development

All code has been run on a machine with the following settings:

- Ubuntu 22.04
- Linux kernel 6.5
- qemu-system-aarch64 6.2.0
- glibc 2.36

The original coruse is used ubuntu 18.04 and kernel 5.4. Follow this [tutorial](/ubuntu18.md) if you want to run on the same environment.

## Prerequisite

### Prepare Building environment

```sh
sudo apt update
sudo apt install git build-essential libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev ninja-build
sudo apt install flex bison
```

### Install Cross compiler

```sh
sudo apt update
sudo apt install gcc-aarch64-linux-gnu
```

### Compile Linux

- If you have more cpu cores, you can set higher cores on `make`: `-j8`
- `make oldconfig` will ask you several question, click `enter` to all questions

```sh
git clone https://github.com/torvalds/linux.git
cd linux
git checkout tags/v6.5
cp $PATH_TO_defconfig .config
yes "" | make oldconfig ARCH=arm64
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j4
```

It will build the image here:

```sh
$PATH_TO_YOUR_LINUX/arch/arm64/boot/Image
```

### Install Qemu

```sh
sudo apt-get install qemu-system-arm
```

### Create Virtual Disk Image

First download ubuntu 22.04 file system binaries:

```sh
wget https://cloud-images.ubuntu.com/releases/22.04/release-20221101.1/ubuntu-22.04-server-cloudimg-arm64-root.tar.xz
```

Create virtual disk image and mount on `/mnt`.

```sh
make qemu-img
```

Modify `/mnt/etc/passwd` to disable root password

```sh
root::0:0:root:/root:/bin/bash
```

Umount the file system image

```sh
sudo umount /mnt
```

### Run Linux VM

```sh
make qemu
```

Your Linux image is here:

```sh
$PATH_TO_YOUR_LINUX/arch/arm64/boot/Image
```

## Share files between qemu and host

```sh
$ make qemu
...

Ubuntu 22.04 LTS

ubuntu login: root

...

root@ubuntu:~# mount -t 9p -o trans=virtio shared /mnt
```

Or you can put this line to `etc/fstab`

```
shared /mnt 9p trans=virtio 0 0
```
