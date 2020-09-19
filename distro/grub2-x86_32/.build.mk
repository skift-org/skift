$(BOOTDISK): $(RAMDISK) $(KERNEL_BINARY) $(DISTRO_DIRECTORY)/grub.cfg
	$(DIRECTORY_GUARD)
	@echo [GRUB-MKRESCUE] $@

	@mkdir -p $(BOOTROOT)/boot/grub
	@cp $(DISTRO_DIRECTORY)/grub.cfg $(BOOTROOT)/boot/grub/
	@cp $(RAMDISK) $(BOOTROOT)/boot/
	@cp $(KERNEL_BINARY) $(BOOTROOT)/boot/kernel.bin

	@grub-mkrescue -o $@ $(BOOTROOT) || \
	 grub2-mkrescue -o $@ $(BOOTROOT)
