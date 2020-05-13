#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

TMP_FILE += visitor/visitor.h

visitor/visitor.h: visitor/gen_visitor.py visitor/visitor_tmpl.h
	@echo - generating ast header file
	@$<

all: visitor/visitor.h

.PHONY: all
