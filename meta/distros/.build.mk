DISTRO_DIRECTORY:=meta/distros/$(BUILD_DISTRO)

-include $(DISTRO_DIRECTORY)/.build.mk

print-distro:
	echo $(BUILD_DISTRO)