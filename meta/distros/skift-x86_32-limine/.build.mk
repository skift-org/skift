$(RAMDISK).gz: $(RAMDISK)
	@gzip -c $(RAMDISK) > $(RAMDISK).gz

$(KERNEL_BINARY).gz: $(KERNEL_BINARY)
	@gzip -c $(KERNEL_BINARY) > $(KERNEL_BINARY).gz


$(BOOTDISK): $(RAMDISK).gz $(KERNEL_BINARY).gz $(DISTRO_DIRECTORY)/limine.cfg $(ECHFS) $(LIMINE)
	$(DIRECTORY_GUARD)

	rm -f $(BOOTDISK)

	dd if=/dev/zero bs=1M count=0 seek=256 of=$(BOOTDISK)

	parted -s $(BOOTDISK) mklabel msdos
	parted -s $(BOOTDISK) mkpart primary 1 100%

	$(ECHFS) -m -p0 $(BOOTDISK) quick-format 32768
	$(ECHFS) -m -p0 $(BOOTDISK) import $(KERNEL_BINARY).gz kernel.bin.gz
	$(ECHFS) -m -p0 $(BOOTDISK) import $(RAMDISK).gz ramdisk.tar.gz
	$(ECHFS) -m -p0 $(BOOTDISK) import $(DISTRO_DIRECTORY)/limine.cfg limine.cfg
	$(ECHFS) -m -p0 $(BOOTDISK) import thirdparty/limine/limine.sys limine.sys

	$(LIMINE) $(BOOTDISK) 1

QEMU_DISK:=-hda $(BOOTDISK)
