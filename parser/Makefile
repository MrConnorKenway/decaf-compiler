#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

build/parser_gen.yxx: parser/gen_parser.py parser/bnf parser/parser_tmpl.yxx
	@echo - generating bison file
	@$<

parser/parser_handwritten.yxx: build/parser_gen.yxx ast/ast.h
	@echo - init parser_handwritten.yxx using parser_gen.yxx
	@parser/merge_hand_and_gen.py

build/parser_gen.tab.cxx: build/parser_gen.yxx parser/gen_parser.py parser/bnf parser/parser_tmpl.yxx
	@echo - generating parser source file
	@bison --defines=build/parser_gen.tab.h $< -v -Wall -o $@

build/parser_handwritten.tab.cxx: parser/parser_handwritten.yxx
	@echo - using handwritten bison file
	@echo - generating parser source file
	@bison --defines=build/parser_handwritten.tab.h $< -v -Wall -o $@

all: build/parser_gen.tab.cxx build/parser_handwritten.tab.cxx build/parser_gen.yxx

.PHONY: all init