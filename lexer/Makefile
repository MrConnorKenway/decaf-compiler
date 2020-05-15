#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

OBJ += $(OBJDIR)/lexer.o

$(OBJDIR)/lexer.o: $(SRCDIR)/lexer_gen.yy.cxx $(INCDIR)/parser_handwritten.tab.h $(INCDIR)/ast.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)

$(GENDIR)/lexer_gen.l: lexer/gen_lexer.py lexer/lexer_tmpl.l
	@echo - generating flex file
	@$< $@

$(SRCDIR)/lexer_gen.yy.cxx: $(GENDIR)/lexer_gen.l lexer/gen_lexer.py lexer/lexer_tmpl.l
	@echo - generating lexer source file
	@flex -o $@ $<
