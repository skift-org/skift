KERNEL_LDFLAGS += -z max-page-size=0x1000

KERNEL_SOURCES += $(wildcard kernel/archs/x86/*.cpp)

KERNEL_ASSEMBLY_SOURCES += $(wildcard kernel/archs/x86/*.s)
