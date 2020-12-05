.DEFAULT_GOAL := all

TESTS=$(wildcard test_*.cpp)

CXXFLAGS:= \
	-MD \
	-std=c++20 \
	-I../libraries \
	-Idummies \
	-fsanitize=address \
	-fsanitize=undefined

%.out: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ $< common.cpp
	./$@
	@echo $@ SUCCESS

-include $(wildcard *.d)

all: $(patsubst %.cpp, %.out, $(TESTS))

clean:
	rm -f $(patsubst %.cpp, %.out, $(TESTS)) $(wildcard *.d)
