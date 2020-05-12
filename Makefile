all:

include lexer/Makefile
include parser/Makefile

build: bin/decaf

bin/decaf: build/lexer_gen.yy.cxx build/parser_handwritten.tab.cxx
	@echo - building compiler
	@g++ -o $@ $< $(word 2, $^) -ly -std=c++11

clean:
	-rm bin/*
	-rm build/*

test-%: bin/decaf
	@echo - testing using $*.decaf
	@$< test/$*.decaf

.PHONY: clean build all