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

$(BINDIR)/decaf: build

$(BINDIR)/%.out: $(BINDIR)/decaf test/%.decaf
	@$< test/$*.decaf -o $(BINDIR)/$*.out

run-%: $(BINDIR)/%.out
	@$(BINDIR)/$*.out

.PRECIOUS: $(BINDIR)/%.out

.PHONY: clean build all