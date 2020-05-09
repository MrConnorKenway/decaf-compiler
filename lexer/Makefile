#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

build/lexer_gen.yy.c: lexer/gen_lexer.py lexer/lexer_tmpl.l
	@echo - generating flex file
	@$<
	@echo - building lexer source file
	@flex -o $@ build/lexer_gen.l

all: build/lexer_gen.yy.c

.PHONY: all