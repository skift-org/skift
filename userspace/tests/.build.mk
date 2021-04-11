TESTS_BINARY  = $(BUILD_DIRECTORY_APPS)/tests/tests

TESTS_SOURCES = $(wildcard userspace/tests/*.cpp) \
			    $(wildcard userspace/tests/*/*.cpp) \
				$(wildcard userspace/tests/*/*/*.cpp)

TESTS_OBJECTS = $(patsubst %.cpp, $(BUILDROOT)/%.o, $(TESTS_SOURCES))

TESTS_LIBS = graphic compression injection io system c

TARGETS += $(TESTS_BINARY)
OBJECTS += $(TESTS_OBJECTS)

$(TESTS_BINARY): $(TESTS_OBJECTS) $(patsubst %, $(BUILD_DIRECTORY_LIBS)/lib%.a, $(TESTS_LIBS)) $(CRTS)
	$(DIRECTORY_GUARD)
	@echo [TESTS] [LD] tests
	@$(CXX) $(LDFLAGS) -o $@ $(TESTS_OBJECTS) $(patsubst %, -l%, $(TESTS_LIBS))
	@if $(CONFIG_STRIP); then \
		echo [TESTS] [STRIP] tests; \
		$(STRIP) $@; \
	fi

$(BUILDROOT)/userspace/tests/%.o: userspace/tests/%.cpp
	$(DIRECTORY_GUARD)
	@echo [TESTS] [CXX] $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<