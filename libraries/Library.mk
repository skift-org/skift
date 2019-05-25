SOURCES = ${wildcard *.c} 
OBJECTS = ${SOURCES:.c=.o} 

install: $(LIBRARY).a
	cp $(LIBRARY).a $(SYSROOT)/lib

clean:
	rm -f $(OBJECTS) $(LIBRARY).a

$(LIBRARY).a: $(OBJECTS)
	ar rcs $(LIBRARY).a $(OBJECTS)