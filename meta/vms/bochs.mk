.PHONY: run-bochs
run: $(BOOTDISK)
	bochs -q -f vms/.bochsrc -rc vms/.bochsini