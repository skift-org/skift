CSOURCES = ${wildcard *.c} 
SSOURCES = ${wildcard *.s} 
OBJECTS = ${CSOURCES:.c=.o} ${SOURCES:.s=.o} 

install: $(LIBRARY).a
	cp $(LIBRARY).a $(SYSROOT)/lib

clean:
	rm -f $(OBJECTS) $(LIBRARY).a

$(LIBRARY).a: $(OBJECTS)
	$(AR) rcs $(LIBRARY).a $(OBJECTS)