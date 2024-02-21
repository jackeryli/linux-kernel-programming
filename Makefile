KIMG=/home/jack/Desktop/linux/arch/arm64/boot/Image
IMG=cloud
UV=22
IMAGE=$(IMG)$(UV)
IMAGEFILE=$(IMAGE).img

qemu-img:
	qemu-img create -f raw $(IMAGEFILE) 20g
	mkfs.ext4 $(IMAGEFILE)
	sudo mount $(IMAGEFILE) /mnt
	sudo tar xvf ubuntu-$(UV).04-server-cloudimg-arm64-root.tar.xz -C /mnt
	sync
	sudo touch /mnt/etc/cloud/cloud-init.disabled
	sudo sed -i 's/root:x:/root::/' /mnt/etc/passwd

qemu:
	./run_vm.sh -k $(KIMG) -i $(IMAGEFILE)

clean:
	rm *.img
	sudo umount /mnt