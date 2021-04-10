$(BOOTDISK).hdd: $(BOOTDISK)
	cp $(BOOTDISK) $(BOOTDISK).hdd

.PHONY: run-vbox
run: $(BOOTDISK).hdd
	@VBoxManage unregistervm --delete "skiftOS-dev" || echo "Look like it's the fist time you are running this command, this is fine"
	@VBoxManage createvm \
		--name skiftOS-dev \
		--ostype Other \
		--register \
		--basefolder $(shell pwd)/vm

	@VBoxManage modifyvm \
		skiftOS-dev \
		--memory $(CONFIG_MEMORY) \
		--uart1 0x3F8 4 \
		--uartmode1 tcpserver 1234

	@VBoxManage storagectl \
		skiftOS-dev \
		--name IDE \
		--add ide \

	@VBoxManage storageattach \
		skiftOS-dev \
		--storagectl IDE \
		--port 0 \
		--device 0 \
		--type hdd \
		--medium $(BOOTDISK).hdd

ifeq ($(CONFIG_DISPLAY),sdl)
	@vboxsdl --startvm skiftOS-dev &
	@sleep 1
else
	@VBoxManage startvm skiftOS-dev --type gui
endif
	nc localhost 1234
