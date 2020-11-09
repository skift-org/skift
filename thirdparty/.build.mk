ECHFS:=thirdparty/echfs/echfs-utils
LIMINE:=thirdparty/limine/limine-install
LIMINE_LOADER:=thirdparty/limine/limine.bin

$(ECHFS):
	make -C thirdparty/echfs/ all

$(LIMINE):
	make -C thirdparty/limine/ limine-install

$(LIMINE_LOADER):
	make -C thirdparty/limine/ all

.PHONY: \
	$(ECHFS) \
	$(LIMINE) \
	$(LIMINE_LOADER)
