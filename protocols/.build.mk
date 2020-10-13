PROTOCOLS=$(wildcard protocols/*.ipc)

PROTOCOLS_SOURCES= \
	$(patsubst %.ipc,%Protocol.h, $(PROTOCOLS)) \
	$(patsubst %.ipc,%Server.h, $(PROTOCOLS)) \
	$(patsubst %.ipc,%Client.h, $(PROTOCOLS)) \
	$(patsubst %.ipc,%ServerConnection.h, $(PROTOCOLS)) \
	$(patsubst %.ipc,%ClientConnection.h, $(PROTOCOLS))

%Protocol.h: %.ipc
	ipc-compiler.py --protocol $< > $@

%Server.h: %.ipc
	ipc-compiler.py --server $< > $@

%Client.h: %.ipc
	ipc-compiler.py --client $< > $@

%ServerConnection.h: %.ipc
	ipc-compiler.py --server-connection $< > $@

%ClientConnection.h: %.ipc
	ipc-compiler.py --client-connection $< > $@

TARGETS+=$(PROTOCOLS_SOURCES)
