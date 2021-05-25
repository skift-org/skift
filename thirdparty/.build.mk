ECHFS:=thirdparty/echfs/echfs-utils
LIMINE:=thirdparty/limine/limine-install
LIMINE_LOADER:=thirdparty/limine/limine.bin

$(ECHFS):
	cd thirdparty/echfs/ && $(MAKE) all

$(LIMINE):
	cd thirdparty/limine/ && $(MAKE) all

