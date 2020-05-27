#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

OBJ += $(OBJDIR)/main.o $(OBJDIR)/static_semantic_analyser.o $(OBJDIR)/symbol_table.o

$(OBJDIR)/main.o: kern/main.cxx $(INCDIR)/display_visitor.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)

$(OBJDIR)/static_semantic_analyser.o: kern/static_semantic_analyser.cxx \
		kern/symbol_table.h kern/error.h \
		$(INCDIR)/static_semantic_analysis_visitor.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)

$(OBJDIR)/symbol_table.o: kern/symbol_table.cxx kern/symbol_table.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)
