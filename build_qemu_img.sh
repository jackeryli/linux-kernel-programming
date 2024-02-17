rm cloud.img
qemu-img create -f raw cloud.img 20g
mkfs.ext4 cloud.img
sudo mount cloud.img /mnt
sudo tar xvf ubuntu-22.04-server-cloudimg-arm64-root.tar.xz -C /mnt
sync
sudo touch /mnt/etc/cloud/cloud-init.disabled