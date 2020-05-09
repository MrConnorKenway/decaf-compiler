all:

include lexer/Makefile
include parser/Makefile

build: bin/decaf

bin/decaf: build/lexer_gen.yy.c build/parser_gen.tab.c
	@echo - building compiler
	@gcc build/lexer_gen.yy.c build/parser_gen.tab.c -lfl -ly -o bin/decaf

clean:
	-rm -rf bin/*
	-rm -rf build/*

test-%: bin/decaf
	@echo - testing using $*.decaf
	@$< test/$*.decaf

.PHONY: clean build