#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

$(SRCDIR)/%_visitor.cxx: visitor/gen_visitor.py ast/ast_tmpl.h
	@$< $*_visitor --header-dst $(INCDIR)/$*_visitor.h --src-dst $@

$(INCDIR)/%_visitor.h: visitor/gen_visitor.py visitor/%_visitor_tmpl.h ast/ast_tmpl.h
	@$< $*_visitor --header-dst $@ --src-dst $(SRCDIR)/$*_visitor.cxx

$(OBJDIR)/%.o: visitor/%_visitor_handwritten.cxx $(INCDIR)/%_visitor.h $(INCDIR)/visitor.h $(INCDIR)/ast.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)

$(INCDIR)/visitor.h: visitor/gen_visitor.py visitor/visitor_tmpl.h ast/ast_tmpl.h
	@echo - generating visitor header file
	@$< base --header-dst $@
