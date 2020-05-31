all: build

BUILDDIR := build
BINDIR := $(BUILDDIR)/bin

build/Makefile:
	@mkdir -p build
	@cd build && cmake ..

build: build/Makefile
	@echo - building compiler
	@make -C build

gdb-%: 
	@gdb --args $(BINDIR)/decaf test/$*.decaf

clean:
	@rm -rf build/*

test-%: $(BINDIR)/decaf
	@echo - testing using $*.decaf
	@$< test/$*.decaf

.PHONY: clean build all