.SUFFIXES:
.DELETE_ON_ERROR:
.DEFAULT_GOAL:=all
SHELL:=/bin/bash
export LC_ALL=C

# --- Utils ------------------------------------------------------------------ #

MKCWD=mkdir -p $(@D)

# --- Target ----------------------------------------------------------------- #

SYS?=$(shell uname -s)
BOARD?=$(shell uname -i)
ARCH?=$(shell uname -m)

TARGET=${SYS}-${BOARD}-${ARCH}

# --- Directories ------------------------------------------------------------ #

BINDIR=bin/${TARGET}
OBJDIR=obj/${TARGET}
SRCDIR=src

# --- Recipes ---------------------------------------------------------------- #


# --- Phonies ---------------------------------------------------------------- #

all:

clean:
	rm -rf ${OBJDIR}

