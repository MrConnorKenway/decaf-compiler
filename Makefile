all:

OBJ :=

BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj
SRCDIR := $(BUILDDIR)/src
GENDIR := $(BUILDDIR)/gen
INCDIR := $(BUILDDIR)/include
BINDIR := $(BUILDDIR)/bin

CFLAGS := -g -std=c++11 -I./

.SECONDARY:

include lexer/Makefile
include parser/Makefile
include ast/Makefile
include visitor/Makefile

dir:
	@mkdir -p build
	@mkdir -p build/obj
	@mkdir -p build/src
	@mkdir -p build/gen
	@mkdir -p build/include
	@mkdir -p build/bin

build-%: $(OBJDIR)/%.o $(OBJDIR)/%_main.o $(OBJ)
	@echo - building $* ast version of compiler
	@g++ -o $(BINDIR)/decaf $< $(word 2, $^) $(OBJ) $(CFLAGS)

gdb-%: 
	@gdb --args $(BINDIR)/decaf test/$*.decaf

clean:
	@find build/ -type f -delete

test-%: $(BINDIR)/decaf
	@echo - testing using $*.decaf
	@$< test/$*.decaf

.PHONY: clean build all dir