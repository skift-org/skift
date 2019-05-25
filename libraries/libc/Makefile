USER_SOURCES= ${wildcard core/*.c} ${wildcard user/*.c}
KERNEL_SOURCES= ${wildcard core/*.c} ${wildcard kernel/*.c}

LIBC_OBJECTS=${USER_SOURCES:.c=.o} 
LIBK_OBJECTS=${KERNEL_SOURCES:.c=.o} 

install: libc.a libk.a
	cp libc.a $(SYSROOT)/lib
	cp libk.a $(SYSROOT)/lib

clean:
	rm -f $(LIBC_OBJECTS) $(LIBK_OBJECTS)

libc.a: $(LIBC_OBJECTS)
	ar rcs libc.a $(LIBC_OBJECTS)

libk.a: $(LIBK_OBJECTS)
	ar rcs libk.a $(LIBK_OBJECTS)
