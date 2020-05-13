all:

include lexer/Makefile
include parser/Makefile
include ast/Makefile

build: bin/decaf

bin/decaf: build/lexer_gen.yy.cxx build/parser_handwritten.tab.cxx
	@echo - building compiler
	@g++ -o $@ $< $(word 2, $^) -ly -std=c++11 -g

clean:
	-rm bin/*
	-rm build/*

test-%: bin/decaf
	@echo - testing using $*.decaf
	@$< test/$*.decaf

.PHONY: clean build all