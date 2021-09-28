
build/sysroot/System/Includes/%.h: userspace/libs/%.h
	$(DIRECTORY_GUARD)
	cp $< $@

build/sysroot/System/Includes/%.h: userspace/libs/libc/%.h
	$(DIRECTORY_GUARD)
	cp $< $@

.PHONY: crosscompiler-headers
crosscompiler-headers: $(patsubst $(BUILD_DIRECTORY_INCLUDE)/%, build/sysroot/System/Includes/%, $(HEADERS))
