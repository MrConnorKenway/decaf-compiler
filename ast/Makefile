#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

ast/ast.h: ast/gen_ast.py ast/ast_tmpl.h visitor/visitor_tmpl.h
	@echo - generating ast header file
	@$<
	@visitor/gen_visitor.py

all: ast/ast.h

.PHONY: all