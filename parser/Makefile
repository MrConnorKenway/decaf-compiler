#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

build/parser_gen.tab.c: parser/gen_parser.py parser/bnf parser/parser_tmpl.y
	@echo - generating bison file
	@$<
	@echo - building parser source file
	@bison -d build/parser_gen.y -v -Wall -o $@

all: build/parser_gen.tab.c

.PHONY: all