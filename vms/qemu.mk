QEMU=qemu-system-x86_64

QEMU_DISK?=-cdrom $(BOOTDISK)

QEMU_FLAGS= \
	-m $(CONFIG_MEMORY)M \
	-serial mon:stdio \
	-rtc base=localtime \
	$(QEMU_DISK)

ifeq ($(CONFIG_DISPLAY),sdl)
	QEMU_FLAGS+=-display sdl
endif

QEMU_FLAGS_VIRTIO=-device virtio-rng-pci \
				 -device virtio-serial \
				 -nic user,model=virtio-net-pci \
				 -vga virtio

.PHONY: run-qemu
run: $(BOOTDISK)
	@echo [QEMU] $^
	@$(QEMU) $(QEMU_FLAGS) -enable-kvm -device ac97|| \
	 $(QEMU) $(QEMU_FLAGS) -enable-kvm -soundhw all

.PHONY: run-qemu-no-kvm
run-no-kvm: $(BOOTDISK)
	@echo [QEMU] $^
	@$(QEMU) $(QEMU_FLAGS) -device ac97|| \
	 $(QEMU) $(QEMU_FLAGS) -soundhw all

.PHONY: run-qemu-virtio
run-virtio: $(BOOTDISK)
	@echo [QEMU] $^
	@$(QEMU) $(QEMU_FLAGS) $(QEMU_FLAGS_VIRTIO) -enable-kvm -device ac97|| \
	 $(QEMU) $(QEMU_FLAGS) $(QEMU_FLAGS_VIRTIO) -enable-kvm -soundhw all
