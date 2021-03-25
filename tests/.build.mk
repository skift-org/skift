TESTS_BINARY  = $(BUILD_DIRECTORY_APPS)/tests/tests

TESTS_SOURCES = $(wildcard tests/*.cpp) \
			    $(wildcard tests/*/*.cpp) \
				$(wildcard tests/*/*/*.cpp)

TESTS_OBJECTS = $(patsubst %.cpp, $(CONFIG_BUILD_DIRECTORY)/%.o, $(TESTS_SOURCES))

TESTS_LIBS = graphic compression system injection io c

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

$(CONFIG_BUILD_DIRECTORY)/tests/%.o: tests/%.cpp
	$(DIRECTORY_GUARD)
	@echo [TESTS] [CXX] $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<