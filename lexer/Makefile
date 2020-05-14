#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

OBJ += $(OBJDIR)/lexer.o

$(OBJDIR)/lexer.o: $(GENDIR)/lexer_gen.yy.cxx $(INCDIR)/parser_handwritten.tab.h $(INCDIR)/ast.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< -I./

$(GENDIR)/lexer_gen.l: lexer/gen_lexer.py lexer/lexer_tmpl.l
	@echo - generating flex file
	@$< $@

$(GENDIR)/lexer_gen.yy.cxx: $(GENDIR)/lexer_gen.l lexer/gen_lexer.py lexer/lexer_tmpl.l
	@echo - generating lexer source file
	@flex -+ -o $@ $<
