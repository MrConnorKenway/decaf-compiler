#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

OBJ += $(OBJDIR)/main.o $(OBJDIR)/static_semantic_analyser.o

$(OBJDIR)/main.o: kern/main.cxx $(INCDIR)/display_visitor.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)

$(OBJDIR)/static_semantic_analyser.o: kern/static_semantic_analyser.cxx kern/symbol_table.h kern/error.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)