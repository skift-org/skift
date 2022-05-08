.SUFFIXES:
.DELETE_ON_ERROR:
.DEFAULT_GOAL:=all
SHELL:=/bin/bash
export LC_ALL=C


# --- Utils ------------------------------------------------------------------ #

BLANK:=
define NEWLINE


endef

MKCWD=mkdir -p $(@D)

# --- Target ----------------------------------------------------------------- #

SYS?=$(shell uname -s | tr '[:upper:]' '[:lower:]')
BOARD?=$(shell uname -i | tr '[:upper:]' '[:lower:]')
ARCH?=$(shell uname -m | tr '[:upper:]' '[:lower:]')

# Normalize aarch64 to arm64
ifeq ($(ARCH),aaarch64)
ARCH:=arm64
endif

TARGET=${SYS}-${BOARD}-${ARCH}

# --- Toolchain -------------------------------------------------------------- #

CXX?=g++
CXXFLAGS?=

LD?=ld
LDFLAGS?=

AS?=nasm
ASFLAGS?=

# --- Directories ------------------------------------------------------------ #

BINDIR=bin/${TARGET}
OBJDIR=obj/${TARGET}
SRCDIR=src

# --- Packages --------------------------------------------------------------- #

TEMPLATE:=.build/template.mk
include $(wildcard ${SRCDIR}/*/build.mk)

# --- Phonies ---------------------------------------------------------------- #

.PHONY: all
all:

.PHONY: dump
dump:
	$(info $(strip [$(META)]))
	@echo ""

.PHONY: clean
clean:
	rm -rf ${OBJDIR}

