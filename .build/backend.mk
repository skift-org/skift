.SUFFIXES:
.DELETE_ON_ERROR:
.DEFAULT_GOAL:=all
SHELL:=/bin/bash
export LC_ALL=C

# --- Utils ------------------------------------------------------------------ #

BLANK:=
define NEWLINE


endef

MKCWD=@mkdir -p $(@D)

# --- Target ----------------------------------------------------------------- #

SYS?=$(shell uname -s | tr '[:upper:]' '[:lower:]')
ARCH?=$(shell uname -m | tr '[:upper:]' '[:lower:]')
BOARD?=unknown

# Normalize aarch64 to arm64
ifeq ($(ARCH),aaarch64)
ARCH:=arm64
endif

TARGET=${SYS}-${ARCH}-${BOARD}

# --- Toolchain -------------------------------------------------------------- #

CC=clang
CFLAGS=-std=gnu2x -Wall -Wextra -Werror -Isrc

CXX=clang++
CXXFLAGS=-std=gnu++20 -Wall -Wextra -Werror -Isrc

LD=ld
LDFLAGS=

AS=nasm
ASFLAGS=

# --- Directories ------------------------------------------------------------ #

BINDIR=bin/${TARGET}
OBJDIR=obj/${TARGET}
SRCDIR=src

# --- Packages --------------------------------------------------------------- #

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	$(MKCWD)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

TEMPLATE:=.build/template.mk
include $(wildcard ${SRCDIR}/*/build.mk)

# --- Phonies ---------------------------------------------------------------- #

.PHONY: all
all: $(ALL_BINS) $(ALL_LIBS)

.PHONY: dump
dump:
	$(info $(strip [$(ALL_META) null]))
	@echo ""

.PHONY: dump-libs
dump-libs:
	$(info $(ALL_LIBS))
	@echo ""

.PHONY: dump-bins
dump-bins:
	$(info $(ALL_BINS))
	@echo ""

.PHONY: clean
clean:
	rm -rf ${OBJDIR}

