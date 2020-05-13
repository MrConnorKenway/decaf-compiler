all:

TMP_FILE := 

include lexer/Makefile
include parser/Makefile
include ast/Makefile
include visitor/Makefile

build: bin/decaf

bin/decaf: build/lexer_gen.yy.cxx build/parser_handwritten.tab.cxx ast/ast.cxx
	@echo - building compiler
	@g++ -o $@ $< $(word 2, $^) $(word 3, $^) -ly -std=c++11 -g

clean:
	@-rm -rf bin/*
	@-rm -rf build/*
	@-rm -rf $(TMP_FILE)

test-%: bin/decaf
	@echo - testing using $*.decaf
	@$< test/$*.decaf

.PHONY: clean build all