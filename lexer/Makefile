#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

build/lexer_gen.l: lexer/gen_lexer.py lexer/lexer_tmpl.l ast/ast.h
	@echo - generating flex file
	@$<

build/lexer_gen.yy.cxx: build/lexer_gen.l lexer/gen_lexer.py lexer/lexer_tmpl.l
	@echo - generating lexer source file
	@flex -+ -o $@ $<

all: build/lexer_gen.yy.cxx

.PHONY: all