$(CXX_MODULE_MAPPER): $(MODULEMAPS)
	$(DIRECTORY_GUARD)
	@echo [GLOBAL] [GENERATE-MODULEMAP] $(CXX_MODULE_MAPPER)
	@cat $^ > $@

cxx-modulemaps-list:
	@echo $(MODULEMAPS)

cxx-modulemaps-all: $(CXX_MODULE_MAPPER)

%.c++m:
	@echo IGNORE $@
