#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

OBJ += $(OBJDIR)/ast.o

$(OBJDIR)/ast.o: $(SRCDIR)/ast.cxx $(INCDIR)/ast.h $(INCDIR)/visitor.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)

$(INCDIR)/ast.h: ast/gen_ast.py ast/ast_tmpl.h
	@echo - generating ast header and source file
	@$< --header-dst $@ --src-dst $(SRCDIR)/ast.cxx

$(SRCDIR)/ast.cxx: ast/gen_ast.py ast/ast_tmpl.h
	@echo - generating ast header and source file
	@$< --header-dst $(INCDIR)/ast.h --src-dst $@

.DELETE_ON_ERROR: $(INCDIR)/ast.h $(SRCDIR)/ast.cxx