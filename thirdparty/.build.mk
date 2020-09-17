ECHFS:=thirdparty/echfs/echfs-utils
LIMINE:=thirdparty/limine/limine-install

$(ECHFS):
	make -C thirdparty/echfs/ all

$(LIMINE):
	make -C thirdparty/limine/ all
