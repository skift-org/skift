$(BOOTDISK): $(RAMDISK) $(KERNEL_BINARY) $(DISTRO_DIRECTORY)/limine.cfg $(ECHFS) $(LIMINE) $(LIMINE_LOADER)
	$(DIRECTORY_GUARD)

	rm -f $(BOOTDISK)

	dd if=/dev/zero bs=1M count=0 seek=256 of=$(BOOTDISK)

	parted -s $(BOOTDISK) mklabel msdos
	parted -s $(BOOTDISK) mkpart primary 1 100%

	$(ECHFS) -m -p0 $(BOOTDISK) quick-format 32768
	$(ECHFS) -m -p0 $(BOOTDISK) import $(KERNEL_BINARY) kernel.bin
	$(ECHFS) -m -p0 $(BOOTDISK) import $(RAMDISK) ramdisk.tar
	$(ECHFS) -m -p0 $(BOOTDISK) import $(DISTRO_DIRECTORY)/limine.cfg limine.cfg

	$(LIMINE) $(BOOTDISK) 1

QEMU_DISK:=-hda $(BOOTDISK)
