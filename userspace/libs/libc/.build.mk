LIBS += C

C_NAME = c

# -fno-tree-loop-distribute-patterns is needed because gcc
# like to optimise away our implementation of memcpy

C_CXXFLAGS = \
	-fno-tree-loop-distribute-patterns \
	-fno-rtti \
	-fno-exceptions \
	-DDISABLE_LOGGER

C_CFLAGS = -fno-tree-loop-distribute-patterns
