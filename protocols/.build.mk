PROTOCOLS=$(wildcard protocols/*.ipc)

PROTOCOLS_SOURCES= \
	$(patsubst %.ipc,%Protocol.h, $(PROTOCOLS)) \
	$(patsubst %.ipc,%Server.h, $(PROTOCOLS)) \
	$(patsubst %.ipc,%Client.h, $(PROTOCOLS))

%Protocol.h: %.ipc
	ipc-compiler.py --protocol $< | clang-format > $@

%Server.h: %.ipc
	ipc-compiler.py --server $< | clang-format > $@

%Client.h: %.ipc
	ipc-compiler.py --client $< | clang-format > $@

TARGETS+=$(PROTOCOLS_SOURCES)
