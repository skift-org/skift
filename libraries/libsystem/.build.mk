LIBS += SYSTEM

SYSTEM_NAME = system
SYSTEM_CXXFLAGS = \
	-fno-tree-loop-distribute-patterns \
	-fno-rtti \
	-fno-exceptions
